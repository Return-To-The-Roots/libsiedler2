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

#include "ArchivItem_Sound_XMidi.h"
#include "ErrorCodes.h"
#include "XMIDI_TrackConverter.h"
#include "fileFormatHelpers.h"
#include "libendian/EndianIStreamAdapter.h"
#include "libendian/EndianOStreamAdapter.h"
#include <iostream>
#include <stdexcept>

namespace libsiedler2 {

/** @class baseArchivItem_Sound_XMidi
 *
 *  Klasse für XMIDI-Sounds.
 */

ArchivItem_Sound_XMidi::ArchivItem_Sound_XMidi() : ArchivItem_Sound(SoundType::XMidi), numTracks(0), ppqs(60) {}

int ArchivItem_Sound_XMidi::load(std::istream& file, uint32_t length)
{
    if(!file)
        return ErrorCode::FILE_NOT_ACCESSIBLE;
    if(length == 0)
        return ErrorCode::WRONG_HEADER;

    libendian::EndianIStreamAdapter<true, std::istream&> fs(file);
    libendian::EndianIStreamAdapter<false, std::istream&> fsLE(file);
    // Position after the current item
    long endPos = fs.getPosition() + length;

    std::array<char, 4> header;

    // ist es eine XMIDI-File? (Header "FORM")
    if(!(fs >> header) || !isChunk(header, "FORM"))
        return ErrorCode::WRONG_HEADER;

    // Länge einlesen
    uint32_t formHeaderLen;
    fs >> formHeaderLen;
    if(formHeaderLen & 1)
        ++formHeaderLen;
    const long headerEndPos = fs.getPosition() + formHeaderLen;

    std::array<char, 4> chunkId;
    fs >> chunkId;

    // ist es eine singleTrack-XMIDI-File? (Typ "XMID")
    if(isChunk(chunkId, "XMID"))
        numTracks = 1;
    else if(isChunk(chunkId, "XDIR"))
    {
        fs >> chunkId;
        if(!isChunk(chunkId, "INFO"))
            return ErrorCode::WRONG_FORMAT;
        // Länge einlesen
        uint32_t chunkLen;
        fs >> chunkLen;
        // Bei ungerader Zahl aufrunden
        if(chunkLen & 1)
            ++chunkLen;
        if(chunkLen != sizeof(numTracks))
            return ErrorCode::WRONG_FORMAT;

        // Little endian track count
        fsLE >> numTracks;

        if(fs.getPosition() != headerEndPos)
            return ErrorCode::WRONG_FORMAT;

        fs >> chunkId;
        if(!isChunk(chunkId, "CAT "))
            return ErrorCode::WRONG_FORMAT;
        fs.ignore(4); // Ignore cat size which is the following XMID (4)
        fs >> chunkId;
        if(!isChunk(chunkId, "XMID"))
            return ErrorCode::WRONG_FORMAT;
    } else
        return ErrorCode::WRONG_FORMAT;

    if(numTracks == 0 || numTracks > 256)
        return ErrorCode::WRONG_FORMAT;

    uint16_t track_nr = 0;
    while(track_nr < numTracks)
    {
        if(!fs)
            return ErrorCode::UNEXPECTED_EOF;
        // Chunk-Typ einlesen
        fs >> header;

        if(!isChunk(header, "FORM"))
            return ErrorCode::WRONG_FORMAT;

        uint32_t chunkLen;
        fs >> formHeaderLen;
        if(formHeaderLen & 1)
            ++formHeaderLen;

        fs >> chunkId;
        if(!isChunk(chunkId, "XMID"))
            return ErrorCode::WRONG_FORMAT;
        fs >> chunkId;
        // Optional TIMB
        if(isChunk(chunkId, "TIMB"))
        {
            fs >> chunkLen;
            // Alignment: Round uneven up
            if(chunkLen & 1)
                ++chunkLen;
            uint16_t numTimbres;
            fsLE >> numTimbres;
            if(numTimbres * 2 + sizeof(numTimbres) != chunkLen)
                return ErrorCode::WRONG_FORMAT;
            tracklist[track_nr].getTimbres().resize(numTimbres);
            if(numTimbres > 0)
                fs.readRaw(&tracklist[track_nr].getTimbres()[0], numTimbres);
            fs >> chunkId;
        }
        if(!isChunk(chunkId, "EVNT"))
            return ErrorCode::WRONG_FORMAT;
        fs >> chunkLen;
        // Alignment: Round uneven up
        if(chunkLen & 1)
            ++chunkLen;
        if(int ec = tracklist[track_nr].read(file, chunkLen))
            return ec;

        ++track_nr;
    }

    if(fs.getPosition() != endPos)
        return ErrorCode::WRONG_FORMAT;
    return (!file) ? ErrorCode::UNEXPECTED_EOF : ErrorCode::NONE;
}

int ArchivItem_Sound_XMidi::write(std::ostream& file) const
{
    if(!file)
        return ErrorCode::FILE_NOT_ACCESSIBLE;

    libendian::EndianOStreamAdapter<true, std::ostream&> fs(file);
    libendian::EndianOStreamAdapter<false, std::ostream&> fsLE(file);
    fs.write("FORM", 4);
    fs << uint32_t(14);
    fs.write("XDIR", 4);
    fs.write("INFO", 4);
    fs << uint32_t(sizeof(numTracks));
    fsLE << numTracks;
    fs.write("CAT ", 4);
    uint32_t len = 4; // "XMID"
    std::vector<uint32_t> trackLengths(numTracks);
    for(unsigned i = 0; i < numTracks; i++)
    {
        uint32_t trackLength = 4 + sizeof(uint32_t) + 4; // XMID EVNT len
        const XMIDI_Track& track = tracklist[i];
        trackLength += static_cast<uint32_t>(track.getData().size());
        if(!track.getTimbres().empty())
            trackLength += 4 + sizeof(uint32_t) + sizeof(uint16_t) + static_cast<uint32_t>(track.getTimbres().size()) * 2; //-V119
        trackLengths[i] = trackLength;
        len += 4 + sizeof(uint32_t) + trackLength; // FORM len
    }
    fs << len;
    fs.write("XMID", 4);
    for(unsigned i = 0; i < numTracks && !!fs; i++)
    {
        fs.write("FORM", 4);
        fs << uint32_t(trackLengths[i]); // - FORM
        fs.write("XMID", 4);
        const XMIDI_Track& track = tracklist[i];
        if(!track.getTimbres().empty())
        {
            fs.write("TIMB", 4);
            fs << uint32_t(track.getTimbres().size() * 2 + 2);
            fsLE << uint16_t(track.getTimbres().size());
            fs.writeRaw(&track.getTimbres()[0], track.getTimbres().size());
        }
        fs.write("EVNT", 4);
        fs << uint32_t(track.getData().size());
        fs << track.getData();
    }

    return (!file) ? ErrorCode::UNEXPECTED_EOF : ErrorCode::NONE;
}

bool ArchivItem_Sound_XMidi::isTrackConverted(uint16_t track) const
{
    return track < midiTracklist.size() && !midiTracklist[track].getData().empty();
}

const MIDI_Track& ArchivItem_Sound_XMidi::getMidiTrack(uint16_t trackIdx)
{
    if(!isTrackConverted(trackIdx))
        midiTracklist[trackIdx] = const_cast<const ArchivItem_Sound_XMidi*>(this)->getMidiTrack(trackIdx);
    return midiTracklist[trackIdx];
}

MIDI_Track ArchivItem_Sound_XMidi::getMidiTrack(uint16_t trackIdx) const
{
    if(isTrackConverted(trackIdx))
        return midiTracklist[trackIdx];
    const XMIDI_Track* origTrack = getTrack(trackIdx);
    if(!origTrack)
        throw std::out_of_range("Invalid XMidi track index");
    XMIDI_TrackConverter converter(*origTrack);
    if(!converter.Convert())
        throw std::runtime_error("Invalid XMIDI track detected");
    return converter.CreateMidiTrack();
}

void ArchivItem_Sound_XMidi::addTrack(const XMIDI_Track& track)
{
    if(getNumTracks() >= tracklist.size())
        throw std::runtime_error("No more space for tracks");
    tracklist[numTracks++] = track;
}

} // namespace libsiedler2
