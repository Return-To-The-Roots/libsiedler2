// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "Archiv.h"
#include "ArchivItem_Bitmap.h"
#include "ArchivItem_Bitmap_Player.h"
#include "ArchivItem_Palette.h"
#include "BmpHeader.h"
#include "ErrorCodes.h"
#include "fileFormatHelpers.h"
#include "prototypen.h"
#include "libendian/EndianOStreamAdapter.h"
#include <boost/nowide/fstream.hpp>
#include <vector>

/**
 *  schreibt ein Archiv in eine BMP-File.
 *
 *  @param[in] file    Dateiname der BMP-File
 *  @param[in] items   Archiv-Struktur, von welcher gelesen wird
 *  @param[in] palette Palette die für das Bitmap verwendet werden soll
 *
 *  @todo RGB Bitmaps (Farben > 8Bit) ebenfalls schreiben.
 *
 *  @return Null bei Erfolg, ein Wert ungleich Null bei Fehler
 */
int libsiedler2::loader::WriteBMP(const boost::filesystem::path& filepath, const Archiv& items,
                                  const ArchivItem_Palette* palette)
{
    if(filepath.empty())
        return ErrorCode::INVALID_BUFFER;

    const auto* bitmap = dynamic_cast<const ArchivItem_BitmapBase*>(items[0]);
    if(!bitmap)
        return ErrorCode::WRONG_ARCHIV;

    const uint16_t width = bitmap->getWidth();
    const uint16_t height = bitmap->getHeight();
    BmpFileHeader bmpHd;
    BitmapInfoHeader bmih{};
    setChunkId(bmpHd.header, "BM");
    bmpHd.reserved = 0;
    bmih.headerSize = sizeof(bmih);
    bmih.width = width;
    bmih.height = height;
    bmih.planes = 1;
    bmih.compression = 0;
    bmih.size = 0; // Valid for BI_RGB
    bmih.xppm = bmih.yppm = 2834;
    bmih.clrimp = 0;

    bool isPaletted;
    // Passed palette is only used for player bitmaps if the bitmap does not have one as our format supports unpaletted
    // images and the bitmap palette (or not having one) takes preference
    if(bitmap->getPalette())
    {
        bmih.clrused = 256;
        bmih.bpp = 8;
        isPaletted = true;
        // Prefer bitmaps palette
        palette = bitmap->getPalette();
    } else
    {
        bmih.clrused = 0;
        bmih.bpp = 24;
        isPaletted = false;
    }
    bmpHd.pixelOffset = sizeof(bmpHd) + sizeof(bmih) + bmih.clrused * 4; //-V119
    unsigned numLineAlignBytes = (bmih.width * bmih.bpp / 8) % 4;
    numLineAlignBytes = numLineAlignBytes == 0 ? 0 : 4 - numLineAlignBytes;
    bmpHd.fileSize = (width * bmih.bpp / 8 + numLineAlignBytes) * height + bmpHd.pixelOffset;

    // Datei zum schreiben öffnen
    libendian::EndianOStreamAdapter<false, boost::nowide::ofstream> fs(filepath, std::ios_base::binary);

    // hat das geklappt?
    if(!fs)
        return ErrorCode::FILE_NOT_ACCESSIBLE;

    if(!fs.writeRaw(&bmpHd, 1) || !fs.writeRaw(&bmih, 1))
        return ErrorCode::UNEXPECTED_EOF;

    if(bmih.clrused > 0)
    {
        uint8_t colors[256][4];
        for(int i = 0; i < bmih.clrused; i++)
        {
            palette->get(i).toBGR(&colors[i][0]);
            colors[i][3] = 0; // Last entry ('A') is always 0 in bmps
        }
        if(palette->hasTransparency())
        {
            // Write special transparent color
            TRANSPARENT_COLOR.toBGR(&colors[palette->getTransparentIdx()][0]);
        }
        fs.write(colors[0], bmih.clrused * 4);
    }

    std::vector<uint8_t> buffer(bmih.width * bmih.height * (isPaletted ? 1 : 4),
                                isPaletted ? palette->getTransparentIdx() : 0);
    TextureFormat bufFmt = isPaletted ? TextureFormat::Paletted : TextureFormat::BGRA;

    if(bitmap->getBobType() == BobType::BitmapPlayer)
    {
        const auto* bmpPl = dynamic_cast<const ArchivItem_Bitmap_Player*>(bitmap);
        if(!bmpPl)
            return ErrorCode::UNSUPPORTED_FORMAT;
        if(int ec = bmpPl->print(&buffer.front(), bmih.width, bmih.height, bufFmt, palette, 128))
            return ec;
    } else
    {
        const auto* bmpBase = dynamic_cast<const baseArchivItem_Bitmap*>(bitmap);
        if(!bmpBase)
            return ErrorCode::UNSUPPORTED_FORMAT;
        if(int ec = bmpBase->print(&buffer.front(), bmih.width, bmih.height, bufFmt, palette))
            return ec;
    }

    std::vector<uint8_t> lineAlignBytes(numLineAlignBytes);

    // Bottom-Up, "von unten nach oben"
    for(int y = height - 1; y >= 0; --y)
    {
        unsigned idx = width * y;
        if(isPaletted)
            fs.write(&buffer[idx], width);
        else
        {
            idx *= 4; // BGRA
            for(int x = 0; x < width; ++x, idx += 4)
            {
                ColorRGB clr;
                if(buffer[idx + 3] == 0x00)
                    fs << TRANSPARENT_COLOR.b << TRANSPARENT_COLOR.g << TRANSPARENT_COLOR.r;
                else
                    fs << buffer[idx] << buffer[idx + 1] << buffer[idx + 2]; // Buffer is BGRA
            }
        }
        if(!lineAlignBytes.empty())
            fs << lineAlignBytes;
    }

    assert(fs.getPosition() == static_cast<long>(bmpHd.fileSize));

    return (!fs) ? ErrorCode::UNEXPECTED_EOF : ErrorCode::NONE;
}
