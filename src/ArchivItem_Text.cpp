// Copyright (c) 2005 - 2015 Settlers Freaks (sf-team at siedler25.org)
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

///////////////////////////////////////////////////////////////////////////////
// Header
#include "main.h" // IWYU pragma: keep
#include "ArchivItem_Text.h"
#include "oem.h"
#include <fstream>
#include <vector>
#include <limits>

// Include last!
#include "DebugNew.h" // IWYU pragma: keep

///////////////////////////////////////////////////////////////////////////////
/** @class libsiedler2::ArchivItem_Text
 *
 *  Klasse für Texte.
 *
 *  @author FloSoft
 */

///////////////////////////////////////////////////////////////////////////////
/** @var libsiedler2::ArchivItem_Text::text
 *
 *  Der Textspeicher.
 *
 *  @author FloSoft
 */

///////////////////////////////////////////////////////////////////////////////
/** @var libsiedler2::ArchivItem_Text::length
 *
 *  Die Länge des Textes.
 *
 *  @author FloSoft
 */

///////////////////////////////////////////////////////////////////////////////
/**
 *  Konstruktor von @p ArchivItem_Text.
 *
 *  @author FloSoft
 */
libsiedler2::ArchivItem_Text::ArchivItem_Text() : ArchivItem()
{
    setBobType(BOBTYPE_TEXT);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Destruktor von @p ArchivItem_Text.
 *
 *  @author FloSoft
 */
libsiedler2::ArchivItem_Text::~ArchivItem_Text()
{}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Konstruktor von @p ArchivItem_Text mit Laden des Textes aus
 *  einer Datei.
 *
 *  @param[in] file       Dateihandle aus denen die Farbwerte geladen werden sollen
 *  @param[in] conversion Soll ggf. OEM-Charset in ANSI umgewandelt werden?
 *  @param[in] length     Länge des Blocks (Wieviel Bytes sollen eingelesen werden?)
 *
 *  @author FloSoft
 */
libsiedler2::ArchivItem_Text::ArchivItem_Text(std::istream& file, bool conversion, unsigned int length) : ArchivItem()
{
    setBobType(BOBTYPE_TEXT);

    load(file, conversion, length);
}

///////////////////////////////////////////////////////////////////////////////
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
 *
 *  @author FloSoft
 */
int libsiedler2::ArchivItem_Text::load(std::istream& file, bool conversion, unsigned int length)
{
    if(!file)
        return 1;

    std::vector<char> text;
    if(length)
    {
        text.resize(length + 1);
        if(!file.read(&text.front(), length))
            return 2;
    }else
    {
        // Read all that is there
        text.assign(
            std::istreambuf_iterator<char>(file), 
            std::istreambuf_iterator<char>());
        text.push_back(0);
    }

    /// TODO: Hmm nur temporärer Fix! ist dieses doofe Escape-zeichen am Ende der Files
    if(text[length - 1] == 26)
        text[length - 1] = 0;

    if(conversion)
        OemToAnsi(&text.front(), &text.front()); //-V742

    for(unsigned int i = 0; i < length; ++i)
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
    return 0;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  schreibt dem Text in eine Datei.
 *
 *  @param[in] file       Dateihandle in die der Text geschrieben werden soll
 *  @param[in] conversion Soll ggf. ANSI-Charset in OEM umgewandelt werden?
 *
 *  @return liefert Null bei Erfolg, ungleich Null bei Fehler
 *
 *  @author FloSoft
 */
int libsiedler2::ArchivItem_Text::write(std::ostream& file, bool conversion) const
{
    if(!file)
        return 1;

    // Wenn Länge 0, nix schreiben, ist ja kein Fehler!
    if(text_.empty())
        return 0;

    assert(text_.size() < std::numeric_limits<unsigned>::max());
    unsigned int length = static_cast<unsigned>(text_.size());
    std::vector<char> text(length * 2 + 1);


    for(unsigned int i = 0, j = 0; i < length; ++i)
    {
        if(this->text_[i] == '\n')
        {
            text[j++] = '@';
            text[j++] = '@';
        }
        else if(this->text_[i] == '\r')
            continue;
        else
            text[j++] = this->text_[i];
    }

    //memcpy(text, this->text, length);

    // ggf umwandeln
    if(conversion)
        AnsiToOem(&text.front(), &text.front()); //-V742

    // Schreiben
    if(!file.write(&text.front(), length + 1))
        return 2;

    return 0;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  liefert den Text.
 *
 *  @return liefert einen konstanten Zeiger auf das Textelement, NULL bei leerem Text
 *
 *  @author FloSoft
 */
const std::string& libsiedler2::ArchivItem_Text::getText() const
{
    return text_;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  setzt den Text.
 *
 *  @param[in] text       Der Text der gesetzt werden soll, falls @p NULL, wird
 *                        evtl vorhandener Text gelöscht
 *  @param[in] conversion Soll ggf. ANSI-Charset in OEM umgewandelt werden?
 *  @param[in] length     Länge des Textes, bei @p 0 wird @p strlen verwendet
 *
 *  @author FloSoft
 */
void libsiedler2::ArchivItem_Text::setText(const std::string& text, bool conversion)
{
    if(conversion){
        std::vector<char> tmp(text.size() + 1);
        AnsiToOem(text.data(), &tmp.front()); //-V742
        this->text_ = &tmp.front();
    }else
        this->text_ = text;

    // Name setzen
    if(getName().empty())
        setName(this->text_);
}
