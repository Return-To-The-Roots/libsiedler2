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
#ifndef ARCHIVITEM_FONT_H_INCLUDED
#define ARCHIVITEM_FONT_H_INCLUDED

#pragma once

#include "ArchivItem.h"
#include "ArchivItem_Palette.h"
#include "ArchivInfo.h"

namespace libsiedler2
{

/// Klasse für Fonts.
    class ArchivItem_Font : public ArchivItem, public ArchivInfo
    {
        public:
            /// Konstruktor von @p ArchivItem_Font.
            ArchivItem_Font(void);

            /// lädt die Fontdaten aus einer Datei.
            int load(std::istream& file, const ArchivItem_Palette* palette);

            /// schreibt die Fontdaten in eine Datei.
            int write(std::ostream& file, const ArchivItem_Palette* palette) const;

            /// liefert den X-Buchstabenabstand.
            inline unsigned char getDx(void) const
            {
                return dx;
            }

            /// liefert den Y-Buchstabenabstand.
            inline unsigned char getDy(void) const
            {
                return dy;
            }

            /// setzt den X-Buchstabenabstand.
            inline void setDx(unsigned char dx)
            {
                this->dx = dx;
            }

            /// setzt den Y-Buchstabenabstand.
            inline void setDy(unsigned char dy)
            {
                this->dy = dy;
            }

        protected:
            unsigned char dx; ///< X-Buchstabenabstand.
            unsigned char dy; ///< Y-Buchstabenabstand.
    };

}

#endif // !ARCHIVITEM_FONT_H_INCLUDED
