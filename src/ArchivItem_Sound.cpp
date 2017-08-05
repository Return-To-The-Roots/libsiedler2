// Copyright (c) 2005 - 2017 Settlers Freaks (sf-team at siedler25.org)
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
#include "ArchivItem_Sound.h"
#include "IAllocator.h"
#include "fileFormatHelpers.h"
#include "libsiedler2.h"
#include "libendian/src/EndianIStreamAdapter.h"
#include <iostream>

/** @class libsiedler2::ArchivItem_Sound
 *
 *  Basisklasse für Sounditems.
 */

libsiedler2::baseArchivItem_Sound::baseArchivItem_Sound() : ArchivItem(), soundType_(SOUNDTYPE_NONE)
{
    bobtype_ = BOBTYPE_SOUND;
}

libsiedler2::baseArchivItem_Sound::~baseArchivItem_Sound()
{
}

/**
 *  liefert den Typ des Sounds.
 *
 *  @return Der eingestellte Typ
 */
libsiedler2::SoundType libsiedler2::baseArchivItem_Sound::getType() const
{
    return soundType_;
}

libsiedler2::baseArchivItem_Sound* libsiedler2::baseArchivItem_Sound::findSubType(std::istream& file)
{
    libendian::EndianIStreamAdapter<false, std::istream&> fs(file);
    long oldpos = fs.getPosition();
    baseArchivItem_Sound* item = NULL;

    char header[4] = "\0\0\0";
    fs >> header;

    // ist es eine RIFF-File? (Header "FORM" bzw "RIFF")
    if(isChunk(header, "FORM") || isChunk(header, "RIFF"))
    {
        uint32_t length;
        fs >> length >> header;

        if(isChunk(header, "XMID") || isChunk(header, "XDIR"))
        {
            // xmidi
            item = dynamic_cast<baseArchivItem_Sound*>(getAllocator().create(BOBTYPE_SOUND, SOUNDTYPE_XMIDI));
        } else if(isChunk(header, "WAVE"))
        {
            // wave-format inkl-header
            item = dynamic_cast<baseArchivItem_Sound*>(getAllocator().create(BOBTYPE_SOUND, SOUNDTYPE_WAVE));
        }
    } else if(isChunk(header, "MThd"))
    {
        // midi
        item = dynamic_cast<baseArchivItem_Sound*>(getAllocator().create(BOBTYPE_SOUND, SOUNDTYPE_MIDI));
    } else if(isChunk(header, "OggS") || isChunk(header, "ID3") || isChunk(header, "\xFF\xFB"))
    {
        // ogg, mp3 (id3tag, ohne),
        item = dynamic_cast<baseArchivItem_Sound*>(getAllocator().create(BOBTYPE_SOUND, SOUNDTYPE_OTHER));
    } else
    {
        // wave-format ohne header?
        item = dynamic_cast<baseArchivItem_Sound*>(getAllocator().create(BOBTYPE_SOUND, SOUNDTYPE_WAVE));
    }

    fs.setPosition(oldpos);
    return item;
}
