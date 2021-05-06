// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <cstdint>
#include <iosfwd>
#include <vector>

namespace libsiedler2 {

class XMIDI_Track
{
public:
    struct Timbre
    {
        uint8_t patch, bank;
    };
    XMIDI_Track();
    ~XMIDI_Track();

    int read(std::istream& file, size_t length);
    void clear();

    const std::vector<uint8_t>& getData() const { return data_; }
    std::vector<Timbre>& getTimbres() { return timbres_; }
    const std::vector<Timbre>& getTimbres() const { return timbres_; }

protected:
    std::vector<uint8_t> data_;
    std::vector<Timbre> timbres_;
};
} // namespace libsiedler2
