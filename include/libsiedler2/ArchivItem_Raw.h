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
#ifndef ARCHIVITEM_RAW_H_INCLUDED
#define ARCHIVITEM_RAW_H_INCLUDED

#pragma once

#include "ArchivItem.h"
#include <cstdint>
#include <iosfwd>
#include <vector>

namespace libsiedler2 {
/// Klasse für Rawdaten.
class ArchivItem_Raw : public ArchivItem
{
public:
    ArchivItem_Raw();
    ArchivItem_Raw(std::vector<uint8_t> initialData);
    RTTR_CLONEABLE(ArchivItem_Raw)

    /// lädt die Rawdaten aus einer Datei.
    int load(std::istream& file, uint32_t length = 0xFFFFFFFF);
    /// schreibt die Rawdaten in eine Datei.
    int write(std::ostream& file, bool with_length) const;

    /// liefert die Daten zurück (ro).
    const std::vector<uint8_t>& getData() const;
    /// liefert die Daten zurück (rw).
    std::vector<uint8_t>& getData();

    /// löscht den Datenblock.
    void clear();

private:
    std::vector<uint8_t> data; /// Die Daten.
};

} // namespace libsiedler2

#endif // !ARCHIVITEM_RAW_H_INCLUDED
