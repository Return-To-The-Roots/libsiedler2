// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "Archiv.h"
#include "ArchivItem.h"
#include <stdexcept>

namespace libsiedler2 {
/** @class Archiv
 *
 *  Klasse für Archivdateien.
 */

/** @var Archiv::data
 *
 *  die Elemente.
 */

Archiv::Archiv() = default;
Archiv::Archiv(Archiv&&) noexcept = default;
Archiv& Archiv::operator=(Archiv&&) noexcept = default;

Archiv::Archiv(const Archiv& info)
{
    data.reserve(info.size());
    for(const auto& it : info.data)
    {
        if(it)
            pushC(*it);
        else
            push(nullptr);
    }
}

Archiv& Archiv::operator=(const Archiv& info)
{
    if(this == &info)
        return *this;
    clear();
    data.reserve(info.size());
    for(const auto& it : info.data)
    {
        if(it)
            pushC(*it);
        else
            push(nullptr);
    }
    return *this;
}

/**
 *  Destruktor von @p Archiv, räumt automatisch auf.
 */
Archiv::~Archiv() = default;

/**
 *  erstellt den Datensatz in einer bestimmten Größe.
 *
 *  @param[in] count Elementanzahl auf den der Datensatz angelegt werden soll
 */
void Archiv::alloc(size_t count)
{
    clear();
    data.resize(count);
}

void Archiv::alloc_inc(size_t increment)
{
    data.resize(size() + increment);
}

/**
 *  gibt die angelegten Daten wieder frei.
 */
void Archiv::clear()
{
    data.clear();
}

/**
 *  Setzt den Inhalt eines ArchivItems auf das des Übergebenen.
 *
 *  @param[in] index Index des zu setzenden Eintrags
 *  @param[in] item  Item mit dem zu setzenden Inhalt
 */

void Archiv::set(size_t index, std::unique_ptr<ArchivItem> item)
{
    if(index >= size())
        throw std::out_of_range("Index out of range");
    data[index] = std::move(item);
}

/**
 *  kopiert den Inhalt eines ArchivItems auf das des Übergebenen.
 *
 *  @param[in] index Index des zu setzenden Eintrags
 *  @param[in] item  Item mit dem zu kopierenden Inhalt
 */
void Archiv::setC(size_t index, const ArchivItem& item)
{
    set(index, clone(item));
}

void Archiv::push(std::unique_ptr<ArchivItem> item)
{
    data.emplace_back(std::move(item));
}

/**
 *  fügt ein Element hinten an und kopiert die Daten von @p item.
 *
 *  @param[in] item Item mit dem zu kopierenden Inhalt
 */
void Archiv::pushC(const ArchivItem& item)
{
    data.emplace_back(clone(item));
}

const ArchivItem* Archiv::find(const std::string& name) const
{
    for(const auto& it : data)
    {
        if(it && it->getName() == name)
            return it.get();
    }

    return nullptr;
}

ArchivItem* Archiv::find(const std::string& name)
{
    for(auto& it : data)
    {
        if(it && it->getName() == name)
            return it.get();
    }

    return nullptr;
}

std::unique_ptr<ArchivItem> Archiv::release(size_t index)
{
    if(index >= size())
        return nullptr;
    return std::move(data[index]);
}

} // namespace libsiedler2
