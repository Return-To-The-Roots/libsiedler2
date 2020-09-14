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

#include <cstdint>
#include <iosfwd>
#include <vector>

namespace libsiedler2 {

class MIDI_Track
{
public:
    MIDI_Track();
    MIDI_Track(std::vector<uint8_t> data);
    ~MIDI_Track();

    int read(std::istream& file, size_t length);
    void clear();

    const uint8_t* getMid() const;
    uint32_t getMidLength() const;
    const std::vector<uint8_t>& getData() const { return mid_data; }

protected:
    std::vector<uint8_t> mid_data;
};
} // namespace libsiedler2
