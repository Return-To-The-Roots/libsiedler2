// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "ArchivItem.h"
#include <cstdint>
#include <iosfwd>
#include <vector>

namespace libsiedler2 {
/// Klasse für Rawdaten.
class ArchivItem_Raw final : public ArchivItem
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
