// Copyright (c) 2005 - 2020 Settlers Freaks (sf-team at siedler25.org)
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

#include "ArchivItem_Sound_Midi.h"
#include "ErrorCodes.h"
#include "MIDI_Header.h"
#include "fileFormatHelpers.h"
#include "libendian/EndianIStreamAdapter.h"
#include "libendian/EndianOStreamAdapter.h"
#include <iostream>
#include <stdexcept>

namespace libsiedler2 {

/** @class baseArchivItem_Sound_Midi
 *
 *  Klasse für MIDI-Sounds.
 */

ArchivItem_Sound_Midi::ArchivItem_Sound_Midi() : ArchivItem_Sound(SoundType::Midi), numTracks(0), ppqs(0) {}

int ArchivItem_Sound_Midi::load(std::istream& file, uint32_t length)
{
    if(!file)
        return ErrorCode::FILE_NOT_ACCESSIBLE;
    if(length < sizeof(MIDI_Header))
        return ErrorCode::WRONG_HEADER;

    libendian::EndianIStreamAdapter<true, std::istream&> fs(file);
    const long itemEndPos = fs.getPosition() + length;

    MIDI_Header header;
    if(!fs.readRaw(&header, 1))
        return ErrorCode::WRONG_HEADER;
    // ist es eine MIDI-File? (Header "MThd")
    if(!isChunk(header.id, "MThd"))
        return ErrorCode::WRONG_HEADER;

    numTracks = header.numTracks;
    ppqs = header.ppqs;

    if(numTracks == 0 || numTracks > 256)
        return ErrorCode::WRONG_FORMAT;

    uint16_t curTrack = 0;
    while(curTrack < numTracks)
    {
        std::array<char, 4> chunk;
        uint32_t chunkLen;
        if(!(fs >> chunk >> chunkLen))
            return ErrorCode::UNEXPECTED_EOF;
        if(isChunk(chunk, "MTrk"))
        {
            fs.setPositionRel(-8);
            chunkLen += 8;

            int ec = tracklist[curTrack].read(file, chunkLen);
            if(ec)
                return ec;

            ++curTrack;
        } else
            return ErrorCode::WRONG_FORMAT;
    }

    if(fs.getPosition() != itemEndPos)
        return ErrorCode::WRONG_FORMAT;
    return (!file) ? ErrorCode::UNEXPECTED_EOF : ErrorCode::NONE;
}

int ArchivItem_Sound_Midi::write(std::ostream& file) const
{
    if(!file)
        return ErrorCode::FILE_NOT_ACCESSIBLE;

    libendian::EndianOStreamAdapter<true, std::ostream&> fs(file);

    MIDI_Header header;
    setChunkId(header.id, "MThd");
    header.headerSize = 6;
    header.format = numTracks == 1 ? 0 : 1;
    header.numTracks = numTracks;
    header.ppqs = ppqs;
    if(!fs.writeRaw(&header, 1))
        return ErrorCode::UNEXPECTED_EOF;
    for(uint16_t i = 0; i < numTracks; ++i)
        fs.write(tracklist[i].getMid(), tracklist[i].getMidLength());

    return (!file) ? ErrorCode::UNEXPECTED_EOF : ErrorCode::NONE;
}

void ArchivItem_Sound_Midi::addTrack(const MIDI_Track& track)
{
    if(getNumTracks() >= tracklist.size())
        throw std::runtime_error("No more space for tracks");
    tracklist[numTracks++] = track;
}

} // namespace libsiedler2
