// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

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
