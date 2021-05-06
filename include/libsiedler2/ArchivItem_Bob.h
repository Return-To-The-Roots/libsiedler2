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
    uint16_t getNumLinks() const { return static_cast<uint16_t>(links.size()); }
    uint16_t getOverlayIdx(unsigned idx) const { return links[idx]; };
    uint16_t getOverlayIdx(unsigned overlayIdx, bool fat, ImgDir direction, unsigned animationstep) const
    {
        // Array [overlayIdx][animStep][fat][direction]: [35][8][2][6]
        return getOverlayIdx(((overlayIdx * 8 + animationstep) * 2 + fat) * 6 + static_cast<unsigned>(direction));
    }
    ArchivItem_Bitmap_Player* getBody(bool fat, ImgDir direction, unsigned animationstep);
    ArchivItem_Bitmap_Player* getOverlay(unsigned overlayIdx, bool fat, ImgDir direction, unsigned animationstep);

    /// Write the links in mapping format (TAB separated entries with # comments)
    void writeLinks(std::ostream& file) const;
    static std::map<uint16_t, uint16_t> readLinks(std::istream& file);

protected:
    uint16_t numOverlayImgs;     /// Number of actual overlay pictures (e.g. carried wares)
    std::vector<uint16_t> links; /// Array [overlayId][animStep=8][fat=2][direction=6] mapping to an overlay picture
};
} // namespace libsiedler2
