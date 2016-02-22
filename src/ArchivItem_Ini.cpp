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
#include "ArchivItem_Ini.h"
#include "ArchivItem_Text.h"
#include "libsiedler2.h"
#include <boost/lexical_cast.hpp>
#include <stdexcept>
#include <cstddef>
#include <sstream>

// Include last!
#include "DebugNew.h" // IWYU pragma: keep

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
libsiedler2::ArchivItem_Ini::ArchivItem_Ini(const std::string& name) : ArchivItem(), ArchivInfo()
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
libsiedler2::ArchivItem_Ini::ArchivItem_Ini(const ArchivItem_Ini& info) : ArchivItem( info ), ArchivInfo( info )
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
int libsiedler2::ArchivItem_Ini::load(std::istream& file)
{
    if(!file)
        return 1;

    std::string section;
    std::getline(file, section);

    size_t lr = section.find("\r");
    if (lr != std::string::npos) section.erase(lr, 1);
    size_t ln = section.find("\n");
    if (ln != std::string::npos) section.erase(ln, 1);

    if(!section.empty())
    {
        size_t pos = section.find('[');
        if(pos == std::string::npos)
            throw std::runtime_error("Invalid section");
        section = section.substr(pos + 1);
        pos = section.find(']');
        if(pos == std::string::npos)
            throw std::runtime_error("Invalid section");
        section = section.substr(0, pos);
    }
    if(section.empty() && !file.eof())
        return 4;

    setName(section);

    if(!file)
        return 0;

    while(true)
    {
        std::string entry;
        if(!std::getline(file, entry))
            break;
        lr = entry.find("\r");
        if (lr != std::string::npos) entry.erase(lr, 1);
        ln = section.find("\n");
        if (ln != std::string::npos) entry.erase(ln, 1);
        if(entry.empty())
            break;

        size_t pos = entry.find("=");
        if(pos == std::string::npos)
            throw std::runtime_error("Invalid value");
        std::string name = entry.substr(0, pos);
        std::string value = entry.substr(pos + 1);

        if(name.empty() || value.empty())
            continue;

        ArchivItem_Text* item = dynamic_cast<ArchivItem_Text*>( getAllocator().create(BOBTYPE_TEXT) );
        item->setText(value);
        item->setName(name);

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
int libsiedler2::ArchivItem_Ini::write(std::ostream& file) const
{
    if(!file)
        return 1;

    std::string section = "[" + getName() + "]\n";

    file << section;

    for(size_t i = 0; i < size(); ++i)
    {
        const ArchivItem_Text* item = dynamic_cast<const ArchivItem_Text*>(get(i));

        std::string entry = item->getName() + "=" + item->getText() + "\n";

        file << entry;
    }

    file << "\n";

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
void libsiedler2::ArchivItem_Ini::addValue(const std::string& name, const std::string& value)
{
    ArchivItem_Text* item = dynamic_cast<ArchivItem_Text*>( getAllocator().create(BOBTYPE_TEXT) );
    item->setText(value);
    item->setName(name);

    push(item);
}

std::string libsiedler2::ArchivItem_Ini::getValue(const std::string& name) const
{
    const ArchivItem_Text* item = dynamic_cast<const ArchivItem_Text*>(find(name));
    if(item)
    {
        return item->getText();
    }
    return "";
}

void libsiedler2::ArchivItem_Ini::setValue(const std::string& name, const std::string& value)
{
    ArchivItem_Text* item = dynamic_cast<ArchivItem_Text*>(find(name));
    if(item)
    {
        // setText überschreibt Namen, daher nochmals setzen
        item->setText(value);
        item->setName(name);
    }
    else
    {
        // nicht gefunden, also hinzufügen
        addValue(name, value);
    }
}

void libsiedler2::ArchivItem_Ini::setValue(const std::string& name, int value)
{
    std::string temp = boost::lexical_cast<std::string>(value);

    ArchivItem_Text* item = dynamic_cast<ArchivItem_Text*>(find(name));
    if(item)
    {
        // setText überschreibt Namen, daher nochmals setzen
        item->setText(temp);
        item->setName(name);
    }
    else
    {
        // nicht gefunden, also hinzufügen
        addValue(name, temp);
    }
}
