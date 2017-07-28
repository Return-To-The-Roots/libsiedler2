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
#include "ArchivItem_Sound_XMidi.h"
#include "libendian/src/EndianIStreamAdapter.h"
#include "libendian/src/EndianOStreamAdapter.h"
#include <iostream>
#include <cstring>
#include <stdexcept>

namespace libsiedler2{

bool isChunk(const char lhs[4], const char rhs[5])
{
    return strncmp(lhs, rhs, 4) == 0;
}

/** @class baseArchivItem_Sound_XMidi
 *
 *  Basisklasse für XMIDI-Sounds.
 */

baseArchivItem_Sound_XMidi::baseArchivItem_Sound_XMidi() : baseArchivItem_Sound()
{
    setType(SOUNDTYPE_XMIDI);

    numTracks = 0;
}

baseArchivItem_Sound_XMidi::baseArchivItem_Sound_XMidi(const baseArchivItem_Sound_XMidi& item) : baseArchivItem_Sound( item )
{
    numTracks = item.numTracks;
    tracklist = item.tracklist;
}

baseArchivItem_Sound_XMidi::~baseArchivItem_Sound_XMidi()
{
}

baseArchivItem_Sound_XMidi& baseArchivItem_Sound_XMidi::operator=(const baseArchivItem_Sound_XMidi& item)
{
    if(this == &item)
        return *this;
    baseArchivItem_Sound::operator=(item);
    numTracks = item.numTracks;
    tracklist = item.tracklist;
    return *this;
}

int baseArchivItem_Sound_XMidi::load(std::istream& file, uint32_t length)
{
    if(!file || length == 0)
        return 1;

    libendian::EndianIStreamAdapter<true, std::istream &> fs(file);
    // Position after the current item
    long endPos = fs.getPosition() + length;

    char header[4];
    // Header einlesen
    fs >> header;

    // ist es eine XMIDI-File? (Header "FORM")
    if(!isChunk(header, "FORM"))
        return 3;

    // Länge einlesen
    uint32_t formHeaderLen;
    fs >> formHeaderLen;
    if(formHeaderLen & 1)
        ++formHeaderLen;
    const long headerEndPos = fs.getPosition() + formHeaderLen;

    char chunkId[4];
    fs >> chunkId;

    // ist es eine singleTrack-XMIDI-File? (Typ "XMID")
    if(isChunk(chunkId, "XMID"))
        numTracks = 1;
    else if(isChunk(chunkId, "XDIR"))
    {
        fs >> chunkId;
        if(!isChunk(chunkId, "INFO"))
            return 4;
        // Länge einlesen
        uint32_t chunkLen;
        fs >> chunkLen;
        // Bei ungerader Zahl aufrunden
        if(chunkLen & 1)
            ++chunkLen;
        if(chunkLen != sizeof(numTracks))
            return 8;

        // Little endian track count
        libendian::EndianIStreamAdapter<false, std::istream&> fsLE(file);
        fsLE >> numTracks;

        assert(fs.getPosition() == headerEndPos);

        fs >> chunkId;
        if(!isChunk(chunkId, "CAT "))
            return 5;
        fs.ignore(4); // Ignore cat size which is the following XMID (4)
        fs >> chunkId;
        if(!isChunk(chunkId, "XMID"))
            return 12;
    } else
        return 13;

    if(numTracks == 0 || numTracks > 256)
        return 14;

    uint16_t track_nr = 0;
    while(track_nr < numTracks)
    {
        if(!fs)
            return 99;
        // Chunk-Typ einlesen
        fs >> header;

        if(isChunk(header, "FORM"))
        {
            uint32_t chunkLen;
            fs >> formHeaderLen;
            if(formHeaderLen & 1)
                ++formHeaderLen;

            fs >> chunkId;
            if(!isChunk(chunkId, "XMID"))
                return 22;
            fs >> chunkId;
            // Optional TIMB
            if(isChunk(chunkId, "TIMB"))
            {
                fs >> chunkLen;
                // Alignment: Round uneven up
                if(chunkLen & 1)
                    ++chunkLen;

                fs.ignore(chunkLen);
                fs >> chunkId;
            }
            if(!isChunk(chunkId, "EVNT"))
                return 23;
            fs >> chunkLen;
            // Alignment: Round uneven up
            if(chunkLen & 1)
                ++chunkLen;
            if(tracklist[track_nr].readXMid(file, chunkLen) != 0)
                return 18;

            if(tracklist[track_nr].XMid2Mid() != 0)
                return 19;
            ++track_nr;
        } else
            return 33;
    }

    // auf jeden Fall kompletten Datensatz überspringen
    fs.setPosition(endPos);
    return (!file) ? 99 : 0;
}

int baseArchivItem_Sound_XMidi::write(std::ostream& file) const
{
    if(!file)
        return 1;

    uint32_t length = 0;
    for(uint16_t i = 0; i < numTracks; ++i)
        length += tracklist[i].getMidLength(false);
    libendian::EndianOStreamAdapter<true, std::ostream&> fs(file);
    libendian::EndianOStreamAdapter<false, std::ostream&> fsLE(file);

    // LST-Länge schreiben (Little Endian!)
    fsLE << (length + 14);

    // Header schreiben
    fs.write("MThd", 4);

    // Länge schreiben
    fs << length;

    // Typ schreiben
    fs << uint16_t(0);

    // Tracksanzahl schreiben
    fs << numTracks;

    // PPQS schreiben
    fs << uint16_t(96);

    for(uint16_t i = 0; i < numTracks; ++i)
    {
        fs.write(tracklist[i].getMid(false), tracklist[i].getMidLength(false));
    }

    return (!file) ? 99 : 0;
}

void baseArchivItem_Sound_XMidi::addTrack(const MIDI_Track& track)
{
    if(getTrackCount() >= tracklist.size())
        throw std::runtime_error("No more space for tracks");
    tracklist[numTracks++] = track;
}

} // namespace libsiedler2
