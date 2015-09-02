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
#ifndef ARCHIVITEM_SOUND_WAVE_H_INCLUDED
#define ARCHIVITEM_SOUND_WAVE_H_INCLUDED

#pragma once

#include "ArchivItem_Sound.h"
#include <vector>

namespace libsiedler2
{
    /// Basisklasse für WAVE-Sounds.
    class baseArchivItem_Sound_Wave : public virtual baseArchivItem_Sound
    {
        public:
            /// Konstruktor von @p baseArchivItem_Sound_Wave.
            baseArchivItem_Sound_Wave(void);

            /// Destruktor von @p baseArchivItem_Sound_Wave.
            ~baseArchivItem_Sound_Wave(void);

            /// lädt die Wave-Daten aus einer Datei.
            int load(std::istream& file, unsigned int length);

            /// schreibt die Wave-Daten in eine Datei.
            int write(std::ostream& file) const { return write(file, false); }
            int write(std::ostream& file, bool stripheader = false) const;

            /// räumt den Soundspeicher auf.
            void clear(void);

            void setData(const std::vector<unsigned char>& data)
            {
                this->data = data;
            }

            const std::vector<unsigned char>& getData() { return data; }
            unsigned int getLength() { return data.size(); }

        protected:
            std::vector<unsigned char> data;
    };

    /// Klasse für WAVE-Sounds.
    class ArchivItem_Sound_Wave : public virtual baseArchivItem_Sound_Wave, public ArchivItem_Sound
    {
        public:
            /// Konstruktor von @p ArchivItem_Sound_Wave.
            ArchivItem_Sound_Wave(void) : baseArchivItem_Sound_Wave() {}

            /// Kopierkonstruktor von @p ArchivItem_Sound_Wave.
            ArchivItem_Sound_Wave(const ArchivItem_Sound_Wave& item) : baseArchivItem_Sound_Wave(item) {}
    };
}

#endif // !ARCHIVITEM_SOUND_WAVE_H_INCLUDED
