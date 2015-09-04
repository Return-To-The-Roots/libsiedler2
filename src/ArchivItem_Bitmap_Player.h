// $Id: ArchivItem_Bitmap_Player.h 9359 2014-04-25 15:37:22Z FloSoft $
//
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

#include "ArchivItem_Bitmap.h"
#include "ArchivItem_Palette.h"
#include <vector>

namespace libsiedler2
{
    /// Basisklasse für Player-Bitmaps.
    class baseArchivItem_Bitmap_Player : public virtual baseArchivItem_Bitmap
    {
        public:
            /// Konstruktor von @p baseArchivItem_Bitmap_Player.
            baseArchivItem_Bitmap_Player(void);

            /// Konstruktor von @p baseArchivItem_Bitmap_Player mit Laden der Bilddaten aus einer Datei.
            baseArchivItem_Bitmap_Player(FILE* file, const ArchivItem_Palette* palette);

            /// Destruktor von @p baseArchivItem_Bitmap_Player.
            ~baseArchivItem_Bitmap_Player(void);

            /// lädt die Bilddaten aus einer Datei.
            int load(FILE* file, const ArchivItem_Palette* palette);

            /// lädt die Bilddaten aus einem Puffer.
            int load(unsigned short width, unsigned short height, const unsigned char* image, const unsigned short* starts, bool absolute, unsigned int length, const ArchivItem_Palette* palette);

            /// schreibt die Bilddaten in eine Datei.
            int write(FILE* file, const ArchivItem_Palette* palette) const;

            /// alloziert Bildspeicher für die gewünschte Größe.
            void tex_alloc(void);

            /// räumt den Bildspeicher auf.
            void tex_clear(void);

            void getVisibleArea(int& vx, int& vy, int& vw, int& vh);

            /// schreibt das Bitmap inkl. festgelegter Spielerfarbe in einen Puffer.
            int print(unsigned char* buffer,
                      unsigned short buffer_width,
                      unsigned short buffer_height,
                      TEXTURFORMAT buffer_format,
                      const ArchivItem_Palette* palette,
                      unsigned char color,
                      unsigned short to_x = 0,
                      unsigned short to_y = 0,
                      unsigned short from_x = 0,
                      unsigned short from_y = 0,
                      unsigned short from_w = 0,
                      unsigned short from_h = 0,
                      bool only_player = false) const;

            /// erzeugt ein Bitmap inkl. festgelegter Spielerfarbe aus einem Puffer.
            int create(unsigned short width,
                       unsigned short height,
                       const unsigned char* buffer,
                       unsigned short buffer_width,
                       unsigned short buffer_height,
                       TEXTURFORMAT buffer_format,
                       const ArchivItem_Palette* palette,
                       unsigned char color);

        protected:
            std::vector<unsigned char> tex_pdata; ///< Die Spielerfarbedaten.
    };

    /// Klasse für Player-Bitmaps.
    class ArchivItem_Bitmap_Player : public baseArchivItem_Bitmap_Player, public ArchivItem_Bitmap
    {
        public:
            /// Konstruktor von @p ArchivItem_Bitmap_Player.
            ArchivItem_Bitmap_Player(void) : baseArchivItem_Bitmap(), baseArchivItem_Bitmap_Player() {}

            /// Kopierkonstruktor von @p ArchivItem_Bitmap_Player.
            ArchivItem_Bitmap_Player(const ArchivItem_Bitmap_Player& item) : baseArchivItem_Bitmap(item), baseArchivItem_Bitmap_Player(item) {}
    };
}

#endif // !ARCHIVITEM_BITMAP_PLAYER_H_INCLUDED
