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

#include "ArchivItem_Ini.h"
#include "ArchivItem_Text.h"
#include "ErrorCodes.h"
#include "IAllocator.h"
#include "libsiedler2.h"
#include "s25util/StringConversion.h"

/** @class libsiedler2::ArchivItem_Ini
 *
 *  Klasse für INI-Dateien (genauer gesagt eine Sektion).
 */

libsiedler2::ArchivItem_Ini::ArchivItem_Ini() : ArchivItem(BOBTYPE_INI) {}

libsiedler2::ArchivItem_Ini::ArchivItem_Ini(const std::string& name) : ArchivItem(BOBTYPE_INI)
{
    setName(name);
}

/**
 *  lädt die INI-Daten aus einer Datei.
 *
 *  @param[in] file    Dateihandle der Datei
 *
 *  @return liefert Null bei Erfolg, ungleich Null bei Fehler
 */
int libsiedler2::ArchivItem_Ini::load(std::istream& file)
{
    if(!file)
        return ErrorCode::FILE_NOT_ACCESSIBLE;

    std::string section;
    if(!std::getline(file, section))
        return file.eof() ? ErrorCode::NONE : ErrorCode::UNEXPECTED_EOF;

    if(!section.empty())
    {
        size_t posStart = section.find('[');
        size_t posEnd = section.find_last_of(']');
        if(posStart == std::string::npos || posEnd == std::string::npos || posStart > posEnd)
            return ErrorCode::WRONG_FORMAT;
        section = section.substr(posStart + 1, posEnd - posStart - 1);
    }
    if(section.empty())
        return ErrorCode::WRONG_HEADER;

    setName(section);

    while(true)
    {
        std::string entry;
        if(!std::getline(file, entry))
            break;
        size_t posNl = entry.find_first_of("\r\n");
        if(posNl != std::string::npos)
            entry.erase(posNl);
        if(entry.empty())
            break;

        size_t pos = entry.find('=');
        if(pos == std::string::npos)
            return ErrorCode::WRONG_FORMAT;
        std::string name = entry.substr(0, pos);
        std::string value = entry.substr(pos + 1);

        if(name.empty() || value.empty())
            continue;

        addValue(name, value);
    }

    return ErrorCode::NONE;
}

/**
 *  schreibt die INI-Daten in eine Datei.
 *
 *  @param[in] file    Dateihandle der Datei
 *
 *  @return liefert Null bei Erfolg, ungleich Null bei Fehler
 */
int libsiedler2::ArchivItem_Ini::write(std::ostream& file) const
{
    if(!file)
        return ErrorCode::FILE_NOT_ACCESSIBLE;

    file << "[" << getName() << "]\r\n";

    for(size_t i = 0; i < size(); ++i)
    {
        const auto* item = dynamic_cast<const ArchivItem_Text*>(get(i));
        file << item->getName() << "=" << item->getText() << "\r\n"; //-V522
    }

    return (!file) ? ErrorCode::UNEXPECTED_EOF : ErrorCode::NONE;
}

/**
 *  fügt einen Eintrag hinzu.
 *
 *  @param[in] name   Variablenname
 *  @param[in] value  Wert
 */
void libsiedler2::ArchivItem_Ini::addValue(const std::string& name, const std::string& value)
{
    auto item = getAllocator().create<ArchivItem_Text>(BOBTYPE_TEXT);
    item->setText(value); //-V522
    item->setName(name);

    push(std::move(item));
}

std::string libsiedler2::ArchivItem_Ini::getValue(const std::string& name) const
{
    const auto* item = dynamic_cast<const ArchivItem_Text*>(find(name));
    if(item)
    {
        return item->getText();
    }
    return "";
}

int libsiedler2::ArchivItem_Ini::getValueI(const std::string& name) const
{
    return s25util::fromStringClassic<int>(getValue(name));
}

void libsiedler2::ArchivItem_Ini::setValue(const std::string& name, const std::string& value)
{
    auto* item = dynamic_cast<ArchivItem_Text*>(find(name));
    if(item)
    {
        // setText überschreibt Namen, daher nochmals setzen
        item->setText(value);
        item->setName(name);
    } else
    {
        // nicht gefunden, also hinzufügen
        addValue(name, value);
    }
}

void libsiedler2::ArchivItem_Ini::setValue(const std::string& name, int value)
{
    std::string temp = s25util::toStringClassic(value);
    setValue(name, temp);
}
