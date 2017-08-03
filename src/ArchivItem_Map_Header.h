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
#ifndef ARCHIVITEM_MAP_HEADER_H_INCLUDED
#define ARCHIVITEM_MAP_HEADER_H_INCLUDED

#pragma once

#include "ArchivItem.h"
#include <string>
#include <iosfwd>
#include <stdint.h>

namespace libsiedler2
{
    /// Klasse für einen Mapheader.
    class ArchivItem_Map_Header : public ArchivItem
    {
        public:
            ArchivItem_Map_Header();
            ~ArchivItem_Map_Header() override;

            /// lädt den Mapheader aus einer Datei.
            int load(std::istream& file);
            /// schreibt den Mapheader in eine Datei.
            int write(std::ostream& file) const;

            const std::string& getName() const;
            void setName(const std::string& name);

            uint16_t getWidth() const;
            void setWidth(uint16_t width);

            uint16_t getHeight() const;
            void setHeight(uint16_t height);

            uint8_t getGfxSet() const;
            void setGfxSet(uint8_t gfxset);

            uint8_t getPlayer() const;
            void setPlayer(uint8_t player);

            const std::string& getAuthor() const;
            void setAuthor(const std::string& author);
        
            void setPlayerHQ(const uint32_t player, const uint16_t x, const uint16_t y);
            void getPlayerHQ(const uint32_t player, uint16_t& x, uint16_t& y);
        
            bool hasExtraWord() const { return hasExtraWord_; }

        private:
            std::string name_;
            uint16_t width;
            uint16_t height;
            uint8_t gfxset;
            uint8_t player;
            std::string author_;
            uint16_t playerHQx[7], playerHQy[7];
            uint8_t isInvalid;
            uint8_t playerFaces[7];
            uint8_t areaInfos[250 * 9]; // 250 entries with: uint8 Type(0:Unused, 1:water, 2:land), uint16 x,y, uint32 size
            bool hasExtraWord_;
    };
}

#endif // !ARCHIVITEM_MAP_HEADER_H_INCLUDED
