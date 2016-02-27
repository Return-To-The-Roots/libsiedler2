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
#ifndef ARCHIVITEM_BITMAP_H_INCLUDED
#define ARCHIVITEM_BITMAP_H_INCLUDED

#pragma once

#include "ArchivItem_BitmapBase.h"

namespace libsiedler2
{
    /**
     * Base class for regular bitmaps (single bitmaps, no player colored ones)
     */
    class baseArchivItem_Bitmap : public virtual ArchivItem_BitmapBase
    {
        public:

            /// schreibt das Bitmap in einen Puffer.
            int print(unsigned char* buffer,
                      unsigned short buffer_width,
                      unsigned short buffer_height,
                      TEXTURFORMAT buffer_format,
                      const ArchivItem_Palette* palette,
                      unsigned short to_x = 0,
                      unsigned short to_y = 0,
                      unsigned short from_x = 0,
                      unsigned short from_y = 0,
                      unsigned short from_w = 0,
                      unsigned short from_h = 0) const;

            /// erzeugt ein Bitmap aus einem Puffer.
            int create(unsigned short width,
                       unsigned short height,
                       const unsigned char* buffer,
                       unsigned short buffer_width,
                       unsigned short buffer_height,
                       TEXTURFORMAT buffer_format,
                       const ArchivItem_Palette* palette);

    };

    /// Basisklasse für Bitmapitems.
    class ArchivItem_Bitmap : public virtual baseArchivItem_Bitmap
    {
    };
}

#endif // !ARCHIVITEM_BITMAP_H_INCLUDED
