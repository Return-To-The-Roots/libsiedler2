// $Id: MIDI_Track.cpp 9359 2014-04-25 15:37:22Z FloSoft $
//
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

///////////////////////////////////////////////////////////////////////////////
// Header
#include "main.h"
#include "MIDI_Track.h"
#include "XMIDI_Track.h"
#include <libendian.h>
#include <cstring>

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#if defined _WIN32 && defined _DEBUG && defined _MSC_VER
#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

MIDI_Track::MIDI_Track()
{}

MIDI_Track::~MIDI_Track()
{}

int MIDI_Track::readXMid(FILE* file, size_t length)
{
    xmid_data.resize(length);
    if(libendian::le_read_uc(&xmid_data.front(), length, file) != (int)length)
        return 1;
    return 0;
}

void MIDI_Track::clearXMid()
{
    xmid_data.clear();
}

int MIDI_Track::readMid(FILE* file, size_t length)
{
    clearMid();
    if(length == 0)
        return 0;

    mid_data.resize(length + 14);
    // MIDI Header setzen
    memcpy(&mid_data.front(), "MThd", 4);
    mid_data[7] = 0x06; // block length (bytes 3-7)
    *reinterpret_cast<unsigned short*>(&mid_data[8]) = 0x0000; // type (MIDI 0)
    *reinterpret_cast<unsigned short*>(&mid_data[10]) = 0x0100; // trackanzahl (1)
    *reinterpret_cast<unsigned short*>(&mid_data[12]) = 0x3C00; // PPQN (?)
    if(libendian::le_read_uc(&mid_data[14], length - 14, file) != (int)length - 14)
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
    if(withheader)
        return mid_data.size();
    return (mid_data.size() - 14);
}
