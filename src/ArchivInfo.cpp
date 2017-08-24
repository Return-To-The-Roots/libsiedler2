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
#include "ArchivInfo.h"
#include "libsiedler2.h"
#include "IAllocator.h"
#include <stdexcept>

namespace libsiedler2{
/** @class ArchivInfo
 *
 *  Klasse für Archivdateien.
 */


/** @var ArchivInfo::data
 *
 *  die Elemente.
 */

ArchivInfo::ArchivInfo()
{
}

ArchivInfo::ArchivInfo(const ArchivInfo& info)
{
    data.reserve(info.size());
    for(std::vector<ArchivItem*>::const_iterator it = info.data.begin(); it != info.data.end(); ++it)
    {
        if(*it)
            pushC(**it);
        else
            push(NULL);
    }
}

ArchivInfo& ArchivInfo::operator=(const ArchivInfo& info){
    if(this == &info)
        return *this;
    clear();
    data.reserve(info.size());
    for(std::vector<ArchivItem*>::const_iterator it = info.data.begin(); it != info.data.end(); ++it)
    {
        if(*it)
            pushC(**it);
        else
            push(NULL);
    }
    return *this;
}

/**
 *  Destruktor von @p ArchivInfo, räumt automatisch auf.
 */
ArchivInfo::~ArchivInfo()
{
    clear();
}

/**
 *  erstellt den Datensatz in einer bestimmten Größe.
 *
 *  @param[in] count Elementanzahl auf den der Datensatz angelegt werden soll
 */
void ArchivInfo::alloc(size_t count)
{
    clear();

    data.resize(count);
}

void ArchivInfo::alloc_inc(size_t increment)
{
    data.resize(size() + increment);
}

/**
 *  gibt die angelegten Daten wieder frei.
 */
void ArchivInfo::clear()
{
    
    for(std::vector<ArchivItem*>::iterator it = data.begin(); it != data.end(); ++it)
        delete *it;
    data.clear();
}

/**
 *  Setzt den Inhalt eines ArchivItems auf das des Übergebenen.
 *
 *  @param[in] index Index des zu setzenden Eintrags
 *  @param[in] item  Item mit dem zu setzenden Inhalt
 */

void ArchivInfo::set(size_t index, ArchivItem* item)
{
   if(index >= size())
       throw std::range_error("Index out of range");
   delete data[index];
   data[index] = item;
}

/**
 *  kopiert den Inhalt eines ArchivItems auf das des Übergebenen.
 *
 *  @param[in] index Index des zu setzenden Eintrags
 *  @param[in] item  Item mit dem zu kopierenden Inhalt
 */
void ArchivInfo::setC(size_t index, const ArchivItem& item)
{
    set(index, item.clone());
}

void ArchivInfo::clearItem(size_t index)
{
    set(index, NULL);
}
/**
 *  fügt ein Element hinten an und kopiert die Daten von @p item.
 *
 *  @param[in] item Item mit dem zu kopierenden Inhalt
 */
void ArchivInfo::pushC(const ArchivItem& item)
{
    data.push_back(item.clone());
}

const libsiedler2::ArchivItem* ArchivInfo::find(const std::string& name) const
{
    for(std::vector<ArchivItem*>::const_iterator it = data.begin(); it != data.end(); ++it)
    {
        if(*it && (*it)->getName() == name)
            return *it;
    }

    return NULL;
}

libsiedler2::ArchivItem* ArchivInfo::find(const std::string& name)
{
    for(std::vector<ArchivItem*>::iterator it = data.begin(); it != data.end(); ++it)
    {
        if(*it && (*it)->getName() == name)
            return *it;
    }

    return NULL;
}

}
