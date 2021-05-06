// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

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
