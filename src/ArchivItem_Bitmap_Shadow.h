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
#ifndef ARCHIVITEM_BITMAP_SHADOW_H_INCLUDED
#define ARCHIVITEM_BITMAP_SHADOW_H_INCLUDED

#pragma once

#include "ArchivItem_Bitmap.h"
#include "ArchivItem_BitmapBase.h"

#include <iosfwd>

namespace libsiedler2 {
class ArchivItem_Palette;
}

namespace libsiedler2 {
/// Basisklasse für Shadow-Bitmaps.
class baseArchivItem_Bitmap_Shadow : public virtual baseArchivItem_Bitmap
{
public:
    baseArchivItem_Bitmap_Shadow();

    baseArchivItem_Bitmap_Shadow(const baseArchivItem_Bitmap_Shadow& item);

    ~baseArchivItem_Bitmap_Shadow() override;

    /// lädt die Bilddaten aus einer Datei.
    int load(std::istream& file, const ArchivItem_Palette* palette) override;

    /// schreibt die Bilddaten in eine Datei.
    int write(std::ostream& file, const ArchivItem_Palette* palette) const override;
};

/// Klasse für Shadow-Bitmaps.
class ArchivItem_Bitmap_Shadow : public baseArchivItem_Bitmap_Shadow, public ArchivItem_Bitmap
{
public:
    ArchivItem_Bitmap_Shadow() {}

    ArchivItem_Bitmap_Shadow(const ArchivItem_Bitmap_Shadow& item)
        : ArchivItem_BitmapBase(item), baseArchivItem_Bitmap(item), baseArchivItem_Bitmap_Shadow(item)
    {
    }
    RTTR_CLONEABLE(ArchivItem_Bitmap_Shadow)
};
} // namespace libsiedler2

#endif // !ARCHIVITEM_BITMAP_SHADOW_H_INCLUDED
