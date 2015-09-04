// $Id: ArchivItem_Sound_Midi.h 9359 2014-04-25 15:37:22Z FloSoft $
//
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

namespace libsiedler2
{
    /// Basisklasse für MIDI-Sounds.
    class baseArchivItem_Sound_Midi : public virtual baseArchivItem_Sound
    {
        public:
            /// Konstruktor von @p baseArchivItem_Sound_Midi.
            baseArchivItem_Sound_Midi(void);

            /// Kopierkonstruktor von @p baseArchivItem_Sound_Midi.
            baseArchivItem_Sound_Midi(const baseArchivItem_Sound_Midi& item);

            /// Destruktor von @p baseArchivItem_Sound_Midi.
            ~baseArchivItem_Sound_Midi(void);

            int load(FILE* file, unsigned int length);
            int write(FILE* file) const;

            const MIDI_Track* getTrack(unsigned short track) const { if(track < tracks) return &tracklist[track]; return NULL; }
            unsigned short getTrackCount() const { return tracks; }

        protected:
            unsigned short tracks;
            MIDI_Track tracklist[256];
    };

    /// Klasse für MIDI-Sounds.
    class ArchivItem_Sound_Midi : public virtual baseArchivItem_Sound_Midi, public ArchivItem_Sound
    {
        public:
            /// Konstruktor von @p ArchivItem_Sound_Midi.
            ArchivItem_Sound_Midi(void) : baseArchivItem_Sound_Midi() {}

            /// Kopierkonstruktor von @p ArchivItem_Sound_Midi.
            ArchivItem_Sound_Midi(const ArchivItem_Sound_Midi& item) : baseArchivItem_Sound_Midi(item) {}
    };
}

#endif // !ARCHIVITEM_SOUND_MIDI_H_INCLUDED
