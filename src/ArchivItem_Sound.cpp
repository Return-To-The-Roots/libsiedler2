// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

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

ArchivItem_Sound::ArchivItem_Sound(SoundType soundType) : ArchivItem(BobType::Sound), soundType_(soundType) {}

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
            sndType = SoundType::XMidi;
        else if(isChunk(header, "WAVE"))
            sndType = SoundType::Wave;
        else
        {
            fs.setPosition(oldpos);
            return nullptr;
        }
    } else if(isChunk(header, "MThd"))
        sndType = SoundType::Midi;
    else if(isChunk(header, "OggS"))
        sndType = SoundType::OGG;
    else if(isChunk(header, "ID3") || isChunk(header, "\xFF\xFB"))
        sndType = SoundType::MP3;
    else // wave-format ohne header?
        sndType = SoundType::Wave;

    fs.setPosition(oldpos);
    return getAllocator().create<ArchivItem_Sound>(BobType::Sound, sndType);
}
} // namespace libsiedler2
