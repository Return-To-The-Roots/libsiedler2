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

///////////////////////////////////////////////////////////////////////////////
// Header
#include "main.h"
#include "ArchivItem_Bitmap_Player.h"
#include "ArchivItem_Bitmap.h"
#include "ArchivItem_Palette.h"
#include "ArchivInfo.h"
#include "prototypen.h"
#include "types.h"
#include <fstream>
#include <EndianStream.h>
#include <vector>
#include <cstring>

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#if defined _WIN32 && defined _DEBUG && defined _MSC_VER
#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////////
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
 *
 *  @author FloSoft
 */
int libsiedler2::loader::WriteBMP(const std::string& file, const ArchivItem_Palette* palette, const ArchivInfo& items, long nr)
{
    struct BMHD
    {
        unsigned short header; // 2
        unsigned int size; // 6
        unsigned int reserved; // 10
        unsigned int offset; // 14
    } bmhd = { 0x4D42, 40, 0, 54 };

    struct BMIH
    {
        unsigned int length; // 4
        int width; // 8
        int height; // 12
        short planes; // 14
        short bbp; // 16
        unsigned int compression; // 20
        unsigned int size; // 24
        int xppm; // 28
        int yppm; // 32
        int clrused; // 36
        int clrimp; // 40
    } bmih = { 40, 0, 0, 1, 24, 0, 40, 0, 0, 0, 0 };

    if(file.empty())
        return 1;

    if(nr == -1)
    {
        // Bitmap in ArchivInfo suchen, erstes Bitmap wird geschrieben
        for(size_t i = 0; i < items.size(); ++i)
        {
            if(!items.get(i))
                continue;

            switch(items.get(i)->getBobType())
            {
                case BOBTYPE_BITMAP_PLAYER:
                case BOBTYPE_BITMAP_RLE:
                case BOBTYPE_BITMAP_SHADOW:
                case BOBTYPE_BITMAP_RAW:
                {
                    nr = static_cast<long>(i);
                } break;
                default:
                    nr = -1;
                    break;
            }
            if(nr != -1)
                break;
        }
    }

    // Haben wir eine gefunden?
    if(nr == -1)
        return 2;

    const baseArchivItem_Bitmap* bitmap = dynamic_cast<const baseArchivItem_Bitmap*>(items.get(nr));

    // Datei zum schreiben öffnen
    libendian::LittleEndianOFStream fs(file);

    // hat das geklappt?
    if(!fs)
        return 3;

    bmih.height = bitmap->getHeight();
    bmih.width = bitmap->getWidth();
    bmih.size = 0;

    bmhd.size = 0;

    // Bitmap-Header schreiben
    //if(libendian::le_write_uc((unsigned char*)&bmhd, 14, bmp.get()) != 14)
    //  return 4;
    fs << bmhd.header;
    fs << bmhd.size;
    fs << bmhd.reserved;
    fs << bmhd.offset;

    // Bitmap-Info-Header schreiben
    //if(libendian::le_write_uc((unsigned char*)&bmih, 40, bmp.get()) != 40)
    //  return 5;
    fs << bmih.length;
    fs << bmih.width;
    fs << bmih.height;
    fs << bmih.planes;
    fs << bmih.bbp;
    fs << bmih.compression;

    unsigned int bmihsizepos = fs.getPosition();
    fs << bmih.size;
    fs << bmih.xppm;
    fs << bmih.yppm;
    fs << bmih.clrused;
    fs << bmih.clrimp;

    // Farbpalette lesen
    unsigned char colors[256][4];

    // Farbpalette zuweisen
    for(int i = 0; i < bmih.clrused; ++i)
    {
        colors[i][3] = 0;
        palette->get(i, colors[i][2], colors[i][1], colors[i][0]);
    }

    // Farbpalette schreiben
    fs.write(colors[0], bmih.clrused * 4);

    std::vector<unsigned char> buffer(bmih.width * bmih.height * 4 + 1);

    /// @todo: bug im print?!?
    if(bitmap->getBobType() == BOBTYPE_BITMAP_PLAYER)
    {
        if(dynamic_cast<const baseArchivItem_Bitmap_Player*>(bitmap)->print(&buffer.front(), bmih.width, bmih.height, FORMAT_RGBA, palette, 128) != 0)
            return 7;
    }
    else if(bitmap->print(&buffer.front(), bmih.width, bmih.height, FORMAT_RGBA, palette) != 0)
        return 7;

    unsigned char placeholder[80];
    memset(placeholder, 0, 80);

    // Bottom-Up, "von unten nach oben"
    for(int y = bmih.height - 1; y >= 0; --y)
    {
        for(int x = 0; x < bmih.width; ++x)
        {
            switch(bmih.bbp)
            {
                case 8:
                {
                    unsigned char color = buffer[x + bmih.width * y];
                    fs << color;
                } break;
                case 24:
                {
                    unsigned char r = buffer[4 * (x + bmih.width * y) + 2];
                    unsigned char g = buffer[4 * (x + bmih.width * y) + 1];
                    unsigned char b = buffer[4 * (x + bmih.width * y) + 0];
                    if(buffer[4 * (x + bmih.width * y) + 3] == 0x00)
                    {
                        r = 0xff;
                        g = 0x00;
                        b = 0x8f;
                    }
                    fs << b << g << r;
                } break;
            }
        }
        if((bmih.width * bmih.bbp / 8) % 4 > 0)
            fs.write(placeholder, 4 - (bmih.width * bmih.bbp / 8) % 4);
    }
    if(fs.getPosition() % 4 > 0)
        fs.write(placeholder, 4 - fs.getPosition() % 4);

    unsigned int endsize = fs.getPosition();
    fs.setPosition(bmihsizepos);
    fs << (endsize - bmihsizepos);

    // alles ok
    return 0;
}
