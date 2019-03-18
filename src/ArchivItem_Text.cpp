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

#include "libSiedler2Defines.h" // IWYU pragma: keep
#include "ArchivItem_Text.h"
#include "ErrorCodes.h"
#include "oem.h"
#include <cassert>
#include <iostream>
#include <limits>
#include <vector>

/** @class libsiedler2::ArchivItem_Text
 *
 *  Klasse für Texte.
 */

/** @var libsiedler2::ArchivItem_Text::text
 *
 *  Der Textspeicher.
 */

/** @var libsiedler2::ArchivItem_Text::length
 *
 *  Die Länge des Textes.
 */

libsiedler2::ArchivItem_Text::ArchivItem_Text() : ArchivItem(BOBTYPE_TEXT) {}

libsiedler2::ArchivItem_Text::~ArchivItem_Text() = default;

/**
 *  liest den Text aus einer Datei.
 *
 *  @param[in] file       Dateihandle aus denen der Text geladen werden sollen
 *  @param[in] conversion Soll ggf. OEM-Charset in ANSI umgewandelt werden?
 *  @param[in] length     Länge des Blocks (Wieviel Bytes sollen eingelesen werden?)
 *
 *  @return liefert Null bei Erfolg, ungleich Null bei Fehler
 *
 *  @todo Hmm nur temporärer Fix! ist dieses doofe Escape-zeichen am Ende der Files
 */
int libsiedler2::ArchivItem_Text::load(std::istream& file, bool conversion, uint32_t length)
{
    if(!file)
        return ErrorCode::FILE_NOT_ACCESSIBLE;

    std::vector<char> text;
    if(length)
    {
        // Avoid copy if we need to append nullptr terminator
        text.reserve(length + 1);
        text.resize(length);
        if(!file.read(&text.front(), length))
            return ErrorCode::UNEXPECTED_EOF;
    } else
    {
        // Read all that is there
        text.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
    }

    // Add nullptr terminator if it is missing
    if(text.empty() || text.back() != 0)
        text.push_back(0);

    /// TODO: Hmm nur temporärer Fix! ist dieses doofe Escape-zeichen am Ende der Files
    if(text.size() >= 2 && text[text.size() - 2] == 26)
        text[text.size() - 2] = 0;

    if(conversion)
        OemToAnsi(&text.front(), &text.front()); //-V742

    for(uint32_t i = 0; i + 1 < text.size(); ++i)
    {
        if(text[i] == '@' && text[i + 1] == '@')
        {
            text[i] = '\r';
            text[i + 1] = '\n';
        }
    }

    this->text_ = &text.front();
    // Text setzen
    setName(this->text_);

    // Alles OK
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

    std::string convText = getFileText(conversion);
    if(!file.write(&convText[0], convText.size()))
        return ErrorCode::UNEXPECTED_EOF;

    return ErrorCode::NONE;
}

/**
 *  liefert den Text.
 *
 *  @return liefert einen konstanten Zeiger auf das Textelement, nullptr bei leerem Text
 */
const std::string& libsiedler2::ArchivItem_Text::getText() const
{
    return text_;
}

std::string libsiedler2::ArchivItem_Text::getFileText(bool convertToOem) const
{
    assert(text_.size() < std::numeric_limits<uint32_t>::max());
    const auto length = static_cast<uint32_t>(text_.size());
    std::vector<char> textBuf;
    textBuf.reserve(length * 2 + 1);

    for(uint32_t i = 0; i < length; ++i)
    {
        if(this->text_[i] == '\n')
        {
            textBuf.push_back('@');
            textBuf.push_back('@');
        } else if(this->text_[i] == '\r')
            continue;
        else
            textBuf.push_back(this->text_[i]);
    }
    textBuf.push_back('\0');

    if(convertToOem)
        AnsiToOem(&textBuf.front(), &textBuf.front()); //-V742
    return &textBuf.front();
}

/**
 *  setzt den Text.
 *
 *  @param[in] text       Der Text der gesetzt werden soll, falls @p nullptr, wird
 *                        evtl vorhandener Text gelöscht
 *  @param[in] length     Länge des Textes, bei @p 0 wird @p strlen verwendet
 */
void libsiedler2::ArchivItem_Text::setText(const std::string& text)
{
    this->text_ = text;

    // Name setzen
    if(getName().empty())
        setName(this->text_);
}
