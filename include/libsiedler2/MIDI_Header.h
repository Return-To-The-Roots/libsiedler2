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

#include <boost/endian/arithmetic.hpp>
#include <array>

namespace libsiedler2 {
struct MIDI_Header
{
    std::array<char, 4> id; /// Must be MThd
    boost::endian::big_uint32_t headerSize;
    boost::endian::big_uint16_t format;
    boost::endian::big_uint16_t numTracks;
    boost::endian::big_int16_t ppqs;
};

static_assert(sizeof(MIDI_Header) == 14u, "Invalid padding detected");
} // namespace libsiedler2
