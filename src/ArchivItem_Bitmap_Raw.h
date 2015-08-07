// $Id: ArchivItem_Bitmap_Raw.h 9359 2014-04-25 15:37:22Z FloSoft $
//
// Copyright (c) 2005 - 2011 Settlers Freaks (sf-team at siedler25.org)
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
#ifndef ARCHIVITEM_BITMAP_RAW_H_INCLUDED
#define ARCHIVITEM_BITMAP_RAW_H_INCLUDED

#pragma once

#include "ArchivItem_Bitmap.h"

namespace libsiedler2
{
    /// Basisklasse für Raw-Bitmaps.
    class baseArchivItem_Bitmap_Raw : public virtual baseArchivItem_Bitmap
    {
        public:
            /// Konstruktor von @p baseArchivItem_Bitmap_Raw.
            baseArchivItem_Bitmap_Raw(void);

            /// Kopierkonstruktor von @p baseArchivItem_Bitmap_Raw.
            baseArchivItem_Bitmap_Raw(const baseArchivItem_Bitmap_Raw* item);

            /// Konstruktor von @p baseArchivItem_Bitmap_Raw mit Laden der Bilddaten aus einer Datei.
            baseArchivItem_Bitmap_Raw(FILE* file, const ArchivItem_Palette* palette);

            /// Destruktor von @p baseArchivItem_Bitmap_Raw.
            ~baseArchivItem_Bitmap_Raw(void);

            /// lädt die Bilddaten aus einer Datei.
            int load(FILE* file, const ArchivItem_Palette* palette);

            /// schreibt die Bilddaten in eine Datei.
            int write(FILE* file, const ArchivItem_Palette* palette) const;
    };

    /// Klasse für Raw-Bitmaps.
    class ArchivItem_Bitmap_Raw : public baseArchivItem_Bitmap_Raw, public ArchivItem_Bitmap
    {
        public:
            /// Konstruktor von @p ArchivItem_Bitmap_Raw.
            ArchivItem_Bitmap_Raw(void) : baseArchivItem_Bitmap(), baseArchivItem_Bitmap_Raw() {}

            /// Kopierkonstruktor von @p ArchivItem_Bitmap_Raw.
            ArchivItem_Bitmap_Raw(const ArchivItem_Bitmap_Raw* item) : baseArchivItem_Bitmap(item), baseArchivItem_Bitmap_Raw(item) {}
    };
}

#endif // !ARCHIVITEM_BITMAP_RAW_H_INCLUDED
