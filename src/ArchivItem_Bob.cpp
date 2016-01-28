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
#include <fstream>
#include <EndianStream.h>

// Include last!
#include "DebugNew.h"

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
libsiedler2::ArchivItem_Bob::ArchivItem_Bob(std::istream& file, const ArchivItem_Palette* palette) : ArchivItem(), ArchivInfo(), good_count(0), item_count(0)
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
int libsiedler2::ArchivItem_Bob::load(std::istream& file, const ArchivItem_Palette* palette)
{
    if(!file || palette == NULL)
        return 1;

    alloc(96);
    libendian::LittleEndianIStreamRef fs(file);
    // Größe des ersten Farbblocks auslesen
    unsigned short size;
    fs >> size;

    // Farbblock auslesen
    std::vector<unsigned char> raw_base(size);
    fs >> raw_base;

    // Einzelner Bilder auslesen ( untere Körper )
    for(unsigned int i = 0; i < 96; ++i)
    {
        unsigned short id;
        fs >> id;

        // stimmt die ID? (ID 0xF401)
        if(id != 0x01F4)
            return 5;

        unsigned char height;
        fs >> height;

        std::vector<unsigned short> starts(height);
        fs >> starts;

        unsigned char ny;
        fs >> ny;

        baseArchivItem_Bitmap_Player* image = dynamic_cast<baseArchivItem_Bitmap_Player*>(getAllocator().create(BOBTYPE_BITMAP_PLAYER));
        image->setNx(16);
        image->setNy(ny);

        if(image->load(32, height, raw_base, starts, true, palette) != 0){
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
        fs >> id;

        // stimmt die ID? (ID 0xF401)
        if(id != 0x01F5)
            return 10;

        // Größe des Farbblocks
        unsigned short size;
        fs >> size;

        raw[i].resize(size);
        fs >> raw[i];
    }

    // Anzahl Warenbilder
    fs >> good_count;

    alloc_inc(good_count);

    std::vector<bool> used(good_count);

    std::vector<unsigned char> heights(good_count);
    std::vector< std::vector<unsigned short> > starts(good_count);
    std::vector<unsigned char> ny(good_count);

    for(unsigned short i = 0; i < good_count; ++i)
    {
        unsigned short id;
        fs >> id;

        // stimmt die ID? (ID 0xF401)
        if(id != 0x01F4)
            return 15;

        fs >> heights[i];

        starts[i].resize(heights[i]);
        fs >> starts[i];

        fs >> ny[i];
    }

    // Anzahl Bilder
    fs >> item_count;

    links.resize(item_count);

    for(unsigned int i = 0; i < item_count; ++i)
    {
        fs >> links[i];

        if(!used[links[i]])
        {
            baseArchivItem_Bitmap_Player* image = dynamic_cast<baseArchivItem_Bitmap_Player*>(getAllocator().create(BOBTYPE_BITMAP_PLAYER));

            image->setNx(16);
            image->setNy(ny[links[i]]);

            if(image->load(32, heights[links[i]], raw[i % 6], starts[links[i]], true, palette) != 0){
                delete image;
                return 21;
            }

            set(96 + links[i], image);

            starts[links[i]].clear();
        }

        used[links[i]] = true;

        // 2 Unbekannte Null-Bytes überspringen
        fs.ignore(2);
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
int libsiedler2::ArchivItem_Bob::write(std::ostream& file, const ArchivItem_Palette* palette) const
{
    if(!file || palette == NULL)
        return 1;

    /// @todo Schreiben der Bobdaten.

    return 254;
}
