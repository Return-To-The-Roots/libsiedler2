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
#ifndef ARCHIVITEM_MAP_HEADER_H_INCLUDED
#define ARCHIVITEM_MAP_HEADER_H_INCLUDED

#pragma once

#include "ArchivItem.h"
#include <string>

namespace libsiedler2
{
    /// Klasse für einen Mapheader.
    class ArchivItem_Map_Header : public ArchivItem
    {
        public:
            /// Konstruktor von @p ArchivItem_Map_Header.
            ArchivItem_Map_Header(void);
            /// Destruktor von @p ArchivItem_Map_Header.
            ~ArchivItem_Map_Header(void);

            /// lädt den Mapheader aus einer Datei.
            int load(std::istream& file);
            /// schreibt den Mapheader in eine Datei.
            int write(std::ostream& file) const;

            const std::string& getName(void) const;
            void setName(const std::string& name);

            unsigned short getWidth(void) const;
            void setWidth(unsigned short width);

            unsigned short getHeight(void) const;
            void setHeight(unsigned short height);

            unsigned char getGfxSet(void) const;
            void setGfxSet(unsigned char gfxset);

            unsigned char getPlayer(void) const;
            void setPlayer(unsigned char player);

            const std::string& getAuthor(void) const;
            void setAuthor(const std::string& author);

        private:
            std::string name_;
            unsigned short width;
            unsigned short height;
            unsigned char gfxset;
            unsigned char player;
            std::string author_;
    };
}

#endif // !ARCHIVITEM_MAP_HEADER_H_INCLUDED
