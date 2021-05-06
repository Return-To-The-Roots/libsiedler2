// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "ArchivItem_Bitmap.h"

namespace libsiedler2 {
class ArchivItem_Palette;

/// Basisklasse für Raw-Bitmaps.
class baseArchivItem_Bitmap_Raw : public virtual baseArchivItem_Bitmap
{
public:
    baseArchivItem_Bitmap_Raw();

    baseArchivItem_Bitmap_Raw(const baseArchivItem_Bitmap_Raw& item);

    ~baseArchivItem_Bitmap_Raw() override;

    /// lädt die Bilddaten aus einer Datei.
    int load(std::istream& file, const ArchivItem_Palette* palette) override;

    /// schreibt die Bilddaten in eine Datei.
    int write(std::ostream& file, const ArchivItem_Palette* palette) const override;
};

/// Klasse für Raw-Bitmaps.
class ArchivItem_Bitmap_Raw : public baseArchivItem_Bitmap_Raw, public ArchivItem_Bitmap
{
public:
    ArchivItem_Bitmap_Raw() = default;

    ArchivItem_Bitmap_Raw(const ArchivItem_Bitmap_Raw& item)
        : ArchivItem_BitmapBase(item), baseArchivItem_Bitmap(item), baseArchivItem_Bitmap_Raw(item),
          ArchivItem_Bitmap(item)
    {}

    RTTR_CLONEABLE(ArchivItem_Bitmap_Raw)
};
} // namespace libsiedler2
