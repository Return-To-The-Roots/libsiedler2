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
#include "ArchivItem_Bob.h"
#include "ArchivItem_Bitmap_Player.h"
#include "libsiedler2.h"
#include "IAllocator.h"
#include <iostream>
#include "libendian/src/EndianIStreamAdapter.h"

/** @class libsiedler2::ArchivItem_Bob
 *
 *  Klasse für Bobfiles.
 */

libsiedler2::ArchivItem_Bob::ArchivItem_Bob() : ArchivItem(), ArchivInfo(), numGoodImgs(0)
{
    setBobType(BOBTYPE_BOB);
}

/**
 *  Konstruktor von @p ArchivItem_Bob mit Laden der Bob-Daten aus
 *  einer Datei.
 *
 *  @param[in] file    Dateihandle aus denen die Bob-Daten geladen werden sollen
 *  @param[in] palette Grundpalette
 */
libsiedler2::ArchivItem_Bob::ArchivItem_Bob(std::istream& file, const ArchivItem_Palette* palette) : ArchivItem(), ArchivInfo(), numGoodImgs(0)
{
    setBobType(BOBTYPE_BOB);

    load(file, palette);
}

libsiedler2::ArchivItem_Bob::~ArchivItem_Bob()
{}

/**
 *  lädt die Bobdaten aus einer Datei.
 *
 *  @param[in] file    Dateihandle der Datei
 *  @param[in] palette Grundpalette
 *
 *  @return liefert Null bei Erfolg, ungleich Null bei Fehler
 */
int libsiedler2::ArchivItem_Bob::load(std::istream& file, const ArchivItem_Palette* palette)
{
    if(!file || palette == NULL)
        return 1;

    libendian::EndianIStreamAdapter<false, std::istream&> fs(file);
    // Größe des ersten Farbblocks auslesen
    uint16_t size;
    fs >> size;

    // Farbblock auslesen
    std::vector<uint8_t> raw_base(size);
    fs >> raw_base;

    // Einzelner Bilder auslesen ( untere Körper ): 8 Animation Steps, 6 directions, 2 types (Fat, Non-Fat) = 96
    alloc(96);
    for(uint32_t i = 0; i < 96; ++i)
    {
        uint16_t id;
        uint8_t height;
        if(!(fs >> id >> height))
            return 4;

        // stimmt die ID? (ID 0xF401)
        if(id != 0x01F4)
            return 5;

        std::vector<uint16_t> starts(height);
        uint8_t ny;
        if(!(fs >> starts >> ny))
            return 6;

        ArchivItem_Bitmap_Player* image = dynamic_cast<ArchivItem_Bitmap_Player*>(getAllocator().create(BOBTYPE_BITMAP_PLAYER));
        image->setNx(16);
        image->setNy(ny);

        if(image->load(32, height, raw_base, starts, true, palette) != 0){
            delete image;
            return 9;
        }

        set(i, image);
    }

    // erstmal die 6 Farbblöcke fr die 6 Richtungen
    std::vector<uint8_t> raw[6];

    for(uint32_t i = 0; i < 6; ++i)
    {
        uint16_t id, size;
        if(!(fs >> id >> size))
            return 7;

        // stimmt die ID? (ID 0xF401)
        if(id != 0x01F5)
            return 10;

        raw[i].resize(size);
        fs >> raw[i];
    }

    // Anzahl Warenbilder
    fs >> numGoodImgs;

    alloc_inc(numGoodImgs);

    std::vector<bool> loaded(numGoodImgs, false);

    std::vector<uint8_t> heights(numGoodImgs);
    std::vector< std::vector<uint16_t> > starts(numGoodImgs);
    std::vector<uint8_t> ny(numGoodImgs);

    for(uint16_t i = 0; i < numGoodImgs; ++i)
    {
        uint16_t id;
        if(!(fs >> id >> heights[i]))
            return 14;

        // stimmt die ID? (ID 0xF401)
        if(id != 0x01F4)
            return 15;

        starts[i].resize(heights[i]);
        fs >> starts[i] >> ny[i];
    }

    // Number of complete pictures.
    // Links form an array: [ware][animStep][fat][direction]: [][8][2][6]
    // the item at position 96 + link[ware][animStep][fat][direction] shall be combined
    // with the appropriate body
    uint16_t item_count;
    if(!(fs >> item_count))
        return 16;

    links.resize(item_count);

    for(uint32_t i = 0; i < item_count; ++i)
    {
        uint16_t unknown;
        if(!(fs >> links[i] >> unknown))
            return 22;

        if(loaded[links[i]])
            continue;

        ArchivItem_Bitmap_Player* image = dynamic_cast<ArchivItem_Bitmap_Player*>(getAllocator().create(BOBTYPE_BITMAP_PLAYER));

        image->setNx(16);
        image->setNy(ny[links[i]]);

        if(image->load(32, heights[links[i]], raw[i % 6], starts[links[i]], true, palette) != 0){
            delete image;
            return 21;
        }

        set(96 + links[i], image);
        loaded[links[i]] = true;
    }

    return (!fs) ? 99 : 0;
}

/**
 *  schreibt die Bobdaten in eine Datei.
 *
 *  @param[in] file    Dateihandle der Datei
 *  @param[in] palette Grundpalette
 *
 *  @return liefert Null bei Erfolg, ungleich Null bei Fehler
 */
int libsiedler2::ArchivItem_Bob::write(std::ostream& file, const ArchivItem_Palette* palette) const
{
    if(!file || palette == NULL)
        return 1;

    /// @todo Schreiben der Bobdaten.

    return 254;
}
