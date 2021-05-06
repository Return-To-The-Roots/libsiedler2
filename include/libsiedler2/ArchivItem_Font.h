// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "Archiv.h"
#include "ArchivItem.h"
#include <cstdint>
#include <iosfwd>

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
