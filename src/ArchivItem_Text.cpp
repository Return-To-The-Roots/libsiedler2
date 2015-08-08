// $Id: ArchivItem_Text.cpp 9359 2014-04-25 15:37:22Z FloSoft $
//
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
#include "main.h"
#include "ArchivItem_Text.h"
#include "oem.h"
#include <vector>

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#if defined _WIN32 && defined _DEBUG && defined _MSC_VER
#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

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
libsiedler2::ArchivItem_Text::ArchivItem_Text(FILE* file, bool conversion, unsigned int length) : ArchivItem()
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
int libsiedler2::ArchivItem_Text::load(FILE* file, bool conversion, unsigned int length)
{
    if(file == NULL)
        return 1;

    // Aktuelle Position bestimmen
    long pos = ftell(file);

    if(length == 0)
    {
        // Länge der Datei bestimmen
        fseek(file, 0, SEEK_END);
        length = ftell(file) - pos;
        fseek(file, pos, SEEK_SET);
    }

    std::vector<char>text(length + 1);

    // Einlesen
    if(libendian::le_read_c(&text.front(), length, file) != (int)length)
        return 2;

    /// TODO: Hmm nur temporärer Fix! ist dieses doofe Escape-zeichen am Ende der Files
    if(text[length - 1] == 26)
        text[length - 1] = 0;

    if(conversion)
        OemToAnsi(&text.front(), &text.front());

    for(unsigned int i = 0; i < length; ++i)
    {
        if(text[i] == '@' && text[i + 1] == '@')
        {
            text[i] = '\r';
            text[i + 1] = '\n';
        }
    }

    this->text = &text.front();
    // Text setzen
    setName(this->text);

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
int libsiedler2::ArchivItem_Text::write(FILE* file, bool conversion) const
{
    if(file == NULL)
        return 1;

    // Wenn Länge 0, nix schreiben, ist ja kein Fehler!
    if(text.size() == 0)
        return 0;

    unsigned int length = text.size();
    std::vector<char> text(length * 2 + 1);


    for(unsigned int i = 0, j = 0; i < length; ++i)
    {
        if(this->text[i] == '\n')
        {
            text[j++] = '@';
            text[j++] = '@';
        }
        else if(this->text[i] == '\r')
            continue;
        else
            text[j++] = this->text[i];
    }

    //memcpy(text, this->text, length);

    // ggf umwandeln
    if(conversion)
        AnsiToOem(&text.front(), &text.front());

    // Schreiben
    if(libendian::le_write_c(&text.front(), length + 1, file) != (int)length + 1)
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
const std::string& libsiedler2::ArchivItem_Text::getText(void) const
{
    return text;
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
        AnsiToOem(text.data(), &tmp.front());
        this->text = &tmp.front();
    }else
        this->text = text;

    // Name setzen
    if(getName().empty())
        setName(this->text);
}
