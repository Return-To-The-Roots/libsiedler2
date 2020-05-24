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

#include "ArchivItem_Sound.h"
#include "IAllocator.h"
#include "fileFormatHelpers.h"
#include "libsiedler2.h"
#include "libendian/EndianIStreamAdapter.h"
#include <iostream>

namespace libsiedler2 {
/** @class ArchivItem_Sound
 *
 *  Klasse für Sounditems.
 */

ArchivItem_Sound::ArchivItem_Sound(SoundType soundType) : ArchivItem(BOBTYPE_SOUND), soundType_(soundType) {}

ArchivItem_Sound::~ArchivItem_Sound() = default;

/**
 *  liefert den Typ des Sounds.
 *
 *  @return Der eingestellte Typ
 */
SoundType ArchivItem_Sound::getType() const
{
    return soundType_;
}

std::unique_ptr<ArchivItem_Sound> ArchivItem_Sound::findSubType(std::istream& file)
{
    libendian::EndianIStreamAdapter<false, std::istream&> fs(file);
    long oldpos = fs.getPosition();

    std::array<char, 4> header;
    fs >> header;

    SoundType sndType;
    // ist es eine RIFF-File? (Header "FORM" bzw "RIFF")
    if(isChunk(header, "FORM") || isChunk(header, "RIFF"))
    {
        uint32_t length;
        fs >> length >> header;

        if(isChunk(header, "XMID") || isChunk(header, "XDIR"))
            sndType = SOUNDTYPE_XMIDI;
        else if(isChunk(header, "WAVE"))
            sndType = SOUNDTYPE_WAVE;
        else
        {
            fs.setPosition(oldpos);
            return nullptr;
        }
    } else if(isChunk(header, "MThd"))
        sndType = SOUNDTYPE_MIDI;
    else if(isChunk(header, "OggS"))
        sndType = SOUNDTYPE_OGG;
    else if(isChunk(header, "ID3") || isChunk(header, "\xFF\xFB"))
        sndType = SOUNDTYPE_MP3;
    else // wave-format ohne header?
        sndType = SOUNDTYPE_WAVE;

    fs.setPosition(oldpos);
    return getAllocator().create<ArchivItem_Sound>(BOBTYPE_SOUND, sndType);
}
} // namespace libsiedler2
