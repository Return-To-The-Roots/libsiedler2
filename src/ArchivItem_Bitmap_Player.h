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
#ifndef ARCHIVITEM_BITMAP_PLAYER_H_INCLUDED
#define ARCHIVITEM_BITMAP_PLAYER_H_INCLUDED

#pragma once

#include "ArchivItem_BitmapBase.h"
#include "enumTypes.h"
#include "PixelBufferPaletted.h"
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
            BOOST_STATIC_CONSTEXPR uint8_t numPlayerClrs = 4;

            ArchivItem_Bitmap_Player();

            ArchivItem_Bitmap_Player(const ArchivItem_Bitmap_Player& other): ArchivItem_BitmapBase(other), tex_pdata(other.tex_pdata) {}

            ~ArchivItem_Bitmap_Player() override;

            /// lädt die Bilddaten aus einer Datei.
            int load(std::istream& file, const ArchivItem_Palette* palette) override;

            /// lädt die Bilddaten aus einem Puffer.
            int load(uint16_t width, uint16_t height, const std::vector<uint8_t>& image, const std::vector<uint16_t>& starts, bool absoluteStarts, const ArchivItem_Palette* palette);

            /// schreibt die Bilddaten in eine Datei.
            int write(std::ostream& file, const ArchivItem_Palette* palette) const override;

            /// Creates a new texture and initializes it to transparent
            void init(int16_t width, int16_t height, TextureFormat format) override;
            using ArchivItem_BitmapBase::init;

            /// räumt den Bildspeicher auf.
            void clear() override;

            void getVisibleArea(int& vx, int& vy, unsigned& vw, unsigned& vh) override;

            /// schreibt das Bitmap inkl. festgelegter Spielerfarbe in einen Puffer.
            int print(uint8_t* buffer,
                      uint16_t buffer_width,
                      uint16_t buffer_height,
                      TextureFormat buffer_format,
                      const ArchivItem_Palette* palette = NULL,
                      uint8_t plClrStartIdx = 128,
                      uint16_t to_x = 0,
                      uint16_t to_y = 0,
                      uint16_t from_x = 0,
                      uint16_t from_y = 0,
                      uint16_t from_w = 0,
                      uint16_t from_h = 0,
                      bool only_player = false) const;

            /// Create a bitmap with player colors.
            /// All colors with palette index in [plClrStartIdx, plClrStartIdx + numPlayerClrs) are considered to be player colors
            int create(uint16_t width,
                       uint16_t height,
                       const uint8_t* buffer,
                       uint16_t buffer_width,
                       uint16_t buffer_height,
                       TextureFormat buffer_format,
                       const ArchivItem_Palette* palette,
                       uint8_t plClrStartIdx = 128);

        protected:
            PixelBufferPaletted tex_pdata; /// Die Spielerfarbedaten.
    };

}

#endif // !ARCHIVITEM_BITMAP_PLAYER_H_INCLUDED
