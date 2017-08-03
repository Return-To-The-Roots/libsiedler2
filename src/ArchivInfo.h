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
#ifndef ARCHIVINFO_H_INCLUDED
#define ARCHIVINFO_H_INCLUDED

#pragma once

#include "ArchivItem.h"
#include <vector>
#include <cstddef>
#include <string>
#include <stdint.h>

namespace libsiedler2
{
    /// Klasse für Archivdateien.
    class ArchivInfo
    {
        public:
            ArchivInfo();

            ArchivInfo(const ArchivInfo& info);

            /// Destruktor von @p ArchivInfo, räumt automatisch auf.
            virtual ~ArchivInfo();

            /// erstellt den Datensatz in einer bestimmten Größe.
            void alloc(size_t count);

            /**
             *  vergrößert den Datensatz um eine bestimmten Größe.
             *
             *  @param[in] increment Elementanzahl um den der Datensatz vergrößert werden soll
            */
            void alloc_inc(size_t increment);

            /// gibt die angelegten Daten wieder frei.
            void clear();

            /// Setzt den Inhalt eines ArchivItems auf das des Übergebenen.
            /// Transfers ownership!
            void set(size_t index, ArchivItem* item);

            /// kopiert den Inhalt eines ArchivItems auf das des Übergebenen.
            void setC(size_t index, const ArchivItem& item);

            /// Deletes the item at the given index
            void clearItem(size_t index);

            /// Adds an element to the end. Transfers ownership!
            void push(ArchivItem* item)
            {
                data.push_back(item);
            }

            /// fügt ein Element hinten an und kopiert die Daten von @p item.
            void pushC(const ArchivItem& item);

            /// liefert den Inhalt eines ArchivItems am entsprechenden Index.
            ArchivItem* get(size_t index)
            {
                return (index < size()) ? data[index] : NULL;
            }

            /// liefert den Inhalt eines ArchivItems am entsprechenden Index.
            const ArchivItem* get(size_t index) const
            {
                return (index < size()) ? data[index] : NULL;
            }

            /// liefert das erste Item mit entsprechenden Namen
            ArchivItem* find(const std::string& name);

            /// liefert das erste Item mit entsprechenden Namen
            const ArchivItem* find(const std::string& name) const;

            /// liefert die Größe des Archivs.
            size_t size() const
            {
                return data.size();
            }

            bool empty() const
            {
                return data.empty();
            }

            const ArchivItem* operator[](size_t index) const
            {
                return get(index);
            }

            ArchivItem* operator[](size_t index)
            {
                return get(index);
            }

            ArchivInfo& operator=(const ArchivInfo& info);

        private:
            std::vector<ArchivItem*> data;   /// die Elemente.
    };
}

#endif // ARCHIVINFO_H_INCLUDED
