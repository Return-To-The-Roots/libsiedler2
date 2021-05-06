// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "Archiv.h"
#include "ArchivItem.h"
#include <iosfwd>
#include <string>

namespace libsiedler2 {

/// Klasse für INI-Dateien (genauergesagt eine Sektion).
class ArchivItem_Ini : public ArchivItem, public Archiv
{
public:
    ArchivItem_Ini();

    ArchivItem_Ini(const std::string& name);

    RTTR_CLONEABLE(ArchivItem_Ini)

    /// lädt die INI-Daten aus einer Datei.
    int load(std::istream& file);

    /// schreibt die INI-Daten in eine Datei.
    int write(std::ostream& file) const;

    /// liest einen Wert aus der Ini
    std::string getValue(const std::string& name) const;

    int getValueI(const std::string& name) const;

    /// fügt einen Eintrag hinzu.
    void addValue(const std::string& name, const std::string& value);

    /// schreibt einen Wert in die Ini
    void setValue(const std::string& name, const std::string& value);
    void setValue(const std::string& name, int value);
};

} // namespace libsiedler2
