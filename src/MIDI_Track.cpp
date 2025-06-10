// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "MIDI_Track.h"
#include "ErrorCodes.h"
#include <cassert>
#include <iostream>
#include <limits>
#include <utility>

namespace libsiedler2 {
MIDI_Track::MIDI_Track() = default;

MIDI_Track::MIDI_Track(std::vector<uint8_t> data) : mid_data(std::move(data)) {}

MIDI_Track::~MIDI_Track() = default;

int MIDI_Track::read(std::istream& file, size_t length)
{
    clear();
    if(length == 0)
        return ErrorCode::NONE;

    mid_data.resize(length);
    if(!file.read(reinterpret_cast<char*>(mid_data.data()), length))
        return ErrorCode::UNEXPECTED_EOF;
    return ErrorCode::NONE;
}

void MIDI_Track::clear()
{
    mid_data.clear();
}

const uint8_t* MIDI_Track::getMid() const
{
    if(mid_data.empty())
        return nullptr;
    return mid_data.data();
}

uint32_t MIDI_Track::getMidLength() const
{
    assert(mid_data.size() < std::numeric_limits<uint32_t>::max());
    return static_cast<uint32_t>(mid_data.size());
}
} // namespace libsiedler2
