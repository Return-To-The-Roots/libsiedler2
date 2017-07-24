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
#include "ArchivItem_Map.h"
#include "ArchivItem_Map_Header.h"
#include "ArchivItem_Raw.h"
#include "libsiedler2.h"
#include "IAllocator.h"
#include "libendian/src/EndianIStreamAdapter.h"
#include "libendian/src/EndianOStreamAdapter.h"
#include <iostream>

struct BlockHeader{ //-V802
    uint16_t id; // Must be 0x2710
    uint32_t unknown; // Always 0
    uint16_t w, h;
    uint16_t multiplier; // Not sure, always 1
    uint32_t blockLength;
};

libsiedler2::ArchivItem_Map::ArchivItem_Map() : ArchivItem(), ArchivInfo()
{
    setBobType(BOBTYPE_MAP);
}

libsiedler2::ArchivItem_Map::~ArchivItem_Map()
{
}

/**
 *  lädt die Mapdaten aus einer Datei.
 *
 *  @param[in] file Dateihandle der Datei
 *  @param[in] only_header Soll nur der Header gelesen werden?
 *
 *  @return liefert Null bei Erfolg, ungleich Null bei Fehler
 */
int libsiedler2::ArchivItem_Map::load(std::istream& file, bool only_header)
{
    if(!file)
        return 1;

    clear();

    ArchivItem_Map_Header* header = dynamic_cast<ArchivItem_Map_Header*>(getAllocator().create(BOBTYPE_MAP_HEADER));

    if(header->load(file) != 0){
        delete header;
        return 2;
    }

    push(header);

    // nur der Header?
    if(only_header)
        return 0;

    const uint16_t w = header->getWidth();
    const uint16_t h = header->getHeight();

    libendian::EndianIStreamAdapter<false, std::istream&> fs(file);
    for(uint32_t i = 1; i < 15; ++i)
    {
        BlockHeader bHeader;
        fs >> bHeader.id >> bHeader.unknown >> bHeader.w >> bHeader.h >> bHeader.multiplier >> bHeader.blockLength;
        // Header id must match
        if(bHeader.id != 0x2710 || bHeader.unknown != 0)
        {
            assert(false);
            return 3;
        }
        // Blocksize must match extents
        if(bHeader.blockLength != static_cast<uint32_t>(w)*static_cast<uint32_t>(h))
        {
            assert(false);
            return 4;
        }
        // Multiplier of 0 means unused block and implies no data
        if(bHeader.multiplier == 0)
        {
            assert(bHeader.blockLength == 0);
            push(NULL);
            continue;
        }
        // If there is data, size must match
        if(bHeader.w != w || bHeader.h != h)
        {
            assert(false);
            return 5;
        }

        if(i == 1 && header->hasExtraWord())
        {
            // Work around for map file bug: There are 2 extra bytes inbetween the header which would actually belong to the first block
            fs.setPositionRel(-2);
        }

        ArchivItem_Raw* layer = dynamic_cast<ArchivItem_Raw*>(getAllocator().create(BOBTYPE_RAW));
        if(layer->load(file, bHeader.blockLength) != 0){
            delete layer;
            return 6;
        }
        push(layer);
    }

    extraInfo.clear();

    while(true)
    {
        ExtraAnimalInfo info;
        fs >> info.id;
        if(info.id == 0xFF)
            break;
        fs >> info.x >> info.y;
        extraInfo.push_back(info);
    }

    return (!file) ? 99 : 0;
}

/**
 *  schreibt die Mapdaten in eine Datei.
 *
 *  @param[in] file Dateihandle der Datei
 *
 *  @return liefert Null bei Erfolg, ungleich Null bei Fehler
 */
int libsiedler2::ArchivItem_Map::write(std::ostream& file) const
{
    if(!file)
        return 1;

    const ArchivItem_Map_Header* header = dynamic_cast<const ArchivItem_Map_Header*>(get(0));

    if(!header)
        return 2;
    if(header->write(file) != 0)
        return 3;

    BlockHeader bHeader;
    bHeader.id = 0x2710;
    bHeader.unknown = 0;
    bHeader.w = header->getWidth();
    bHeader.h = header->getHeight();
    // For unused
    bHeader.multiplier = 0;
    bHeader.blockLength = 0;

    libendian::EndianOStreamAdapter<false, std::ostream&> fs(file);
    for(uint32_t i = 1; i < std::max<uint32_t>(15u, size()); ++i)
    {
        const ArchivItem_Raw* layer = dynamic_cast<const ArchivItem_Raw*>(get(i));
        fs << bHeader.id << bHeader.unknown << bHeader.w << bHeader.h;
        if(layer)
        {
            fs << uint16_t(1);
            assert(layer->getData().size() == size_t(header->getWidth()) * size_t(header->getHeight()));
            // Size is written in layer
            if(layer->write(file, true) != 0)
                return 4 + i;
        } else
            fs << bHeader.multiplier << bHeader.blockLength;
    }

    for(std::vector<ExtraAnimalInfo>::const_iterator it = extraInfo.begin(); it != extraInfo.end(); ++it)
    {
        fs << it->id << it->x << it->y;
    }
    fs << uint8_t(0xFF);

    return (!file) ? 99 : 0;
}
