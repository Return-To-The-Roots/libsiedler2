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
#include "ArchivInfo.h"
#include "prototypen.h"
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
    struct BMHD
    {
        uint16_t header; // 2
        uint32_t size; // 6
        uint32_t reserved; // 10
        uint32_t offset; // 14
    } bmhd = { 0x4D42, 40, 0, 54 };

    struct BMIH
    {
        uint32_t length; // 4
        int32_t width; // 8
        int32_t height; // 12
        int16_t planes; // 14
        int16_t bbp; // 16
        uint32_t compression; // 20
        uint32_t size; // 24
        int32_t xppm; // 28
        int32_t yppm; // 32
        int32_t clrused; // 36
        int32_t clrimp; // 40
    } bmih = { 40, 0, 0, 1, 24, 0, 0, 2834, 2834, 0, 0 };

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
                nr = i;
                break;
            }
        }
    }

    // Haben wir eine gefunden?
    if(nr == -1)
        return 2;
    const ArchivItem_BitmapBase* bitmap = dynamic_cast<const ArchivItem_BitmapBase*>(items.get(nr));

    if(!palette)
        palette = bitmap->getPalette();

    if(palette)
    {
        bmih.clrused = 256;
        bmih.bbp = 8;
        bmih.size = bitmap->getWidth() * bitmap->getHeight();
    }else
    {
        bmih.clrused = 0;
        bmih.bbp = 24;
    }

    // Datei zum schreiben öffnen
    libendian::EndianOStreamAdapter<false, bfs::ofstream> fs(file, std::ios_base::binary);

    // hat das geklappt?
    if(!fs)
        return 3;

    bmih.height = bitmap->getHeight();
    bmih.width = bitmap->getWidth();

    // Bitmap-Header schreiben
    fs << bmhd.header;
    uint32_t bmhdsizepos = fs.getPosition();
    fs << bmhd.size;
    fs << bmhd.reserved;
    fs << bmhd.offset;

    // Bitmap-Info-Header schreiben
    fs << bmih.length;
    fs << bmih.width;
    fs << bmih.height;
    fs << bmih.planes;
    fs << bmih.bbp;
    fs << bmih.compression;
    fs << bmih.size;
    fs << bmih.xppm;
    fs << bmih.yppm;
    fs << bmih.clrused;
    fs << bmih.clrimp;

    // Farbpalette lesen
    uint8_t colors[256][4];

    // Farbpalette zuweisen
    for(int i = 0; i < bmih.clrused; ++i)
    {
        colors[i][3] = 0;
        palette->get(i, colors[i][2], colors[i][1], colors[i][0]);
    }
    colors[TRANSPARENT_INDEX][0] = TRANSPARENT_COLOR.b;
    colors[TRANSPARENT_INDEX][1] = TRANSPARENT_COLOR.g;
    colors[TRANSPARENT_INDEX][2] = TRANSPARENT_COLOR.r;

    // Farbpalette schreiben
    fs.write(colors[0], bmih.clrused * 4);

    std::vector<uint8_t> buffer(bmih.width * bmih.height * (palette ? 1 : 4), palette ? TRANSPARENT_INDEX : 0);
    TexturFormat bufFmt = palette ? FORMAT_PALETTED : FORMAT_RGBA;

    if(bitmap->getBobType() == BOBTYPE_BITMAP_PLAYER)
    {
        if(dynamic_cast<const ArchivItem_Bitmap_Player*>(bitmap)->print(&buffer.front(), bmih.width, bmih.height, bufFmt, palette, 128) != 0)
            return 7;
    }
    else if(dynamic_cast<const baseArchivItem_Bitmap*>(bitmap)->print(&buffer.front(), bmih.width, bmih.height, bufFmt, palette) != 0)
        return 7;

    unsigned lineAlignOff = (bmih.width * bmih.bbp / 8) % 4;
    std::vector<uint8_t> lineAlignBytes(lineAlignOff == 0 ? 0 : 4 - lineAlignOff);

    // Bottom-Up, "von unten nach oben"
    for(int y = bmih.height - 1; y >= 0; --y)
    {
        unsigned idx = bmih.width * y;
        if(bmih.bbp == 8)
            fs.write(&buffer[idx], bmih.width);
        else
        {
            idx *= 4; // ARGB
            for(int x = 0; x < bmih.width; ++x, idx += 4)
            {
                Color clr;
                if(buffer[idx + 3] == 0x00)
                    clr = TRANSPARENT_COLOR;
                else
                {
                    clr.b = buffer[idx + 0];
                    clr.g = buffer[idx + 1];
                    clr.r = buffer[idx + 2];
                }
                fs << clr.b << clr.g << clr.r;
            }
        }
        if(!lineAlignBytes.empty())
            fs << lineAlignBytes;
    }

    uint32_t endsize = fs.getPosition();
    fs.setPosition(bmhdsizepos);
    fs << endsize;

    // alles ok
    return (!fs) ? 99 : 0;
}
