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
