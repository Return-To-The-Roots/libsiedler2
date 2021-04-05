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

#include "ArchivItem_Map.h"
#include "ArchivItem_Map_Header.h"
#include "ArchivItem_Raw.h"
#include "ErrorCodes.h"
#include "IAllocator.h"
#include "libsiedler2.h"
#include "libendian/EndianIStreamAdapter.h"
#include "libendian/EndianOStreamAdapter.h"
#include <iostream>
#include <stdexcept>

namespace {

struct BlockHeader
{                     //-V802
    uint16_t id;      // Must be 0x2710
    uint32_t unknown; // Always 0
    uint16_t w, h;
    uint16_t multiplier; // Not sure, always 1
    uint32_t blockLength;
};
} // namespace

namespace libsiedler2 {

ArchivItem_Map::ArchivItem_Map() : ArchivItem(BobType::Map), header_(nullptr) {}

ArchivItem_Map::ArchivItem_Map(const ArchivItem_Map& other) : ArchivItem(other), Archiv(other)
{
    header_ = static_cast<ArchivItem_Map_Header*>(get(0));
}

ArchivItem_Map::~ArchivItem_Map() = default;

/**
 *  lädt die Mapdaten aus einer Datei.
 *
 *  @param[in] file Dateihandle der Datei
 *  @param[in] only_header Soll nur der Header gelesen werden?
 *
 *  @return liefert Null bei Erfolg, ungleich Null bei Fehler
 */
int ArchivItem_Map::load(std::istream& file, bool onlyHeader)
{
    if(!file)
        return ErrorCode::FILE_NOT_ACCESSIBLE;

    clear();
    header_ = nullptr;

    {
        auto header = getAllocator().create<ArchivItem_Map_Header>(BobType::MapHeader);
        assert(header);

        int ec = header->load(file); //-V522
        if(ec)
            return ec;

        push(std::move(header));
        header_ = static_cast<ArchivItem_Map_Header*>(get(0));
    }

    // nur der Header?
    if(onlyHeader)
        return ErrorCode::NONE;

    const ArchivItem_Map_Header& header = getHeader();
    const uint16_t w = header.getWidth();
    const uint16_t h = header.getHeight();

    libendian::EndianIStreamAdapter<false, std::istream&> fs(file);
    for(uint32_t i = 1; i < 15; ++i)
    {
        BlockHeader bHeader;
        fs >> bHeader.id >> bHeader.unknown >> bHeader.w >> bHeader.h >> bHeader.multiplier >> bHeader.blockLength;
        // Header id must match
        if(!fs || bHeader.id != 0x2710 || bHeader.unknown != 0)
        {
            return ErrorCode::WRONG_FORMAT;
        }
        // Multiplier of 0 means unused block and implies no data
        if(bHeader.multiplier == 0)
        {
            assert(bHeader.blockLength == 0);
            push(nullptr);
            continue;
        }
        // Blocksize must match extents
        if(bHeader.blockLength != static_cast<uint32_t>(w) * static_cast<uint32_t>(h))
        {
            assert(false);
            return ErrorCode::WRONG_FORMAT;
        }
        // If there is data, size must match
        if(bHeader.w != w || bHeader.h != h)
        {
            assert(false);
            return ErrorCode::WRONG_FORMAT;
        }

        auto layer = getAllocator().create<ArchivItem_Raw>(BobType::Raw);
        if(auto ec = layer->load(file, bHeader.blockLength)) //-V522
            return ec;
        if(i == 1 && header.hasExtraWord())
        {
            // Work around for map file bug: There are 2 extra bytes inbetween the header which would actually belong to
            // the first block
            fs.setPositionRel(-2);
            std::vector<uint8_t>& data = layer->getData();
            // Replace last 2 bytes by 3rd last one
            data[data.size() - 1] = data[data.size() - 2] = data[data.size() - 3];
        }

        push(std::move(layer));
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

    return (!file) ? ErrorCode::UNEXPECTED_EOF : ErrorCode::NONE;
}

/**
 *  schreibt die Mapdaten in eine Datei.
 *
 *  @param[in] file Dateihandle der Datei
 *
 *  @return liefert Null bei Erfolg, ungleich Null bei Fehler
 */
int ArchivItem_Map::write(std::ostream& file) const
{
    if(!file)
        return ErrorCode::FILE_NOT_ACCESSIBLE;

    const auto* header = dynamic_cast<const ArchivItem_Map_Header*>(get(0));

    if(!header)
        return ErrorCode::WRONG_ARCHIV;
    int ec = header->write(file);
    if(ec)
        return ec;

    BlockHeader bHeader;
    bHeader.id = 0x2710;
    bHeader.unknown = 0;
    bHeader.w = header->getWidth();
    bHeader.h = header->getHeight();
    // For unused
    bHeader.multiplier = 0;
    bHeader.blockLength = 0;

    libendian::EndianOStreamAdapter<false, std::ostream&> fs(file);
    for(uint32_t i = 1; i < std::max(15u, uint32_t(size())); ++i)
    {
        const auto* layer = dynamic_cast<const ArchivItem_Raw*>(get(i));
        fs << bHeader.id << bHeader.unknown << bHeader.w << bHeader.h;
        if(layer)
        {
            fs << uint16_t(1);
            assert(layer->getData().size() == size_t(header->getWidth()) * size_t(header->getHeight()));
            // Size is written in layer
            ec = layer->write(file, true);
            if(ec)
                return ec;
        } else
            fs << bHeader.multiplier << bHeader.blockLength;
    }

    for(const ExtraAnimalInfo& it : extraInfo)
    {
        fs << it.id << it.x << it.y;
    }
    fs << uint8_t(0xFF);

    return (!file) ? ErrorCode::UNEXPECTED_EOF : ErrorCode::NONE;
}

void ArchivItem_Map::init(std::unique_ptr<ArchivItem_Map_Header> header)
{
    clear();
    header_ = nullptr;
    if(!header)
        return;
    alloc(NUM_SWD_LAYERS + 1u); // Include header
    set(0, std::move(header));
    header_ = static_cast<ArchivItem_Map_Header*>(get(0));
    const size_t numNodes = header_->getWidth() * header_->getHeight();
    for(unsigned i = 0; i < NUM_SWD_LAYERS; ++i)
        set(i + 1, std::make_unique<libsiedler2::ArchivItem_Raw>(std::vector<uint8_t>(numNodes)));
}

const libsiedler2::ArchivItem_Map_Header& ArchivItem_Map::getHeader() const
{
    assert(header_);
    return *header_;
}

const std::vector<uint8_t>& ArchivItem_Map::getLayer(MapLayer type) const
{
    const auto* item = static_cast<const ArchivItem_Raw*>(get(static_cast<size_t>(type) + 1)); // 0 = header
    if(!item)
        throw std::range_error("Layer not found");
    return item->getData(); //-V522
}

std::vector<uint8_t>& ArchivItem_Map::getLayer(MapLayer type)
{
    auto* item = static_cast<ArchivItem_Raw*>(get(static_cast<size_t>(type) + 1)); // 0 = header
    if(!item)
        throw std::range_error("Layer not found");
    return item->getData(); //-V522
}

bool ArchivItem_Map::hasLayer(MapLayer type) const
{
    return get(static_cast<size_t>(type) + 1) != nullptr;
}

uint8_t ArchivItem_Map::getMapDataAt(MapLayer type, unsigned idx) const
{
    return getLayer(type)[idx];
}

uint8_t ArchivItem_Map::getMapDataAt(MapLayer type, unsigned short x, unsigned short y) const
{
    return getMapDataAt(type, y * header_->getWidth() + x);
}

void ArchivItem_Map::setMapDataAt(MapLayer type, unsigned idx, uint8_t value)
{
    getLayer(type)[idx] = value;
}

void ArchivItem_Map::setMapDataAt(MapLayer type, unsigned short x, unsigned short y, uint8_t value)
{
    setMapDataAt(type, y * header_->getWidth() + x, value);
}

} // namespace libsiedler2