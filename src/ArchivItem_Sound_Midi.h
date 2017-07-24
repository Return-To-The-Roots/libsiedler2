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
#ifndef ARCHIVITEM_SOUND_MIDI_H_INCLUDED
#define ARCHIVITEM_SOUND_MIDI_H_INCLUDED

#pragma once

#include "ArchivItem_Sound.h"
#include "MIDI_Track.h"
#include <boost/array.hpp>
#include <stdint.h>

namespace libsiedler2
{
    /// Basisklasse für MIDI-Sounds.
    class baseArchivItem_Sound_Midi : public virtual baseArchivItem_Sound
    {
        public:
            baseArchivItem_Sound_Midi();

            baseArchivItem_Sound_Midi(const baseArchivItem_Sound_Midi& item);

            ~baseArchivItem_Sound_Midi() override;

            baseArchivItem_Sound_Midi& operator=(const baseArchivItem_Sound_Midi& item);

            int load(std::istream& file, uint32_t length) override;
            int write(std::ostream& file) const override;

            const MIDI_Track* getTrack(uint16_t track) const { if(track < tracks) return &tracklist[track]; return NULL; }
            uint16_t getTrackCount() const { return tracks; }

        protected:
            uint16_t tracks;
            boost::array<MIDI_Track, 256> tracklist; //-V730_NOINIT
    };

    /// Klasse für MIDI-Sounds.
    class ArchivItem_Sound_Midi : public virtual baseArchivItem_Sound_Midi, public ArchivItem_Sound
    {
        public:
            ArchivItem_Sound_Midi() : baseArchivItem_Sound_Midi() {}

            ArchivItem_Sound_Midi(const ArchivItem_Sound_Midi& item) : baseArchivItem_Sound_Midi(item) {}
    };
}

#endif // !ARCHIVITEM_SOUND_MIDI_H_INCLUDED
