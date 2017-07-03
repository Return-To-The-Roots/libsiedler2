// Copyright (c) 2005 - 2015 Settlers Freaks (sf-team at siedler25.org)
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

#include "libSiedler2Defines.h" // IWYU pragma: keep
#include "MIDI_Track.h"
#include "XMIDI_Track.h"
#include <fstream>
#include <cstring>
#include <limits>

MIDI_Track::MIDI_Track()
{}

MIDI_Track::~MIDI_Track()
{}

int MIDI_Track::readXMid(std::istream& file, size_t length)
{
    xmid_data.resize(length);
    if(!file.read(reinterpret_cast<char*>(&xmid_data.front()), length))
        return 1;
    return 0;
}

void MIDI_Track::clearXMid()
{
    xmid_data.clear();
}

int MIDI_Track::readMid(std::istream& file, size_t length)
{
    clearMid();
    if(length == 0)
        return 0;

    mid_data.resize(length + 14);
    // MIDI Header setzen
    mid_data[0] = 'M';
    mid_data[1] = 'T';
    mid_data[2] = 'h';
    mid_data[3] = 'd';
    mid_data[7] = 0x06; // block length (bytes 3-7)
    *reinterpret_cast<unsigned short*>(&mid_data[8]) = 0x0000; // type (MIDI 0)
    *reinterpret_cast<unsigned short*>(&mid_data[10]) = 0x0100; // trackanzahl (1)
    *reinterpret_cast<unsigned short*>(&mid_data[12]) = 0x3C00; // PPQN (?)
    if(!file.read(reinterpret_cast<char*>(&mid_data[14]), length - 14))
        return 1;
    return 0;
}

void MIDI_Track::clearMid()
{
    mid_data.clear();
}

int MIDI_Track::XMid2Mid()
{
    XMIDI_Track track(this);
    if(track.Convert() != 0)
        return 1;

    return 0;
}

const unsigned char* MIDI_Track::getMid(bool withheader) const
{
    if(mid_data.empty())
        return NULL;

    if(withheader)
        return &mid_data[0];

    return &mid_data[14];
}

unsigned int MIDI_Track::getMidLength(bool withheader) const
{
    assert(mid_data.size() < std::numeric_limits<unsigned>::max());
    unsigned size = static_cast<unsigned>(mid_data.size());
    if(!withheader)
        size -= 14;
    return size;
}
