// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "Archiv.h"
#include "ArchivItem_Bitmap.h"
#include "ArchivItem_Palette.h"
#include "BmpHeader.h"
#include "ColorBGRA.h"
#include "ErrorCodes.h"
#include "IAllocator.h"
#include "OpenMemoryStream.h"
#include "fileFormatHelpers.h"
#include "libsiedler2.h"
#include "prototypen.h"
#include <algorithm>
#include <memory>
#include <vector>

namespace libsiedler2 {

template<class T_FStream>
static int read_palette(ArchivItem_BitmapBase& bitmap, T_FStream& bmpFs, const BitmapInfoHeader& bmih)
{
    unsigned numClrsUsed = bmih.clrused;
    if(numClrsUsed == 0 && bmih.bpp <= 8)
        numClrsUsed = 1u << static_cast<unsigned>(bmih.bpp);

    if(numClrsUsed > 0)
    {
        std::array<ColorBGRA, 256> colors;
        if(!bmpFs.read(reinterpret_cast<char*>(&colors[0]), numClrsUsed * 4))
            return ErrorCode::UNEXPECTED_EOF;

        auto pal = getAllocator().create<ArchivItem_Palette>(BobType::Palette);
        for(unsigned i = 0; i < numClrsUsed; ++i)
            pal->set(i, colors[i]);
        pal->setDefaultTransparentIdx();
        bitmap.setPalette(std::move(pal));
    }
    return ErrorCode::NONE;
}

/// Read the actual color data.
template<uint8_t bpp> // Template to avoid runtime condition in the loop
static std::vector<uint8_t> read_color_data(std::istream& bmpFs, const TextureFormat format, const uint32_t width,
                                            const uint32_t height, const bool bottomUp)
{
    const size_t outLineSize = width * ((format == TextureFormat::Paletted) ? 1 : 4);
    std::vector<uint8_t> buffer(height * outLineSize);
    // Lines are padded to a multiple of 4 byte
    std::vector<uint8_t> lineBuffer(((bpp * width + 31) / 32) * 4);

    // Get start point (first or last row) and increase (+/- 1 line)
    auto* itBuffer = buffer.data();
    std::ptrdiff_t itBufferIncrease = outLineSize;
    if(bottomUp)
    {
        itBuffer += itBufferIncrease * (height - 1);
        itBufferIncrease = -itBufferIncrease;
    }

    const auto numSkipBytes = lineBuffer.size() - outLineSize;
    for(unsigned y = 0; y < height; ++y, itBuffer += itBufferIncrease)
    {
        if(bpp == 8 || bpp == 32) // No conversion required for 8 BPP or 32 BPP. Read directly into buffer
        {
            bmpFs.read(reinterpret_cast<char*>(itBuffer), outLineSize);
            if(numSkipBytes)
                bmpFs.seekg(numSkipBytes, std::ios::cur);
        } else
        {
            bmpFs.read(reinterpret_cast<char*>(lineBuffer.data()), lineBuffer.size());
            if(bpp == 4)
            {
                auto it = lineBuffer.begin();
                auto* itOut = itBuffer;
                auto* itOutEnd = itOut + width - width % 2;
                for(; itOut != itOutEnd; ++itOut)
                {
                    const uint8_t curByte = *it++;
                    *itOut = curByte >> 4;
                    *(++itOut) = curByte & 0x0F;
                }
                if(width % 2)
                    *itOut = *it >> 4;
            } else if(bpp == 24)
            {
                auto* itOut = itBuffer;
                for(uint32_t x = 0; x < width; ++x, itOut += 4)
                {
                    libsiedler2::ColorRGB clr = libsiedler2::ColorRGB::fromBGR(&lineBuffer[x * 3]);
                    libsiedler2::ColorBGRA clrOut;
                    if(clr != libsiedler2::TRANSPARENT_COLOR)
                        clrOut = clr;
                    clrOut.toBGRA(&*itOut);
                }
            }
        }
    }

    assert(!!bmpFs);
    return buffer;
}

int loader::LoadBMP(const boost::filesystem::path& filepath, Archiv& image,
                    const ArchivItem_Palette* palette /*= nullptr*/)
{
    MMStream bmpFs;
    if(int ec = openMemoryStream(filepath, bmpFs))
        return ec;

    BmpFileHeader bmhd;

    if(!bmpFs.read(reinterpret_cast<char*>(&bmhd), sizeof(bmhd)))
        return ErrorCode::WRONG_HEADER;

    if(!isChunk(bmhd.header, "BM"))
        return ErrorCode::WRONG_HEADER;

    BitmapInfoHeader bmih;
    if(!bmpFs.read(reinterpret_cast<char*>(&bmih), sizeof(bmih)))
        return ErrorCode::WRONG_HEADER;

    if(bmih.headerSize != sizeof(bmih))
    {
        if(bmih.headerSize == sizeof(BitmapInfoHeaderV5))
        {
            bmpFs.seekg(-std::streamoff(sizeof(bmih)), std::ios::cur);
            BitmapInfoHeaderV5 bmihV5;
            if(!bmpFs.read(reinterpret_cast<char*>(&bmihV5), sizeof(bmihV5)))
                return ErrorCode::WRONG_HEADER;
            const auto compression(static_cast<BmpCompression>(bmihV5.compression.value()));
            if(compression == BmpCompression::Bitfield)
            {
                // We support only (A)RGB with default bit mask (24/32 bit per pixel)
                if(bmihV5.bpp != 32 && bmihV5.bpp != 24)
                    return ErrorCode::UNSUPPORTED_FORMAT;
                if(bmihV5.redMask != 0x00FF0000 || bmihV5.greenMask != 0x0000FF00 || bmihV5.blueMask != 0x000000FF)
                    return ErrorCode::UNSUPPORTED_FORMAT;
                if(bmihV5.bpp == 32 && bmihV5.alphaMask != 0xFF000000)
                    return ErrorCode::UNSUPPORTED_FORMAT;
                // This basically means: uncompressed
                bmihV5.compression = static_cast<uint32_t>(BmpCompression::RGB);
            }
            // Simply treat as standard format and ignore color space etc.
            // The only currently supported (semi-)compressed format is already checked above
            bmih = static_cast<BitmapInfoHeader>(bmihV5);
        } else
            return ErrorCode::UNSUPPORTED_FORMAT;
    }

    bool bottomup = false;
    if(bmih.height >= 0)
        bottomup = true;
    else
        bmih.height = -bmih.height;

    // Only 1 plane allowed
    if(bmih.planes != 1)
        return ErrorCode::WRONG_FORMAT;

    // Compression not yet supported
    if(bmih.compression != 0)
        return ErrorCode::UNSUPPORTED_FORMAT;

    TextureFormat format;
    switch(bmih.bpp)
    {
        case 4: BOOST_FALLTHROUGH;
        case 8: format = TextureFormat::Paletted; break;
        case 16: return ErrorCode::UNSUPPORTED_FORMAT;
        case 24:
        case 32: format = TextureFormat::BGRA; break;
        default: return ErrorCode::WRONG_FORMAT;
    }

    std::unique_ptr<baseArchivItem_Bitmap> bitmap(getAllocator().create<baseArchivItem_Bitmap>(BobType::Bitmap));
    bitmap->setName(filepath.string());

    if(format == TextureFormat::Paletted)
    {
        if(int ec = read_palette(*bitmap, bmpFs, bmih))
            return ec;
    }

    // Now read pixel data
    std::vector<uint8_t> buffer;
    try
    {
        bmpFs.exceptions(std::ios::failbit | std::ios::badbit | std::ios::eofbit);
        bmpFs.seekg(bmhd.pixelOffset, std::ios::beg);
        switch(bmih.bpp)
        {
            case 4: buffer = read_color_data<4>(bmpFs, format, bmih.width, bmih.height, bottomup); break;
            case 8: buffer = read_color_data<8>(bmpFs, format, bmih.width, bmih.height, bottomup); break;
            case 24: buffer = read_color_data<24>(bmpFs, format, bmih.width, bmih.height, bottomup); break;
            case 32: buffer = read_color_data<32>(bmpFs, format, bmih.width, bmih.height, bottomup); break;
            default: return ErrorCode::WRONG_FORMAT;
        }
    } catch(const std::ios::failure&)
    {
        return ErrorCode::UNEXPECTED_EOF;
    }

    if(int ec = bitmap->create(bmih.width, bmih.height, &buffer[0], bmih.width, bmih.height, format))
        return ec;
    if(ArchivItem_BitmapBase::getWantedFormat(bitmap->getFormat()) != bitmap->getFormat())
    {
        if(!bitmap->getPalette() && palette)
            bitmap->setPaletteCopy(*palette);
        if(int ec = bitmap->convertFormat(ArchivItem_BitmapBase::getWantedFormat(bitmap->getFormat())))
            return ec;
    }

    image.push(std::move(bitmap));

    return (!bmpFs) ? ErrorCode::UNEXPECTED_EOF : ErrorCode::NONE;
}
} // namespace libsiedler2
