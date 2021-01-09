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

#include "ArchivItem_Text.h"
#include "ErrorCodes.h"
#include "oem.h"
#include <boost/algorithm/string/replace.hpp>
#include <cassert>
#include <iostream>
#include <limits>
#include <vector>

libsiedler2::ArchivItem_Text::ArchivItem_Text() : ArchivItem(BobType::Text) {}

libsiedler2::ArchivItem_Text::~ArchivItem_Text() = default;

/**
 *  liest den Text aus einer Datei.
 *
 *  @param[in] file       Dateihandle aus denen der Text geladen werden sollen
 *  @param[in] conversion Soll ggf. OEM-Charset in ANSI umgewandelt werden?
 *
 *  @return liefert Null bei Erfolg, ungleich Null bei Fehler
 */
int libsiedler2::ArchivItem_Text::load(std::istream& file, bool conversion)
{
    if(!file)
        return ErrorCode::FILE_NOT_ACCESSIBLE;

    // Read all that is there
    std::string text{std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()};
    // Remove nullptr terminator if present
    if(!text.empty() && text.back() == '\0')
        text.pop_back();

    if(conversion)
    {
        // S2 sometimes uses the old end-of-file marker SUB, remove all following it if it is present
        const auto posSub = text.find('\26');
        if(posSub != std::string::npos)
            text.resize(posSub);
    }

    setText(text, conversion);
    return ErrorCode::NONE;
}

/**
 *  schreibt dem Text in eine Datei.
 *
 *  @param[in] file       Dateihandle in die der Text geschrieben werden soll
 *  @param[in] conversion Soll ggf. ANSI-Charset in OEM umgewandelt werden?
 *
 *  @return liefert Null bei Erfolg, ungleich Null bei Fehler
 */
int libsiedler2::ArchivItem_Text::write(std::ostream& file, bool conversion) const
{
    if(!file)
        return ErrorCode::FILE_NOT_ACCESSIBLE;

    // Wenn Länge 0, nix schreiben, ist ja kein Fehler!
    if(text_.empty())
        return ErrorCode::NONE;

    const std::string convText = getFileText(conversion);
    if(!file.write(convText.data(), convText.size()))
        return ErrorCode::UNEXPECTED_EOF;

    return ErrorCode::NONE;
}

std::string libsiedler2::ArchivItem_Text::getFileText(bool conversion) const
{
    if(!conversion)
        return text_;

    const std::string convertedText = boost::replace_all_copy(text_, "\n", "@@");
    assert(convertedText.find('\r') == std::string::npos);

    return AnsiToOem(convertedText);
}

void libsiedler2::ArchivItem_Text::setText(const std::string& text, bool convertFromOem)
{
    if(convertFromOem)
    {
        text_ = OemToAnsi(text);
        boost::replace_all(text_, "@@", "\n");
    } else
        text_ = boost::replace_all_copy(text, "\r\n", "\n");
    boost::replace_all(text_, "\r", "\n");
}
