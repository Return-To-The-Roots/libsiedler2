// $Id: MIDI_Track.cpp 9359 2014-04-25 15:37:22Z FloSoft $
//
// Copyright (c) 2005 - 2011 Settlers Freaks (sf-team at siedler25.org)
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

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#if defined _WIN32 && defined _DEBUG && defined _MSC_VER
#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

MIDI_Track::MIDI_Track()
{
    xmid_data = NULL;
    xmid_length = 0;
    mid_data = NULL;
    mid_length = 0;
}

MIDI_Track::MIDI_Track(const MIDI_Track* item)
{
    copy(item);
}

MIDI_Track::~MIDI_Track()
{
    clearXMid();
    clearMid();
}

void MIDI_Track::copy(const MIDI_Track* item)
{
    allocXMid(item->xmid_length);
    if(xmid_length > 0)
        memcpy(xmid_data, item->xmid_data, xmid_length);

    allocMid(item->mid_length > 14 ? item->mid_length - 14 : item->mid_length);
    if(mid_length > 0)
        memcpy(mid_data, item->mid_data, mid_length);
}

void MIDI_Track::allocXMid(unsigned int length)
{
    clearXMid();

    xmid_length = length;
    if(xmid_length > 0)
    {
        xmid_data = new unsigned char[xmid_length + 1];
        memset(xmid_data, 0, xmid_length + 1);
    }
}

int MIDI_Track::readXMid(FILE* file)
{
    if(libendian::le_read_uc(xmid_data, xmid_length, file) != (int)xmid_length)
        return 1;
    return 0;
}

void MIDI_Track::clearXMid()
{
    delete[] xmid_data;
    xmid_data = NULL;
    xmid_length = 0;
}

void MIDI_Track::allocMid(unsigned int length)
{
    clearMid();

    mid_length = length;
    if(mid_length > 0)
    {
        mid_length += 14;
        mid_data = new unsigned char[mid_length + 1];
        memset(mid_data, 0, mid_length + 1);

        // MIDI Header setzen
        memcpy(mid_data, "MThd", 4);
        mid_data[7] = 0x06; // block length (bytes 3-7)
        ((unsigned short*)(mid_data))[4] = 0x0000; // type (MIDI 0)
        ((unsigned short*)(mid_data))[5] = 0x0100; // trackanzahl (1)
        ((unsigned short*)(mid_data))[6] = 0x3C00; // PPQN (?)
    }
}

int MIDI_Track::readMid(FILE* file)
{
    if(libendian::le_read_uc(&mid_data[14], mid_length - 14, file) != (int)mid_length - 14)
        return 1;
    return 0;
}

void MIDI_Track::clearMid()
{
    delete[] mid_data;
    mid_data = NULL;
    mid_length = 0;
}

int MIDI_Track::XMid2Mid()
{
    XMIDI_Track track(this);
    if(track.Convert() != 0)
        return 1;

    return 0;
}

unsigned char* MIDI_Track::getMid(bool withheader) const
{
    if(!mid_data)
        return NULL;

    if(withheader)
        return &mid_data[0];

    return &mid_data[14];
}

unsigned int MIDI_Track::getMidLength(bool withheader) const
{
    if(withheader)
        return mid_length;
    return (mid_length - 14);
}
