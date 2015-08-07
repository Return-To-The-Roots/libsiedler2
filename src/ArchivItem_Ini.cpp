// $Id: ArchivItem_Ini.cpp 9359 2014-04-25 15:37:22Z FloSoft $
//
// Copyright (c) 2005 - 2011 Settlers Freaks (sf-team at siedler25.org)
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
#include "ArchivItem_Ini.h"

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#if defined _WIN32 && defined _DEBUG && defined _MSC_VER
#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////////
/** @class libsiedler2::ArchivItem_Ini
 *
 *  Klasse für INI-Dateien (genauergesagt eine Sektion).
 *
 *  @author FloSoft
 */

///////////////////////////////////////////////////////////////////////////////
/**
 *  Konstruktor von @p ArchivItem_Ini.
 *
 *  @author FloSoft
 */
libsiedler2::ArchivItem_Ini::ArchivItem_Ini(void) : ArchivItem(), ArchivInfo()
{
    setBobType(BOBTYPE_INI);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Konstruktor von @p ArchivItem_Ini.
 *
 *  @author FloSoft
 */
libsiedler2::ArchivItem_Ini::ArchivItem_Ini(const char* name) : ArchivItem(), ArchivInfo()
{
    setName(name);
    setBobType(BOBTYPE_INI);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Kopierkonstruktor von @p ArchivItem_Ini.
 *
 *  @param[in] info Quellitem
 *
 *  @author FloSoft
 */
libsiedler2::ArchivItem_Ini::ArchivItem_Ini(const ArchivItem_Ini* info) : ArchivItem( info ), ArchivInfo( info )
{
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  lädt die INI-Daten aus einer Datei.
 *
 *  @param[in] file    Dateihandle der Datei
 *
 *  @return liefert Null bei Erfolg, ungleich Null bei Fehler
 *
 *  @author FloSoft
 */
int libsiedler2::ArchivItem_Ini::load(FILE* file)
{
    if(file == NULL)
        return 1;

    char temp[512];

    memset(temp, 0, 512);
    if(fgets(temp, 511, file) == NULL && !feof(file))
        return 2;

    std::string section(temp);

    size_t lr = section.find("\r");
    if (lr != std::string::npos) section.erase(lr, 1);
    size_t ln = section.find("\n");
    if (ln != std::string::npos) section.erase(ln, 1);

    section = section.substr(section.find_first_of('[') + 1);
    section = section.substr(0, section.find_first_of(']'));

    if(section.length() == 0 && !feof(file))
        return 4;

    setName(section.c_str());

    if(feof(file))
        return 0;

    while(true)
    {
        memset(temp, 0, 512);
        if(fgets(temp, 511, file) == NULL && !feof(file))
            return 5;

        std::string entry(temp);

        size_t lr = entry.find("\r");
        if (lr != std::string::npos) entry.erase(lr, 1);
        size_t ln = entry.find("\n");
        if (ln != std::string::npos) entry.erase(ln, 1);

        if(entry.length() == 0 || feof(file))
            break;

        std::string name, value;

        name = entry.substr(0, entry.find("="));
        value = entry.substr(entry.find("=") + 1);

        if(name.length() == 0 || value.length() == 0)
            continue;

        ArchivItem_Text* item = dynamic_cast<ArchivItem_Text*>( (*allocator)(BOBTYPE_TEXT, 0, NULL) );
        item->setText(value.c_str());
        item->setName(name.c_str());

        push(item);
    }

    return 0;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  schreibt die INI-Daten in eine Datei.
 *
 *  @param[in] file    Dateihandle der Datei
 *
 *  @return liefert Null bei Erfolg, ungleich Null bei Fehler
 *
 *  @author FloSoft
 */
int libsiedler2::ArchivItem_Ini::write(FILE* file) const
{
    if(file == NULL)
        return 1;

    std::string section = "[" + std::string(getName()) + "]\n";

    if(fputs(section.c_str(), file) < 0)
        return 2;

    for(unsigned long i = 0; i < getCount(); ++i)
    {
        const ArchivItem_Text* item = dynamic_cast<const ArchivItem_Text*>(get(i));

        std::string entry = std::string(item->getName()) + "=" + std::string(item->getText()) + "\n";

        if(fputs(entry.c_str(), file) < 0)
            return 3 + i;
    }

    fputs("\n", file);

    return 0;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  fügt einen Eintrag hinzu.
 *
 *  @param[in] name   Variablenname
 *  @param[in] value  Wert
 *
 *  @author FloSoft
 */
void libsiedler2::ArchivItem_Ini::addValue(const char* name, const char* value)
{
    ArchivItem_Text* item = dynamic_cast<ArchivItem_Text*>( (*allocator)(BOBTYPE_TEXT, 0, NULL) );
    item->setText(value);
    item->setName(name);

    push(item);
}
