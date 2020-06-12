// Copyright (c) 2005 - 2020 Settlers Freaks (sf-team at siedler25.org)
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

#include "Archiv.h"
#include "ArchivItem.h"
#include <cstdint>
#include <iosfwd>
#include <vector>

namespace libsiedler2 {
class ArchivItem_Palette;
}

namespace libsiedler2 {
/// Klasse für Bobfiles.
class ArchivItem_Bob : public ArchivItem, public Archiv
{
public:
    ArchivItem_Bob();

    ~ArchivItem_Bob() override;
    RTTR_CLONEABLE(ArchivItem_Bob)

    /// lädt die Bobdaten aus einer Datei.
    int load(std::istream& file, const ArchivItem_Palette* palette);

    /// schreibt die Bobdaten in eine Datei.
    static int write(std::ostream& file, const ArchivItem_Palette* palette);

    uint32_t getNumGoodImgs() const { return numGoodImgs; }
    uint32_t getNumItems() const { return uint32_t(links.size()); }
    uint16_t getLink(uint32_t idx) const { return (links[idx]); };

    /// Write the links in mapping format (TAB separated entries with # comments)
    void writeLinks(std::ostream& file);

protected:
    uint16_t numGoodImgs;        /// Number of pictures for wares
    std::vector<uint16_t> links; /// "Links" (Zugehörigkeiten der Bilder)
};
} // namespace libsiedler2

#endif // !ARCHIVITEM_BOB_H_INCLUDED
