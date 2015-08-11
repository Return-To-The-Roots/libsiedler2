// $Id: ArchivItem_Bob.cpp 9359 2014-04-25 15:37:22Z FloSoft $
//
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
#include "ArchivItem_Bob.h"
#include "ArchivItem_Bitmap_Player.h"
#include "types.h"
#include <libendian.h>

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#if defined _WIN32 && defined _DEBUG && defined _MSC_VER
#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////////
/** @class libsiedler2::ArchivItem_Bob
 *
 *  Klasse für Bobfiles.
 *
 *  @author FloSoft
 */

///////////////////////////////////////////////////////////////////////////////
/**
 *  Konstruktor von @p ArchivItem_Font.
 *
 *  @author FloSoft
 */
libsiedler2::ArchivItem_Bob::ArchivItem_Bob(void) : ArchivItem(), ArchivInfo(), good_count(0), item_count(0)
{
    setBobType(BOBTYPE_BOB);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Konstruktor von @p ArchivItem_Bob mit Laden der Bob-Daten aus
 *  einer Datei.
 *
 *  @param[in] file    Dateihandle aus denen die Bob-Daten geladen werden sollen
 *  @param[in] palette Grundpalette
 *
 *  @author FloSoft
 */
libsiedler2::ArchivItem_Bob::ArchivItem_Bob(FILE* file, const ArchivItem_Palette* palette) : ArchivItem(), ArchivInfo(), good_count(0), item_count(0)
{
    setBobType(BOBTYPE_BOB);

    load(file, palette);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Destruktor von @p ArchivItem_Bob.
 *
 *  @author FloSoft
 */
libsiedler2::ArchivItem_Bob::~ArchivItem_Bob(void)
{}

///////////////////////////////////////////////////////////////////////////////
/**
 *  lädt die Bobdaten aus einer Datei.
 *
 *  @param[in] file    Dateihandle der Datei
 *  @param[in] palette Grundpalette
 *
 *  @return liefert Null bei Erfolg, ungleich Null bei Fehler
 *
 *  @author FloSoft
 */
int libsiedler2::ArchivItem_Bob::load(FILE* file, const ArchivItem_Palette* palette)
{
    if(file == NULL || palette == NULL)
        return 1;

    alloc(96);

    // Größe des ersten Farbblocks auslesen
    unsigned short size;
    if(libendian::le_read_us(&size, file) != 0)
        return 2;

    // Speicher allokieren
    std::vector<unsigned char>raw_base(size);

    // Farbblock auslesen
    if(libendian::le_read_uc(&raw_base.front(), size, file) != size)
        return 3;

    // Einzelner Bilder auslesen ( untere Körper )
    for(unsigned int i = 0; i < 96; ++i)
    {
        unsigned short id;

        // ID lesen
        if(libendian::be_read_us(&id, file) != 0)
            return 4;

        // stimmt die ID? (ID 0xF401)
        if(id != 0xF401)
            return 5;

        unsigned char height;
        if(libendian::le_read_uc(&height, 1, file) != 1)
            return 6;

        std::vector<unsigned short> starts(height);
        for(unsigned int j = 0; j < height; ++j)
        {
            if(libendian::le_read_us(&starts[j], file) != 0)
                return 7;
        }

        unsigned char ny;
        if(libendian::le_read_uc(&ny, 1, file) != 1)
            return 8;

        baseArchivItem_Bitmap_Player* image = dynamic_cast<baseArchivItem_Bitmap_Player*>(getAllocator().create(BOBTYPE_BITMAP_PLAYER, 0));
        image->setNx(16);
        image->setNy(ny);

        if(image->load(32, height, &raw_base.front(), &starts.front(), true, size, palette) != 0){
            delete image;
            return 9;
        }

        set(i, image);
    }

    // erstmal die 6 Farbblöcke fr die 6 Richtungen
    std::vector<unsigned char> raw[6];

    for(unsigned int i = 0; i < 6; ++i)
    {
        unsigned short id;

        // ID lesen
        if(libendian::be_read_us(&id, file) != 0)
            return 7;

        // stimmt die ID? (ID 0xF401)
        if(id != 0xF501)
            return 10;

        // Größe des Farbblocks
        unsigned short size;
        if(libendian::le_read_us(&size, file) != 0)
            return 11;

        raw[i].resize(size);

        // und auslesen
        if(libendian::le_read_uc(&raw[i].front(), size, file) != size)
            return 12;
    }

    // Anzahl Warenbilder
    if(libendian::le_read_us(&good_count, file) != 0)
        return 13;

    alloc_inc(good_count);

    std::vector<bool> used(good_count);

    std::vector<unsigned char> heights(good_count);
    std::vector< std::vector<unsigned short> > starts(good_count);
    std::vector<unsigned char> ny(good_count);

    for(unsigned short i = 0; i < good_count; ++i)
    {
        unsigned short id;

        // ID lesen
        if(libendian::be_read_us(&id, file) != 0)
            return 14;

        // stimmt die ID? (ID 0xF401)
        if(id != 0xF401)
            return 15;

        if(libendian::le_read_uc(&heights[i], 1, file) != 1)
            return 16;

        starts[i].resize(heights[i]);
        for(unsigned int j = 0; j < heights[i]; ++j)
        {
            if(libendian::le_read_us(&starts[i][j], file) != 0)
                return 17;
        }

        if(libendian::le_read_uc(&ny[i], 1, file) != 1)
            return 18;
    }

    // Anzahl Bilder
    if(libendian::le_read_us(&item_count, file) != 0)
        return 19;

    links.resize(item_count);

    for(unsigned int i = 0; i < item_count; ++i)
    {
        if(libendian::le_read_us(&links[i], file) != 0)
            return 20;

        if(!used[links[i]])
        {
            baseArchivItem_Bitmap_Player* image = dynamic_cast<baseArchivItem_Bitmap_Player*>(getAllocator().create(BOBTYPE_BITMAP_PLAYER, 0));

            image->setNx(16);
            image->setNy(ny[links[i]]);

            if(image->load(32, heights[links[i]], &raw[i % 6].front(), &starts[links[i]].front(), true, raw[i % 6].size(), palette) != 0){
                delete image;
                return 21;
            }

            set(96 + links[i], image);

            starts[links[i]].clear();
        }

        used[links[i]] = true;

        // 2 Unbekannte Null-Bytes überspringen
        fseek(file, 2, SEEK_CUR);
    }

    return 0;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  schreibt die Bobdaten in eine Datei.
 *
 *  @param[in] file    Dateihandle der Datei
 *  @param[in] palette Grundpalette
 *
 *  @return liefert Null bei Erfolg, ungleich Null bei Fehler
 *
 *  @author FloSoft
 */
int libsiedler2::ArchivItem_Bob::write(FILE* file, const ArchivItem_Palette* palette) const
{
    if(file == NULL || palette == NULL)
        return 1;

    /// @todo Schreiben der Bobdaten.

    return 254;
}
