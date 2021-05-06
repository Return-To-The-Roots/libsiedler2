// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

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
