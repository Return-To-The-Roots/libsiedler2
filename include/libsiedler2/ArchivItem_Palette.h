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
#ifndef ARCHIVITEM_PALETTE_H_INCLUDED
#define ARCHIVITEM_PALETTE_H_INCLUDED

#pragma once

#include "ArchivItem.h"
#include "ColorRGB.h"
#include <boost/array.hpp>
#include <cstddef>
#include <iosfwd>
#include <stdint.h>

namespace libsiedler2 {

/// A bright pink color very unlikely to be used in textures.
/// It is the same as index 254 in PAL5 and in some files used as a background color
const ColorRGB TRANSPARENT_COLOR(0xff, 0, 0x8f);

/// Klasse für Paletten.
class ArchivItem_Palette : public ArchivItem
{
public:
    /// The choice for the default transparent index is kind of arbitrary as in most files we have a separate encoding for transparent
    /// pixels. However in the LBMs containing the terrain textures we do have parts that should be transparent (background of edges and
    /// paths) and there index 0 is used consistently. Also PAL5 (default) contains more than 1 black color and index 0 does not seem to be
    /// used for black in the S2 LST files. For compatibility we use this only if the palette does not contain TRANSPARENT_COLOR.
    static const uint8_t DEFAULT_TRANSPARENT_IDX = 0;

    ArchivItem_Palette();

    ~ArchivItem_Palette() override;
    RTTR_CLONEABLE(ArchivItem_Palette)

    /// Load the palette from the file and set the transparent index to the index of TRANSPARENT_COLOR or DEFAULT_TRANSPARENT_IDX
    int load(std::istream& file, bool skip = true);

    /// schreibt die Farbwerte in eine Datei.
    int write(std::ostream& file, bool skip = true) const;

    /// setzt einen Farbwert am entsprechenden Index.
    void set(uint8_t index, ColorRGB clr);

    /// liefert einen Farbwert am entsprechenden Index.
    ColorRGB get(uint8_t index) const { return (*this)[index]; }

    /// Write the index with the given color into clrIdx. Returns true iff color found
    /// If color is not found, then clrIdx is NOT changed
    bool lookup(const ColorRGB& clr, uint8_t& clrIdx) const;
    /// Return the index with the given color. Throws an exception if not found
    uint8_t lookup(const ColorRGB& clr) const;

    /// Return the (first) index with the given color or defaultVal if none found
    uint8_t lookupOrDef(const ColorRGB& clr, uint8_t defaultVal = 0) const;

    /// Index-Operator von @p ArchivItem_Palette.
    const ColorRGB& operator[](unsigned index) const;

    /// Copy palette to the buffer as BGRA. Ignores transparency
    void copyToBGRA(uint8_t* buffer, size_t bufSize) const;

    /// Return true iff the 2 palettes specify the same colors
    bool isEqual(const ArchivItem_Palette& other) const;

    uint8_t transparentIdx;

protected:
    boost::array<ColorRGB, 256> colors; //-V730_NOINIT
};
} // namespace libsiedler2

#endif // !ARCHIVITEM_PALETTE_H_INCLUDED
