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
#ifndef ARCHIVITEM_SOUND_H_INCLUDED
#define ARCHIVITEM_SOUND_H_INCLUDED

#pragma once

#include "ArchivItem.h"
#include "enumTypes.h"
#include <iosfwd>
#include <stdint.h>

namespace libsiedler2
{
    /// Basis-Basisklasse für Sounditems.
    class baseArchivItem_Sound : public ArchivItem
    {
        public:
            baseArchivItem_Sound();
            ~baseArchivItem_Sound() override;

            /// setzt den Typ des Sounds.
            void setType(SOUNDTYPES type);

            /// liefert den Typ des Sounds.
            SOUNDTYPES getType() const;

            /// lädt die Sound-Daten aus einer Datei.
            virtual int load(std::istream& file, uint32_t length) = 0;

            /// schreibt die Sound-Daten in eine Datei.
            virtual int write(std::ostream& file) const = 0;

            static baseArchivItem_Sound* findSubType(std::istream& file);

        protected:
            SOUNDTYPES type_;
    };

    /// Basisklasse für Sounditems.
    class ArchivItem_Sound : public virtual baseArchivItem_Sound
    {
    };
}

#endif // !ARCHIVITEM_SOUND_H_INCLUDED
