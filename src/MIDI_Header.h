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

#ifndef MIDI_Header_h__
#define MIDI_Header_h__

#pragma once

#include <boost/endian/arithmetic.hpp>
#include <boost/static_assert.hpp>

namespace libsiedler2 {
struct MIDI_Header
{
    char id[4]; /// Must be MThd
    boost::endian::big_uint32_t headerSize;
    boost::endian::big_uint16_t format;
    boost::endian::big_uint16_t numTracks;
    boost::endian::big_int16_t ppqs;
};

BOOST_STATIC_ASSERT_MSG(sizeof(MIDI_Header) == 14u, "Invalid padding detected");
} // namespace libsiedler2

#endif // MIDI_Header_h__
