// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

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
    ArchivItem_Bitmap_Shadow() = default;

    ArchivItem_Bitmap_Shadow(const ArchivItem_Bitmap_Shadow& item)
        : ArchivItem_BitmapBase(item), baseArchivItem_Bitmap(item), baseArchivItem_Bitmap_Shadow(item),
          ArchivItem_Bitmap(item)
    {}
    RTTR_CLONEABLE(ArchivItem_Bitmap_Shadow)
};
} // namespace libsiedler2
