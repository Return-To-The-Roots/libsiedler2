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
#ifndef ARCHIVITEM_INI_H_INCLUDED
#define ARCHIVITEM_INI_H_INCLUDED

#pragma once

#include "ArchivInfo.h"
#include "ArchivItem.h"
#include <iosfwd>
#include <string>

namespace libsiedler2 {

/// Klasse für INI-Dateien (genauergesagt eine Sektion).
class ArchivItem_Ini : public ArchivItem, public ArchivInfo
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

#endif // !ARCHIVITEM_INI_H_INCLUDED
