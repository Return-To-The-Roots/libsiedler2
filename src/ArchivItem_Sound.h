// $Id: ArchivItem_Sound.h 9359 2014-04-25 15:37:22Z FloSoft $
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
#ifndef ARCHIVITEM_SOUND_H_INCLUDED
#define ARCHIVITEM_SOUND_H_INCLUDED

#pragma once

#include "ArchivItem.h"
#include "enumTypes.h"

namespace libsiedler2
{
    /// Basis-Basisklasse für Sounditems.
    class baseArchivItem_Sound : public ArchivItem
    {
        public:
            /// Konstruktor von @p ArchivItem_Sound.
            baseArchivItem_Sound(void);

            /// Kopierkonstruktor von @p ArchivItem_Sound.
            baseArchivItem_Sound(const baseArchivItem_Sound& item);

            /// virtueller Destruktor von @p ArchivItem_Sound.
            virtual ~baseArchivItem_Sound(void);

            /// setzt den Typ des Sounds.
            void setType(SOUNDTYPES type);

            /// liefert den Typ des Sounds.
            SOUNDTYPES getType(void) const;

            /// lädt die Sound-Daten aus einer Datei.
            virtual int load(FILE* file, unsigned int length);

            /// schreibt die Sound-Daten in eine Datei.
            virtual int write(FILE* file) const;

            static baseArchivItem_Sound* findSubType(FILE* file);

        protected:
            SOUNDTYPES type;
    };

    /// Basisklasse für Sounditems.
    class ArchivItem_Sound : public virtual baseArchivItem_Sound
    {
    };
}

#endif // !ARCHIVITEM_SOUND_H_INCLUDED
