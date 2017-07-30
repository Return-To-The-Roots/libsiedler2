// Copyright (c) 2005 - 2015 Settlers Freaks (sf-team at siedler25.org)
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
#include "ArchivItem_Bitmap_Player.h"
#include "ArchivItem_Bitmap.h"
#include "ArchivItem_Palette.h"
#include "ColorARGB.h"
#include "ArchivInfo.h"
#include "prototypen.h"
#include "BmpHeader.h"
#include "libendian/src/EndianOStreamAdapter.h"
#include <boost/filesystem/fstream.hpp>
#include <vector>
#include <cstring>

/**
 *  schreibt ein ArchivInfo in eine BMP-File.
 *
 *  @param[in] file    Dateiname der BMP-File
 *  @param[in] items   ArchivInfo-Struktur, von welcher gelesen wird
 *  @param[in] palette Palette die für das Bitmap verwendet werden soll
 *  @param[in] nr      Nummer des Bitmaps das geschrieben werden soll
                       (@p -1 erste Bitmap das gefunden wird)
 *
 *  @todo RGB Bitmaps (Farben > 8Bit) ebenfalls schreiben.
 *
 *  @return Null bei Erfolg, ein Wert ungleich Null bei Fehler
 */
int libsiedler2::loader::WriteBMP(const std::string& file, const ArchivItem_Palette* palette, const ArchivInfo& items, long nr)
{
    if(file.empty())
        return 1;

    if(nr == -1)
    {
        // Bitmap in ArchivInfo suchen, erstes Bitmap wird geschrieben
        for(size_t i = 0; i < items.size(); ++i)
        {
            const ArchivItem_BitmapBase* bitmap = dynamic_cast<const ArchivItem_BitmapBase*>(items.get(i));
            if(bitmap)
            {
                nr = static_cast<long>(i);
                break;
            }
        }
    }

    // Haben wir eine gefunden?
    if(nr == -1)
        return 2;
    const ArchivItem_BitmapBase* bitmap = dynamic_cast<const ArchivItem_BitmapBase*>(items.get(nr));

    if(!palette && bitmap->getFormat() == FORMAT_PALETTED)
        palette = bitmap->getPalette();

    BmpFileHeader bmpHd;
    BitmapInfoHeader bmih;
    bmpHd.header[0] = 'B'; bmpHd.header[1] = 'M';
    bmpHd.reserved = 0;
    bmih.headerSize = sizeof(bmih);
    bmih.width = bitmap->getWidth();
    bmih.height = bitmap->getHeight();
    bmih.planes = 1;
    bmih.compression = 0;
    bmih.size = 0; // Valid for BI_RGB
    bmih.xppm = bmih.yppm = 2834;
    bmih.clrimp = 0;

    if(palette)
    {
        bmih.clrused = 256;
        bmih.bpp = 8;
    }else
    {
        bmih.clrused = 0;
        bmih.bpp = 24;
    }
    bmpHd.pixelOffset = sizeof(bmpHd) + sizeof(bmih) + bmih.clrused * 4;
    unsigned numLineAlignBytes = (bmih.width * bmih.bpp / 8) % 4;
    numLineAlignBytes = numLineAlignBytes == 0 ? 0 : 4 - numLineAlignBytes;
    bmpHd.fileSize = (bitmap->getWidth() * bmih.bpp / 8 + numLineAlignBytes) * bitmap->getHeight() + bmpHd.pixelOffset;

    // Datei zum schreiben öffnen
    libendian::EndianOStreamAdapter<false, bfs::ofstream> fs(file, std::ios_base::binary);

    // hat das geklappt?
    if(!fs)
        return 3;

    if(!fs.writeRaw(&bmpHd, 1))
        return 4;
    if(!fs.writeRaw(&bmih, 1))
        return 5;

    if(bmih.clrused > 0)
    {
        uint8_t colors[256][4];
        palette->copy(&colors[0][0], sizeof(colors), true);
        fs.write(colors[0], bmih.clrused * 4);
    }

    std::vector<uint8_t> buffer(bmih.width * bmih.height * (palette ? 1 : 4), palette ? TRANSPARENT_INDEX : 0);
    TexturFormat bufFmt = palette ? FORMAT_PALETTED : FORMAT_BGRA;

    if(bitmap->getBobType() == BOBTYPE_BITMAP_PLAYER)
    {
        if(!dynamic_cast<const ArchivItem_Bitmap_Player*>(bitmap)->print(&buffer.front(), bmih.width, bmih.height, bufFmt, palette, 128))
            return 7;
    }
    else if(!dynamic_cast<const baseArchivItem_Bitmap*>(bitmap)->print(&buffer.front(), bmih.width, bmih.height, bufFmt, palette))
        return 7;

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

    assert(fs.getPosition() == bmpHd.fileSize);

    // alles ok
    return (!fs) ? 99 : 0;
}
