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

libsiedler2::ArchivItem_Text::ArchivItem_Text() : ArchivItem(BobType::Text) {}

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
        text.resize(length);
        if(!file.read(&text.front(), length))
            return ErrorCode::UNEXPECTED_EOF;
    } else
    {
        // Read all that is there
        text.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
    }

    // Remove nullptr terminator if present
    if(!text.empty() && text.back() == '\0')
        text.pop_back();

    if(conversion)
    {
        /// TODO: Hmm nur temporärer Fix! ist dieses doofe Escape-zeichen am Ende der Files
        if(!text.empty() && text.back() == 26)
            text.pop_back();
    } else
    {
        // Replace all \r\n or \r by \n
        size_t insertPos = 0;
        for(size_t i = 0; i < text.size(); ++i, ++insertPos)
        {
            if(text[i] == '\r')
            {
                text[insertPos] = '\n';
                if(i + 1u < text.size() && text[i + 1u] == '\n')
                    ++i;
            } else
                text[insertPos] = text[i];
        }
        text.resize(insertPos);
    }

    text_.assign(text.begin(), text.end());
    if(text_.empty())
        return ErrorCode::NONE;

    if(conversion)
    {
        text_ = OemToAnsi(text_);

        // Replace all @@ by \n
        size_t insertPos = 0;
        for(size_t i = 0; i + 1u < text_.size(); ++i, ++insertPos)
        {
            if(text_[i] == '@' && text_[i + 1u] == '@')
            {
                text_[insertPos] = '\n';
                ++i;
            } else
                text_[insertPos] = text_[i];
        }
        text_[insertPos++] = text_.back(); // Trailing char
        text_.resize(insertPos);
    }

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

std::string libsiedler2::ArchivItem_Text::getFileText(bool conversion) const
{
    if(!conversion)
        return text_;

    assert(text_.size() < std::numeric_limits<uint32_t>::max());
    const auto length = static_cast<uint32_t>(text_.size());
    std::vector<char> textBuf;
    textBuf.reserve(length);

    for(char c : text_)
    {
        if(c == '\n')
        {
            textBuf.push_back('@');
            textBuf.push_back('@');
        } else if(c != '\r')
            textBuf.push_back(c);
    }

    return AnsiToOem(std::string(textBuf.begin(), textBuf.end()));
}

/**
 *  setzt den Text.
 *
 *  @param[in] text       Der Text der gesetzt werden soll, falls @p nullptr, wird
 *                        evtl vorhandener Text gelöscht
 */
void libsiedler2::ArchivItem_Text::setText(const std::string& text)
{
    this->text_ = text;
}
