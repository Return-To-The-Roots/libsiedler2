// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "Archiv.h"
#include "ArchivItem.h"
#include <iosfwd>
#include <string>

namespace libsiedler2 {
class ArchivItem_Text;

/// Klasse für INI-Dateien (genauergesagt eine Sektion).
class ArchivItem_Ini : public ArchivItem, public Archiv
{
    ArchivItem_Text* getItem(const std::string& name);
    const ArchivItem_Text* getItem(const std::string& name, bool enforce_exists = false) const;

    // Add a value
    void addValue(const std::string& name, const std::string& value);

public:
    ArchivItem_Ini();

    ArchivItem_Ini(const std::string& name);

    RTTR_CLONEABLE(ArchivItem_Ini)

    /// lädt die INI-Daten aus einer Datei.
    int load(std::istream& file);

    /// schreibt die INI-Daten in eine Datei.
    int write(std::ostream& file) const;

    /// Get a value by name, throw if it does not exist
    std::string getValue(const std::string& name) const;
    int getIntValue(const std::string& name) const;
    bool getBoolValue(const std::string& name) const;

    /// Get a value by name, return default if it does not exist
    std::string getValue(const std::string& name, const std::string& defaultVal) const;
    std::string getValue(const std::string& name, const char* defaultVal) const
    {
        return getValue(name, std::string(defaultVal));
    }
    int getValue(const std::string& name, int defaultVal) const;
    int getValue(const std::string& name, bool defaultVal) const;

    /// Set or change a value
    void setValue(const std::string& name, const std::string& value);
    void setValue(const std::string& name, int value);
};

} // namespace libsiedler2
