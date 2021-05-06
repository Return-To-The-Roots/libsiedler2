// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "XMIDI_Track.h"
#include "ErrorCodes.h"
#include <iostream>

namespace libsiedler2 {
XMIDI_Track::XMIDI_Track() = default;

XMIDI_Track::~XMIDI_Track() = default;

int XMIDI_Track::read(std::istream& file, size_t length)
{
    data_.resize(length);
    if(!file.read(reinterpret_cast<char*>(&data_.front()), length))
        return ErrorCode::UNEXPECTED_EOF;
    return ErrorCode::NONE;
}

void XMIDI_Track::clear()
{
    data_.clear();
}
} // namespace libsiedler2
