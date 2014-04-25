// $Id: ArchivItem_Bob.cpp 9359 2014-04-25 15:37:22Z FloSoft $
//
// Copyright (c) 2005 - 2011 Settlers Freaks (sf-team at siedler25.org)
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
libsiedler2::ArchivItem_Bob::ArchivItem_Bob(void) : ArchivItem(), ArchivInfo(), good_count(0), item_count(0), links(NULL)
{
    setBobType(BOBTYPE_BOB);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Kopierkonstruktor von @p ArchivItem_Font.
 *
 *  @param[in] item Quellitem
 *
 *  @author FloSoft
 */
libsiedler2::ArchivItem_Bob::ArchivItem_Bob(const ArchivItem_Bob* item) : ArchivItem( (ArchivItem*)item ), ArchivInfo( (ArchivInfo*)item ), good_count(item->good_count), item_count(item->good_count)
{
    setBobType(BOBTYPE_BOB);

    links = new unsigned short[item->item_count];
    memcpy(links, item->links, sizeof(unsigned short)*item->item_count);
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
libsiedler2::ArchivItem_Bob::ArchivItem_Bob(FILE* file, const ArchivItem_Palette* palette) : ArchivItem(), ArchivInfo(), good_count(0), item_count(0), links(NULL)
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
{
    delete[] links;
}

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
    unsigned char* raw_base = new unsigned char[size];

    // Farbblock auslesen
    if(libendian::le_read_uc(raw_base, size, file) != size)
        return 3;

    // Einzelner Bilder auslesen ( untere Körper )
    for(unsigned int i = 0; i < 96; ++i)
    {
        baseArchivItem_Bitmap_Player* image = dynamic_cast<baseArchivItem_Bitmap_Player*>((*allocator)(BOBTYPE_BITMAP_PLAYER, 0, NULL));
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

        unsigned short* starts = new unsigned short[height];
        for(unsigned int j = 0; j < height; ++j)
        {
            if(libendian::le_read_us(&starts[j], file) != 0)
                return 7;
        }

        unsigned char ny;
        if(libendian::le_read_uc(&ny, 1, file) != 1)
            return 8;

        image->setNx(16);
        image->setNy(ny);

        if(image->load(32, height, raw_base, starts, true, size, palette) != 0)
            return 9;

        delete[] starts;

        set(i, image);
    }

    delete[] raw_base;

    // erstmal die 6 Farbblöcke fr die 6 Richtungen
    unsigned char* raw[6];
    unsigned short sizes[6];

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
        if(libendian::le_read_us(&sizes[i], file) != 0)
            return 11;

        raw[i] = new unsigned char[sizes[i]];

        // und auslesen
        if(libendian::le_read_uc(raw[i], sizes[i], file) != sizes[i])
            return 12;
    }

    // Anzahl Warenbilder
    if(libendian::le_read_us(&good_count, file) != 0)
        return 13;

    alloc_inc(good_count);

    bool* used = new bool[good_count];
    memset(used, 0, sizeof(bool)*good_count);

    unsigned char* heights = new unsigned char[good_count];
    unsigned short** starts = new unsigned short*[good_count];
    unsigned char* ny = new unsigned char[good_count];

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

        starts[i] = new unsigned short[heights[i]];
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

    links = new unsigned short[item_count];

    for(unsigned int i = 0; i < item_count; ++i)
    {
        if(libendian::le_read_us(&links[i], file) != 0)
            return 20;

        if(!used[links[i]])
        {
            baseArchivItem_Bitmap_Player* image = dynamic_cast<baseArchivItem_Bitmap_Player*>((*allocator)(BOBTYPE_BITMAP_PLAYER, 0, NULL));

            image->setNx(16);
            image->setNy(ny[links[i]]);

            if(image->load(32, heights[links[i]], raw[i % 6], starts[links[i]], true, sizes[i % 6], palette) != 0)
                return 21;

            set(96 + links[i], image);

            delete[] starts[links[i]];
            starts[links[i]] = NULL;
        }

        used[links[i]] = true;

        // 2 Unbekannte Null-Bytes überspringen
        fseek(file, 2, SEEK_CUR);
    }

    for(unsigned int i = 0; i < 6; ++i)
        delete[] raw[i];

    delete[] used;
    delete[] heights;
    delete[] starts;
    delete[] ny;

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
