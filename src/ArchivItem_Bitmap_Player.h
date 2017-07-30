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
#ifndef ARCHIVITEM_BITMAP_PLAYER_H_INCLUDED
#define ARCHIVITEM_BITMAP_PLAYER_H_INCLUDED

#pragma once

#include "ArchivItem_BitmapBase.h"
#include "enumTypes.h"

#include <vector>
#include <iosfwd>
#include <stdint.h>

namespace libsiedler2 { class ArchivItem_Palette; }

namespace libsiedler2
{
    /// Basisklasse für Player-Bitmaps.
    class ArchivItem_Bitmap_Player : public virtual ArchivItem_BitmapBase
    {
        public:
            ArchivItem_Bitmap_Player();

            ArchivItem_Bitmap_Player(const ArchivItem_Bitmap_Player& other): ArchivItem_BitmapBase(other), tex_pdata(other.tex_pdata) {}

            /// Konstruktor von @p baseArchivItem_Bitmap_Player mit Laden der Bilddaten aus einer Datei.
            ArchivItem_Bitmap_Player(std::istream& file, const ArchivItem_Palette* palette);

            ~ArchivItem_Bitmap_Player() override;

            ArchivItem_Bitmap_Player& operator=(const ArchivItem_Bitmap_Player& other);

            /// lädt die Bilddaten aus einer Datei.
            int load(std::istream& file, const ArchivItem_Palette* palette) override;

            /// lädt die Bilddaten aus einem Puffer.
            int load(uint16_t width, uint16_t height, const std::vector<uint8_t>& image, const std::vector<uint16_t>& starts, bool absoluteStarts, const ArchivItem_Palette* palette);

            /// schreibt die Bilddaten in eine Datei.
            int write(std::ostream& file, const ArchivItem_Palette* palette) const override;

            /// alloziert Bildspeicher für die gewünschte Größe.
            void tex_alloc(int16_t width, int16_t height, TexturFormat format) override;

            /// räumt den Bildspeicher auf.
            void tex_clear() override;

            void getVisibleArea(int& vx, int& vy, int& vw, int& vh) override;

            /// schreibt das Bitmap inkl. festgelegter Spielerfarbe in einen Puffer.
            int print(uint8_t* buffer,
                      uint16_t buffer_width,
                      uint16_t buffer_height,
                      TexturFormat buffer_format,
                      const ArchivItem_Palette* palette = NULL,
                      uint8_t color = 128,
                      uint16_t to_x = 0,
                      uint16_t to_y = 0,
                      uint16_t from_x = 0,
                      uint16_t from_y = 0,
                      uint16_t from_w = 0,
                      uint16_t from_h = 0,
                      bool only_player = false) const;

            /// erzeugt ein Bitmap inkl. festgelegter Spielerfarbe aus einem Puffer.
            int create(uint16_t width,
                       uint16_t height,
                       const uint8_t* buffer,
                       uint16_t buffer_width,
                       uint16_t buffer_height,
                       TexturFormat buffer_format,
                       const ArchivItem_Palette* palette,
                       uint8_t color = 128);

        protected:
            std::vector<uint8_t> tex_pdata; /// Die Spielerfarbedaten.
    };

}

#endif // !ARCHIVITEM_BITMAP_PLAYER_H_INCLUDED
