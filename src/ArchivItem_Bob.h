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
#ifndef ARCHIVITEM_BOB_H_INCLUDED
#define ARCHIVITEM_BOB_H_INCLUDED

#pragma once

#include "ArchivItem.h"
#include "ArchivInfo.h"
#include <vector>
#include <iosfwd>
#include <stdint.h>

namespace libsiedler2 { class ArchivItem_Palette; }

namespace libsiedler2
{
    /// Klasse für Bobfiles.
    class ArchivItem_Bob : public ArchivItem, public ArchivInfo
    {
        public:
            ArchivItem_Bob();

            /// Konstruktor von @p ArchivItem_Bob mit Laden der Bob-Daten.
            ArchivItem_Bob(std::istream& file, const ArchivItem_Palette* palette);

            ~ArchivItem_Bob() override;

            /// lädt die Bobdaten aus einer Datei.
            int load(std::istream& file, const ArchivItem_Palette* palette);

            /// schreibt die Bobdaten in eine Datei.
            int write(std::ostream& file, const ArchivItem_Palette* palette) const;

            uint16_t getLink(uint32_t idx) {return(links[idx]);};

        protected:
            uint16_t good_count; /// Warenanzahl
            uint16_t item_count; /// Bildanzahl
            std::vector<uint16_t> links;     /// "Links" (Zugehörigkeiten der Bilder)
    };
}

#endif // !ARCHIVITEM_BOB_H_INCLUDED
