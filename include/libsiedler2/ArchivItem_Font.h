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
#ifndef ARCHIVITEM_FONT_H_INCLUDED
#define ARCHIVITEM_FONT_H_INCLUDED

#pragma once

#include "Archiv.h"
#include "ArchivItem.h"
#include <iosfwd>
#include <stdint.h>

namespace libsiedler2 {
class ArchivItem_Palette;
}

namespace libsiedler2 {

/// Klasse für Fonts.
class ArchivItem_Font : public ArchivItem, public Archiv
{
public:
    ArchivItem_Font();
    RTTR_CLONEABLE(ArchivItem_Font)

    /// lädt die Fontdaten aus einer Datei.
    int load(std::istream& file, const ArchivItem_Palette* palette);

    /// schreibt die Fontdaten in eine Datei.
    int write(std::ostream& file, const ArchivItem_Palette* palette) const;

    /// liefert den X-Buchstabenabstand.
    uint8_t getDx() const { return dx; }

    /// liefert den Y-Buchstabenabstand.
    uint8_t getDy() const { return dy; }

    /// setzt den X-Buchstabenabstand.
    void setDx(uint8_t dx) { this->dx = dx; }

    /// setzt den Y-Buchstabenabstand.
    void setDy(uint8_t dy) { this->dy = dy; }

    bool isUnicode;

protected:
    uint8_t dx; /// X-Buchstabenabstand.
    uint8_t dy; /// Y-Buchstabenabstand.
};

} // namespace libsiedler2

#endif // !ARCHIVITEM_FONT_H_INCLUDED
