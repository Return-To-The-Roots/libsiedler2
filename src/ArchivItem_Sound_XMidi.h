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
#ifndef ARCHIVITEM_SOUND_XMIDI_H_INCLUDED
#define ARCHIVITEM_SOUND_XMIDI_H_INCLUDED

#pragma once

#include "ArchivItem_Sound.h"
#include "MIDI_Track.h"
#include <boost/array.hpp>

namespace libsiedler2
{
    /// Basisklasse für XMIDI-Sounds.
    class baseArchivItem_Sound_XMidi : public virtual baseArchivItem_Sound
    {
        public:
            /// Konstruktor von @p baseArchivItem_Sound_XMidi.
            baseArchivItem_Sound_XMidi(void);

            /// Kopierkonstruktor von @p baseArchivItem_Sound_XMidi.
            baseArchivItem_Sound_XMidi(const baseArchivItem_Sound_XMidi& item);

            /// Destruktor von @p baseArchivItem_Sound_XMidi.
            ~baseArchivItem_Sound_XMidi(void) override;

            baseArchivItem_Sound_XMidi& operator=(const baseArchivItem_Sound_XMidi& item);

            int load(std::istream& file, unsigned int length) override;
            int write(std::ostream& file) const override;

            MIDI_Track* getTrack(unsigned short track) { if(track < tracks) return &tracklist[track]; return NULL; }
            unsigned short getTrackCount() const { return tracks; }

        protected:
            unsigned short tracks;
            boost::array<MIDI_Track, 256> tracklist; //-V730_NOINIT
    };

    /// Klasse für XMIDI-Sounds.
    class ArchivItem_Sound_XMidi : public virtual baseArchivItem_Sound_XMidi, public ArchivItem_Sound
    {
        public:
            /// Konstruktor von @p ArchivItem_Sound_XMidi.
            ArchivItem_Sound_XMidi(void) : baseArchivItem_Sound_XMidi() {}

            /// Kopierkonstruktor von @p ArchivItem_Sound_XMidi.
            ArchivItem_Sound_XMidi(const ArchivItem_Sound_XMidi& item) : baseArchivItem_Sound_XMidi(item) {}
    };
}

#endif // !ARCHIVITEM_SOUND_XMIDI_H_INCLUDED
