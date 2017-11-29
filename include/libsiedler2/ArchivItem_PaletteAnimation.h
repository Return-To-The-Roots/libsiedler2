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

#ifndef ArchivItem_PaletteAnimation_h__
#define ArchivItem_PaletteAnimation_h__

#include "ArchivItem.h"
#include <stdint.h>
#include <string>

namespace libsiedler2 {

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
    /// Apply the rotation to the given color. Assumes isActive
    uint8_t getNextClr(uint8_t curClr) const;
};

} // namespace libsiedler2

#endif // ArchivItem_PaletteAnimation_h__
