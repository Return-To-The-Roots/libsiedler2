// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "Archiv.h"
#include "ArchivItem.h"
#include "ImgDir.h"
#include <cstdint>
#include <iosfwd>
#include <map>
#include <vector>

namespace libsiedler2 {
class ArchivItem_Palette;
class ArchivItem_Bitmap_Player;
} // namespace libsiedler2

namespace libsiedler2 {

class ArchivItem_Bob : public ArchivItem, public Archiv
{
public:
    ArchivItem_Bob();
    ~ArchivItem_Bob() override;
    RTTR_CLONEABLE(ArchivItem_Bob)

    /// Load BOB data from file
    int load(std::istream& file, const ArchivItem_Palette* palette);

    /// Write BOB data to file. TODO: Implement
    static int write(std::ostream& file, const ArchivItem_Palette* palette);

    uint32_t getNumOverlayImgs() const { return numOverlayImgs; }

    /// Returns the number of complete sprites, consisting of body+overlay.
    uint16_t getNumSprites() const { return static_cast<uint16_t>( spriteOverlayIdx.size()); }

    /// Returns the index of the overlay for a given sprite id.
    /// the sprite id is (bobId * 96 + animStep * 12 + fat * 6 + direction)
    uint16_t getOverlayIdx(unsigned spriteId) const { return spriteOverlayIdx[spriteId]; }
    
    ArchivItem_Bitmap_Player* getBody(bool fat, ImgDir direction, unsigned animationstep);

    /// Gets the overlay image to be drawn above a body image.
    /// @param bobId         the id of a good or a job
    /// @param fat           can be true and false for carriers, other jobs have only one body type
    /// @param direction     the direction to get [0-5]
    /// @param animationstep the step to get [0-7]
    /// @return The overlay image and the y-offset. The total offset is overlay.first.getNx() + overlay.second
    std::pair< ArchivItem_Bitmap_Player*, int16_t> getOverlay(unsigned bobId, bool fat, ImgDir direction, unsigned animationstep);

    /// Write the [sprite -> overlay] array in mapping format (TAB separated entries with # comments)
    void writeLinks(std::ostream& file) const;
    static std::map<uint16_t, uint16_t> readLinks(std::istream& file);

protected:
    /// Number of actual overlay pictures (e.g. carried wares)
    uint16_t numOverlayImgs;      
    /// Array [bobId][animStep=8][fat=2][direction=6] mapping to an overlay picture
    std::vector<uint16_t> spriteOverlayIdx;
    /// Array [bobId][animStep=8][fat=2][direction=6] of sprite's overlay's ny offsets.
    std::vector<int16_t> spriteOverlayNyOffset; 
};
} // namespace libsiedler2
