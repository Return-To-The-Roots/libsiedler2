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
#include "libendian/EndianIStreamAdapter.h"
#include <algorithm>
#include <memory>
#include <vector>

/**
 *  liest eine Bitmapzeile
 */
template<class T_FStream>
static inline void LoadBMP_ReadLine(T_FStream& bmp, uint16_t y, uint32_t width, uint8_t bbp,
                                    std::vector<uint8_t>& buffer, std::vector<uint8_t>& tmpBuffer)
{
    if(bbp == 1)
    {
        bmp.read(&buffer[y * width], width);
    } else
    {
        bmp >> tmpBuffer;
        for(uint16_t x = 0; x < width; ++x)
        {
            libsiedler2::ColorRGB clr = libsiedler2::ColorRGB::fromBGR(&tmpBuffer[x * 3]);
            libsiedler2::ColorBGRA clrOut;
            if(clr != libsiedler2::TRANSPARENT_COLOR)
                clrOut = clr;
            clrOut.toBGRA(&buffer[(y * width + x) * 4]);
        }
    }
    if(width * bbp % 4 > 0)
        bmp.ignore(4 - (width * bbp % 4));
}

/**
 *  lädt eine BMP-File in ein Archiv.
 *
 *  @param[in]  filepath    Dateiname der BMP-File
 *  @param[out] items   Archiv-Struktur, welche gefüllt wird
 *
 *  @return Null bei Erfolg, ein Wert ungleich Null bei Fehler
 *
 *  @todo RGB Bitmaps (Farben > 8Bit) ebenfalls einlesen.
 */
int libsiedler2::loader::LoadBMP(const boost::filesystem::path& filepath, Archiv& image,
                                 const ArchivItem_Palette* palette /*= nullptr*/)
{
    MMStream mmapStream;
    if(int ec = openMemoryStream(filepath, mmapStream))
        return ec;
    libendian::EndianIStreamAdapter<false, MMStream&> bmpFs(mmapStream);

    BmpFileHeader bmhd;

    if(!bmpFs.readRaw(&bmhd, 1))
        return ErrorCode::WRONG_HEADER;

    if(!isChunk(bmhd.header, "BM"))
        return ErrorCode::WRONG_HEADER;

    BitmapInfoHeader bmih;
    if(!bmpFs.readRaw(&bmih, 1))
        return ErrorCode::WRONG_HEADER;

    if(bmih.headerSize != sizeof(bmih))
        return ErrorCode::WRONG_HEADER;

    bool bottomup = false;
    if(bmih.height >= 0)
        bottomup = true;
    else
        bmih.height = -bmih.height;

    // nur eine Ebene
    if(bmih.planes != 1)
        return ErrorCode::WRONG_FORMAT;

    std::unique_ptr<baseArchivItem_Bitmap> bitmap(getAllocator().create<baseArchivItem_Bitmap>(BobType::Bitmap));
    bitmap->setName(filepath.string());

    // keine Kompression
    if(bmih.compression != 0)
        return ErrorCode::WRONG_FORMAT;

    // Einträge in der Farbtabelle
    if(bmih.clrused == 0)
        bmih.clrused = 1u << uint32_t(bmih.bpp); // 2^n

    if(bmih.bpp == 8)
    {
        unsigned numClrsUsed = std::min<unsigned>(bmih.clrused, 256);
        // Farbpalette lesen
        uint8_t colors[256][4];
        if(!bmpFs.read(colors[0], numClrsUsed * 4))
            return ErrorCode::UNEXPECTED_EOF;

        auto pal = getAllocator().create<ArchivItem_Palette>(BobType::Palette);
        for(unsigned i = 0; i < numClrsUsed; ++i)
        {
            ColorBGRA clr = ColorBGRA::fromBGRA(&colors[i][0]); // NOTE: Alpha is always 0!
            pal->set(i, clr);                                   //-V522
        }
        pal->setDefaultTransparentIdx();
        bitmap->setPalette(std::move(pal));
    }

    uint8_t bbp = (bmih.bpp / 8);
    if(bbp != 1 && bbp != 3)
        return ErrorCode::WRONG_FORMAT;
    std::vector<uint8_t> buffer(bmih.height * bmih.width * (bbp == 1 ? 1 : 4));
    std::vector<uint8_t> tmpBuffer(bmih.width * bbp);

    bmpFs.setPosition(bmhd.pixelOffset);

    // Bitmap Pixel einlesen
    if(bottomup)
    {
        // Bottom-Up, "von unten nach oben"
        for(int y = bmih.height - 1; y >= 0; --y)
            LoadBMP_ReadLine(bmpFs, y, bmih.width, bbp, buffer, tmpBuffer);
    } else
    {
        // Top-Down, "von oben nach unten"
        for(int y = 0; y < bmih.height; ++y)
            LoadBMP_ReadLine(bmpFs, y, bmih.width, bbp, buffer, tmpBuffer);
    }

    if(!bmpFs)
        return ErrorCode::UNEXPECTED_EOF;

    if(int ec = bitmap->create(bmih.width, bmih.height, &buffer[0], bmih.width, bmih.height,
                               (bbp == 1) ? TextureFormat::Paletted : TextureFormat::BGRA))
        return ec;
    if(ArchivItem_BitmapBase::getWantedFormat(bitmap->getFormat()) != bitmap->getFormat())
    {
        if(!bitmap->getPalette() && palette)
            bitmap->setPaletteCopy(*palette);
        if(int ec = bitmap->convertFormat(ArchivItem_BitmapBase::getWantedFormat(bitmap->getFormat())))
            return ec;
    }

    // Bitmap zuweisen
    image.push(std::move(bitmap));

    return (!bmpFs) ? ErrorCode::UNEXPECTED_EOF : ErrorCode::NONE;
}
