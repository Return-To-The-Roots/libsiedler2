// Copyright (c) 2005 - 2020 Settlers Freaks (sf-team at siedler25.org)
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

#pragma once

#include "ArchivItem.h"
#include <cstdint>
#include <memory>

namespace libsiedler2 {

class ArchivItem_Palette;

/// Class that describes an animation through a palette color rotation
class ArchivItem_PaletteAnimation : public ArchivItem
{
public:
    ArchivItem_PaletteAnimation();
    RTTR_CLONEABLE(ArchivItem_PaletteAnimation)

    int load(std::istream& file);
    int write(std::ostream& file) const;
    int loadFromTxt(std::istream& file);
    int writeToTxt(std::ostream& file) const;

    /// Is this animation enabled
    bool isActive;
    /// If true, cycle is forward (idx[n+1] = idx[n]+1) else backward
    bool moveUp;
    /// Scaled frame rate. 8192/30 * 1/rate is the time in seconds
    uint16_t rate;
    /// First and last color used. Note the wrapping: lastClr + 1 = firstClr
    uint8_t firstClr, lastClr;
    /// Creates a new palette from the given one with the rotation applied
    std::unique_ptr<ArchivItem_Palette> apply(const ArchivItem_Palette& pal) const;
};

} // namespace libsiedler2
