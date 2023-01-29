// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "ArchivItem_Ini.h"
#include "ArchivItem_Text.h"
#include "ErrorCodes.h"
#include "IAllocator.h"
#include "libsiedler2.h"
#include "s25util/StringConversion.h"

namespace libsiedler2 {

/** @class ArchivItem_Ini
 *
 *  Klasse für INI-Dateien (genauer gesagt eine Sektion).
 */

ArchivItem_Ini::ArchivItem_Ini() : ArchivItem(BobType::Ini) {}

ArchivItem_Ini::ArchivItem_Ini(const std::string& name) : ArchivItem(BobType::Ini)
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
int ArchivItem_Ini::load(std::istream& file)
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

        if(name.empty())
            continue;

        setValue(name, value);
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
int ArchivItem_Ini::write(std::ostream& file) const
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

void ArchivItem_Ini::addValue(const std::string& name, const std::string& value)
{
    auto item = getAllocator().create<ArchivItem_Text>(BobType::Text);
    item->setText(value); //-V522
    item->setName(name);

    push(std::move(item));
}

ArchivItem_Text* ArchivItem_Ini::getItem(const std::string& name)
{
    return dynamic_cast<ArchivItem_Text*>(find(name));
}
const ArchivItem_Text* ArchivItem_Ini::getItem(const std::string& name, bool enforce_exists) const
{
    const auto* item = dynamic_cast<const ArchivItem_Text*>(find(name));
    if(!item && enforce_exists)
        throw std::runtime_error("Missing INI key '" + name + "'");
    return item;
}

std::string ArchivItem_Ini::getValue(const std::string& name) const
{
    return getItem(name, true)->getText();
}

int ArchivItem_Ini::getIntValue(const std::string& name) const
{
    return s25util::fromStringClassic<int>(getValue(name));
}

bool ArchivItem_Ini::getBoolValue(const std::string& name) const
{
    return s25util::fromStringClassic<int>(getValue(name)) != 0;
}

std::string ArchivItem_Ini::getValue(const std::string& name, const std::string& defaultVal) const
{
    const auto* item = getItem(name);
    return item ? item->getText() : defaultVal;
}

int ArchivItem_Ini::getValue(const std::string& name, int defaultVal) const
{
    const auto* item = getItem(name);
    return item ? s25util::fromStringClassic<int>(item->getText()) : defaultVal;
}

int ArchivItem_Ini::getValue(const std::string& name, bool defaultVal) const
{
    const auto* item = getItem(name);
    return item ? (s25util::fromStringClassic<int>(item->getText()) != 0) : defaultVal;
}

void ArchivItem_Ini::setValue(const std::string& name, const std::string& value)
{
    auto* item = getItem(name);
    if(item)
        item->setText(value);
    else // Not found so add
        addValue(name, value);
}

void ArchivItem_Ini::setValue(const std::string& name, int value)
{
    std::string temp = s25util::toStringClassic(value);
    setValue(name, temp);
}
} // namespace libsiedler2