// Copyright (c) 2005 - 2020 Settlers Freaks (sf-team at siedler25.org)
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

#pragma once

#include "Archiv.h"
#include "ArchivItem.h"
#include <cstdint>
#include <iosfwd>
#include <memory>

namespace libsiedler2 {
class ArchivItem_Map_Header;

/// Names for the different layers of the map. Entry 0 is the header, so Layer 0 = Entry 1
enum class MapLayer
{
    Altitude = 0,
    Terrain1 = 1,
    Terrain2 = 2,
    RoadsOld = 3,
    ObjectIndex = 4,
    ObjectType = 5,
    Animals = 6,
    Unknown7 = 7,
    BuildingQuality = 8,
    Unknown9 = 9,
    Unknown10 = 10,
    Resources = 11,
    Shadows = 12,
    Lakes = 13,
    Reservations = 14,
    Owner = 15
};

/// S2 map file
class ArchivItem_Map : public ArchivItem, private Archiv
{
public:
    struct ExtraAnimalInfo
    {
        uint8_t id;
        uint16_t x, y;
    };

    ArchivItem_Map();
    ~ArchivItem_Map() override;
    RTTR_CLONEABLE(ArchivItem_Map)

    /// Load the map data. If onlyHeader is true, then only the map header will be loaded
    int load(std::istream& file, bool onlyHeader);
    /// Write the map to a file
    int write(std::ostream& file) const;

    /// Init the map layers from the given header
    void init(std::unique_ptr<ArchivItem_Map_Header> header);

    /// Return the map header
    const ArchivItem_Map_Header& getHeader() const;

    /// Get a layer of the map
    const std::vector<uint8_t>& getLayer(MapLayer type) const;
    /// Get a layer of the map
    std::vector<uint8_t>& getLayer(MapLayer type);
    bool hasLayer(MapLayer type) const;

    /// Get the map data at the given index
    uint8_t getMapDataAt(MapLayer type, unsigned idx) const;
    /// Get the map data at the given idxitiob
    uint8_t getMapDataAt(MapLayer type, unsigned short x, unsigned short y) const;

    /// Set the map data at the index
    void setMapDataAt(MapLayer type, unsigned idx, uint8_t value);
    /// Set the map data at the idxition
    void setMapDataAt(MapLayer type, unsigned short x, unsigned short y, uint8_t value);

    std::vector<ExtraAnimalInfo> extraInfo;

protected:
    ArchivItem_Map(const ArchivItem_Map& other);
    ArchivItem_Map(ArchivItem_Map&&) = delete;
    ArchivItem_Map& operator=(const ArchivItem_Map& other) = delete;
    ArchivItem_Map& operator=(ArchivItem_Map&&) = delete;

private:
    ArchivItem_Map_Header* header_;
};
} // namespace libsiedler2
