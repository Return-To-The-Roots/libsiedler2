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

#include "ArchivItem_Bob.h"
#include "ArchivItem_Bitmap_Player.h"
#include "ErrorCodes.h"
#include "IAllocator.h"
#include "libsiedler2.h"
#include "libendian/EndianIStreamAdapter.h"
#include <iostream>

/** @class libsiedler2::ArchivItem_Bob
 *
 *  Klasse für Bobfiles.
 */

libsiedler2::ArchivItem_Bob::ArchivItem_Bob() : ArchivItem(BOBTYPE_BOB), numGoodImgs(0) {}

libsiedler2::ArchivItem_Bob::~ArchivItem_Bob() = default;

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
    if(!file)
        return ErrorCode::FILE_NOT_ACCESSIBLE;
    if(!palette)
        return ErrorCode::PALETTE_MISSING;

    libendian::EndianIStreamAdapter<false, std::istream&> fs(file);
    // Größe des ersten Farbblocks auslesen
    uint16_t size;
    if(!(fs >> size))
        return ErrorCode::WRONG_HEADER;

    // Farbblock auslesen
    std::vector<uint8_t> raw_base(size);
    if(!(fs >> raw_base))
        return ErrorCode::WRONG_HEADER;

    // Einzelner Bilder auslesen ( untere Körper ): 8 Animation Steps, 6 directions, 2 types (Fat, Non-Fat) = 96
    alloc(96);
    for(uint32_t i = 0; i < 96; ++i)
    {
        uint16_t id;
        uint8_t height;
        if(!(fs >> id >> height))
            return ErrorCode::UNEXPECTED_EOF;

        // stimmt die ID? (ID 0xF401)
        if(id != 0x01F4)
            return (i == 0) ? ErrorCode::WRONG_HEADER : ErrorCode::WRONG_FORMAT;

        std::vector<uint16_t> starts(height);
        uint8_t ny;
        if(!(fs >> starts >> ny))
            return ErrorCode::UNEXPECTED_EOF;

        auto image = getAllocator().create<ArchivItem_Bitmap_Player>(BOBTYPE_BITMAP_PLAYER);
        assert(image);
        image->setNx(16); //-V522
        image->setNy(ny);

        int ec = image->load(32, height, raw_base, starts, true, palette);
        if(ec)
            return ec;

        set(i, std::move(image));
    }

    // erstmal die 6 Farbblöcke fr die 6 Richtungen
    std::array<std::vector<uint8_t>, 6> raw;

    for(auto& i : raw)
    {
        uint16_t id, size;
        if(!(fs >> id >> size))
            return ErrorCode::UNEXPECTED_EOF;

        // stimmt die ID? (ID 0xF401)
        if(id != 0x01F5)
            return ErrorCode::WRONG_FORMAT;

        i.resize(size);
        fs >> i;
    }

    // Anzahl Warenbilder
    fs >> numGoodImgs;

    alloc_inc(numGoodImgs);

    std::vector<bool> loaded(numGoodImgs, false);

    std::vector<uint8_t> heights(numGoodImgs);
    std::vector<std::vector<uint16_t> > starts(numGoodImgs);
    std::vector<uint8_t> ny(numGoodImgs);

    for(uint16_t i = 0; i < numGoodImgs; ++i)
    {
        uint16_t id;
        if(!(fs >> id >> heights[i]))
            return ErrorCode::UNEXPECTED_EOF;

        // stimmt die ID? (ID 0xF401)
        if(id != 0x01F4)
            return ErrorCode::WRONG_FORMAT;

        starts[i].resize(heights[i]);
        fs >> starts[i] >> ny[i];
    }

    // Number of complete pictures.
    // Links form an array: [ware][animStep][fat][direction]: [][8][2][6]
    // the item at position 96 + link[ware][animStep][fat][direction] shall be combined
    // with the appropriate body
    uint16_t item_count;
    if(!(fs >> item_count))
        return ErrorCode::UNEXPECTED_EOF;

    links.resize(item_count);

    for(uint32_t i = 0; i < item_count; ++i)
    {
        uint16_t unknown;
        if(!(fs >> links[i] >> unknown))
            return ErrorCode::UNEXPECTED_EOF;

        if(loaded[links[i]])
            continue;

        auto image = getAllocator().create<ArchivItem_Bitmap_Player>(BOBTYPE_BITMAP_PLAYER);
        assert(image);
        image->setNx(16); //-V522
        image->setNy(ny[links[i]]);

        int ec = image->load(32, heights[links[i]], raw[i % 6], starts[links[i]], true, palette);
        if(ec)
            return ec;

        set(96 + links[i], std::move(image));
        loaded[links[i]] = true;
    }

    return (!fs) ? ErrorCode::UNEXPECTED_EOF : ErrorCode::NONE;
}

/**
 *  schreibt die Bobdaten in eine Datei.
 *
 *  @param[in] file    Dateihandle der Datei
 *  @param[in] palette Grundpalette
 *
 *  @return liefert Null bei Erfolg, ungleich Null bei Fehler
 */
int libsiedler2::ArchivItem_Bob::write(std::ostream&, const ArchivItem_Palette*) const
{
    return ErrorCode::UNSUPPORTED_FORMAT;
}
