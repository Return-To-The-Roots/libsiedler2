// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "ArchivItem.h"
#include <cstdint>
#include <string>

namespace libsiedler2 {
/// Klasse für Texte.
class ArchivItem_Text : public ArchivItem
{
public:
    ArchivItem_Text();

    ~ArchivItem_Text() override;
    RTTR_CLONEABLE(ArchivItem_Text)

    /// Load all text from the file
    int load(std::istream& file, bool conversion = true);

    /// Write the text to the file
    int write(std::ostream& file, bool conversion = true) const;

    /// Return the text
    const std::string& getText() const { return text_; }
    /// Get the text as it will be written to file
    std::string getFileText(bool conversion) const;

    /// Set the text.
    /// If convertFromOem is true, the text will be converted from OEM format including @@->\n replacement
    /// Otherwise the text will only be canonicalized, i.e. \r and \r\n will be replaced by \n
    void setText(const std::string& text, bool convertFromOem = false);

private:
    std::string text_; /// Der Textspeicher.
};
} // namespace libsiedler2
