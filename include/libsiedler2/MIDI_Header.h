// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

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
