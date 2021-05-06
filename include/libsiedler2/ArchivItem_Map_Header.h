// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "ArchivItem.h"
#include <array>
#include <cstdint>
#include <iosfwd>
#include <string>

namespace libsiedler2 {
/// Klasse für einen Mapheader.
class ArchivItem_Map_Header : public ArchivItem
{
public:
    /// Maximum number of players supported in the map header
    static constexpr unsigned maxPlayers = 7; // S2 supports only 7 players

    ArchivItem_Map_Header();
    ~ArchivItem_Map_Header() override;
    RTTR_CLONEABLE(ArchivItem_Map_Header)

    /// lädt den Mapheader aus einer Datei.
    int load(std::istream& file);
    /// schreibt den Mapheader in eine Datei.
    int write(std::ostream& file) const;

    uint16_t getWidth() const;
    void setWidth(uint16_t width);

    uint16_t getHeight() const;
    void setHeight(uint16_t height);

    uint8_t getGfxSet() const;
    void setGfxSet(uint8_t gfxset);

    uint8_t getNumPlayers() const;
    void setNumPlayers(uint8_t numPlayers);

    const std::string& getAuthor() const;
    void setAuthor(const std::string& author);

    void setPlayerHQ(uint32_t player, uint16_t x, uint16_t y);
    void getPlayerHQ(uint32_t player, uint16_t& x, uint16_t& y) const;

    bool hasExtraWord() const { return hasExtraWord_; }

private:
    uint16_t width = 0;
    uint16_t height = 0;
    uint8_t gfxset = 0;
    uint8_t numPlayers = 0;
    std::string author_;
    std::array<uint16_t, maxPlayers> playerHQx{}, playerHQy{};
    uint8_t isInvalid = 0;
    std::array<uint8_t, maxPlayers> playerFaces{};
    /// 250 entries with: uint8 Type(0:Unused, 1:water, 2:land), uint16 x,y, uint32 size
    std::array<uint8_t, 250 * 9> areaInfos{};
    bool hasExtraWord_ = false;
};
} // namespace libsiedler2
