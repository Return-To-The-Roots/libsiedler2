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
#ifndef ARCHIVITEM_SOUND_XMIDI_H_INCLUDED
#define ARCHIVITEM_SOUND_XMIDI_H_INCLUDED

#pragma once

#include "ArchivItem_Sound.h"
#include "MIDI_Track.h"
#include "XMIDI_Track.h"
#include <boost/array.hpp>
#include <stdint.h>

namespace libsiedler2
{
    /// Basisklasse für XMIDI-Sounds.
    class baseArchivItem_Sound_XMidi : public virtual baseArchivItem_Sound
    {
        public:
            baseArchivItem_Sound_XMidi();

            baseArchivItem_Sound_XMidi(const baseArchivItem_Sound_XMidi& item);

            ~baseArchivItem_Sound_XMidi() override;

            int load(std::istream& file, uint32_t length) override;
            int write(std::ostream& file) const override;

            XMIDI_Track* getTrack(uint16_t track) { if(track < numTracks) return &tracklist[track]; return NULL; }
            const XMIDI_Track* getTrack(uint16_t track) const { if(track < numTracks) return &tracklist[track]; return NULL; }
            MIDI_Track* getMidiTrack(uint16_t trackIdx);
            uint16_t getTrackCount() const { return numTracks; }

            void addTrack(const XMIDI_Track& track);

        protected:
            uint16_t numTracks, ppqs;
            boost::array<XMIDI_Track, 256> tracklist; //-V730_NOINIT
            boost::array<MIDI_Track, 256> midiTracklist; //-V730_NOINIT
    };

    /// Klasse für XMIDI-Sounds.
    class ArchivItem_Sound_XMidi : public virtual baseArchivItem_Sound_XMidi, public ArchivItem_Sound
    {
        public:
            ArchivItem_Sound_XMidi() : baseArchivItem_Sound_XMidi() {}

            ArchivItem_Sound_XMidi(const ArchivItem_Sound_XMidi& item) : baseArchivItem_Sound(item), baseArchivItem_Sound_XMidi(item) {}
    };
}

#endif // !ARCHIVITEM_SOUND_XMIDI_H_INCLUDED
