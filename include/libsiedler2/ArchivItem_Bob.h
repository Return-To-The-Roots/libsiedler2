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
    struct OverlayIdx
    {
        uint16_t value;
    };
    struct OverlayData
    {
        OverlayIdx idx;   // Index of the overlay
        int16_t nyOffset; // Extra y-offset of the overlay for this sprite
    };

    ArchivItem_Bob();
    ~ArchivItem_Bob() override;
    RTTR_CLONEABLE(ArchivItem_Bob)

    /// Load BOB data from file
    int load(std::istream& file, const ArchivItem_Palette* palette);

    /// Write BOB data to file. TODO: Implement
    static int write(std::ostream& file, const ArchivItem_Palette* palette);

    uint32_t getNumOverlayImgs() const { return numOverlayImgs; }

    /// Return the number of complete sprites, consisting of body+overlay.
    uint16_t getNumSprites() const { return static_cast<uint16_t>(spriteOverlayData.size()); }

    /// Return the overlay index/nyOffset pair of the overlay for a given sprite idx.
    OverlayData getOverlayData(unsigned spriteIdx) const { return spriteOverlayData[spriteIdx]; }
    /// Returns the index in the loaded archive for the overlay indx
    unsigned getOverlayArchiveIndex(OverlayIdx overlayIdx);

    ArchivItem_Bitmap_Player* getBody(bool fat, ImgDir direction, unsigned animationstep);

    /// Get the overlay image to be drawn above a body image.
    /// @param bobId         the id of a good or a job
    /// @param fat           fat or slim body type (usually only different for carriers)
    /// @param direction     the direction to get
    /// @param animationstep the step to get [0-7]
    /// @return The overlay image and the y-offset relative to its ny
    std::pair<ArchivItem_Bitmap_Player*, int16_t> getOverlay(unsigned bobId, bool fat, ImgDir direction,
                                                             unsigned animationstep);

    /// Write the [sprite -> overlay] array in mapping format (TAB separated entries with # comments)
    void writeLinks(std::ostream& file) const;
    static std::map<uint16_t, OverlayData> readLinks(std::istream& file);

protected:
    /// Number of actual overlay pictures (e.g. carried wares)
    uint16_t numOverlayImgs;
    /// Array [bobId][animStep=8][fat=2][direction=6] mapping to overlay data
    std::vector<OverlayData> spriteOverlayData;
};
} // namespace libsiedler2
