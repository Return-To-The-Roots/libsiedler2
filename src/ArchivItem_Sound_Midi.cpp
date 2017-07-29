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
#include "ArchivItem_Sound_Midi.h"
#include "MIDI_Header.h"
#include "libendian/src/EndianIStreamAdapter.h"
#include "libendian/src/EndianOStreamAdapter.h"
#include <iostream>
#include <cstring>
#include <stdexcept>

namespace libsiedler2{

inline bool isChunk(const char lhs[4], const char rhs[5])
{
    return strncmp(lhs, rhs, 4) == 0;
}

/** @class baseArchivItem_Sound_Midi
 *
 *  Basisklasse für MIDI-Sounds.
 */

baseArchivItem_Sound_Midi::baseArchivItem_Sound_Midi() : baseArchivItem_Sound()
{
    setType(SOUNDTYPE_MIDI);

    numTracks = 0;
}

baseArchivItem_Sound_Midi::baseArchivItem_Sound_Midi(const baseArchivItem_Sound_Midi& item) : baseArchivItem_Sound( item )
{
    numTracks = item.numTracks;
    tracklist = item.tracklist;
}

baseArchivItem_Sound_Midi::~baseArchivItem_Sound_Midi()
{
}

baseArchivItem_Sound_Midi& baseArchivItem_Sound_Midi::operator=(const baseArchivItem_Sound_Midi& item)
{
    if(this == &item)
        return *this;

    baseArchivItem_Sound::operator=(item);
    numTracks = item.numTracks;
    tracklist = item.tracklist;

    return *this;
}

int baseArchivItem_Sound_Midi::load(std::istream& file, uint32_t length)
{
    if(!file || length == 0)
        return 1;

    libendian::EndianIStreamAdapter<true, std::istream&> fs(file);
    const long itemEndPos = fs.getPosition() + length;

    MIDI_Header header;
    if(!fs.readRaw(&header, 1))
        return 2;
    // ist es eine MIDI-File? (Header "MThd")
    if(!isChunk(header.id, "MThd"))
        return 3;

    numTracks = header.numTracks;
    ppqs = header.ppqs;

    if(numTracks == 0 || numTracks > 256)
        return 8;

    uint16_t curTrack = 0;
    while(curTrack < numTracks)
    {
        char chunk[4];
        uint32_t chunkLen;
        if(!(fs >> chunk >> chunkLen))
            return 9;
        if(isChunk(chunk, "MTrk"))
        {
            fs.setPositionRel(-8);
            chunkLen += 8;

            if(!tracklist[curTrack].read(file, chunkLen))
                return 11;

            ++curTrack;
        } else
            return 12;
    }

    assert(fs.getPosition() == itemEndPos);
    // auf jeden Fall kompletten Datensatz überspringen
    fs.setPosition(itemEndPos);
    return (!file) ? 99 : 0;
}

int baseArchivItem_Sound_Midi::write(std::ostream& file) const
{
    if(!file)
        return 1;

    libendian::EndianOStreamAdapter<true, std::ostream&> fs(file);

    MIDI_Header header;
    strncpy(header.id, "MThd", 4);
    header.headerSize = 6;
    header.format = numTracks == 1 ? 0 : 1;
    header.numTracks = numTracks;
    header.ppqs = ppqs;
    if(!fs.writeRaw(&header, 1))
        return 2;
    for(uint16_t i = 0; i < numTracks; ++i)
        fs.write(tracklist[i].getMid(), tracklist[i].getMidLength());

    return (!file) ? 99 : 0;
}

void baseArchivItem_Sound_Midi::addTrack(const MIDI_Track& track)
{
    if(getTrackCount() >= tracklist.size())
        throw std::runtime_error("No more space for tracks");
    tracklist[numTracks++] = track;
}

} // namespace libsiedler2
