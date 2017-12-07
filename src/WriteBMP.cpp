// Copyright (c) 2005 - 2017 Settlers Freaks (sf-team at siedler25.org)
//
// This file is part of Return To The Roots.
//
// Return To The Roots is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// Return To The Roots is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Return To The Roots. If not, see <http://www.gnu.org/licenses/>.

#include "libSiedler2Defines.h" // IWYU pragma: keep
#include "Archiv.h"
#include "ArchivItem_Bitmap.h"
#include "ArchivItem_Bitmap_Player.h"
#include "ArchivItem_Palette.h"
#include "BmpHeader.h"
#include "ColorARGB.h"
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
int libsiedler2::loader::WriteBMP(const std::string& file, const Archiv& items, const ArchivItem_Palette* palette)
{
    if(file.empty())
        return ErrorCode::INVALID_BUFFER;

    const ArchivItem_BitmapBase* bitmap = dynamic_cast<const ArchivItem_BitmapBase*>(items[0]);
    if(!bitmap)
        return ErrorCode::WRONG_ARCHIV;

    BmpFileHeader bmpHd;
    BitmapInfoHeader bmih;
    setChunkId(bmpHd.header, "BM");
    bmpHd.reserved = 0;
    bmih.headerSize = sizeof(bmih);
    bmih.width = bitmap->getWidth();
    bmih.height = bitmap->getHeight();
    bmih.planes = 1;
    bmih.compression = 0;
    bmih.size = 0; // Valid for BI_RGB
    bmih.xppm = bmih.yppm = 2834;
    bmih.clrimp = 0;

    bool isPaletted;
    // Passed palette is only used for player bitmaps if the bitmap does not have one as our format supports unpaletted images and the
    // bitmap palette (or not having one) takes preference
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
    bmpHd.pixelOffset = sizeof(bmpHd) + sizeof(bmih) + bmih.clrused * 4;
    unsigned numLineAlignBytes = (bmih.width * bmih.bpp / 8) % 4;
    numLineAlignBytes = numLineAlignBytes == 0 ? 0 : 4 - numLineAlignBytes;
    bmpHd.fileSize = (bitmap->getWidth() * bmih.bpp / 8 + numLineAlignBytes) * bitmap->getHeight() + bmpHd.pixelOffset;

    // Datei zum schreiben öffnen
    libendian::EndianOStreamAdapter<false, bnw::ofstream> fs(file, std::ios_base::binary);

    // hat das geklappt?
    if(!fs)
        return ErrorCode::FILE_NOT_ACCESSIBLE;

    if(!fs.writeRaw(&bmpHd, 1) || !fs.writeRaw(&bmih, 1))
        return ErrorCode::UNEXPECTED_EOF;

    if(bmih.clrused > 0)
    {
        uint8_t colors[256][4];
        palette->copyToBGRA(&colors[0][0], sizeof(colors), true);
        fs.write(colors[0], bmih.clrused * 4);
    }

    std::vector<uint8_t> buffer(bmih.width * bmih.height * (isPaletted ? 1 : 4), isPaletted ? palette->transparentIdx : 0);
    TextureFormat bufFmt = isPaletted ? FORMAT_PALETTED : FORMAT_BGRA;

    if(bitmap->getBobType() == BOBTYPE_BITMAP_PLAYER)
    {
        const ArchivItem_Bitmap_Player* bmpPl = dynamic_cast<const ArchivItem_Bitmap_Player*>(bitmap);
        if(!bmpPl)
            return ErrorCode::UNSUPPORTED_FORMAT;
        if(int ec = bmpPl->print(&buffer.front(), bmih.width, bmih.height, bufFmt, palette, 128))
            return ec;
    } else
    {
        const baseArchivItem_Bitmap* bmpBase = dynamic_cast<const baseArchivItem_Bitmap*>(bitmap);
        if(!bmpBase)
            return ErrorCode::UNSUPPORTED_FORMAT;
        if(int ec = bmpBase->print(&buffer.front(), bmih.width, bmih.height, bufFmt, palette))
            return ec;
    }

    std::vector<uint8_t> lineAlignBytes(numLineAlignBytes);

    // Bottom-Up, "von unten nach oben"
    for(int y = bmih.height - 1; y >= 0; --y)
    {
        unsigned idx = bmih.width * y;
        if(bmih.bpp == 8)
            fs.write(&buffer[idx], bmih.width);
        else
        {
            idx *= 4; // BGRA
            for(int x = 0; x < bmih.width; ++x, idx += 4)
            {
                ColorRGB clr;
                if(buffer[idx + 3] == 0x00)
                    clr = TRANSPARENT_COLOR;
                else
                    clr = ColorRGB::fromBGR(&buffer[idx]);
                fs << clr.b << clr.g << clr.r;
            }
        }
        if(!lineAlignBytes.empty())
            fs << lineAlignBytes;
    }

    assert(fs.getPosition() == static_cast<long>(bmpHd.fileSize));

    return (!fs) ? ErrorCode::UNEXPECTED_EOF : ErrorCode::NONE;
}
