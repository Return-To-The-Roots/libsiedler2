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

#include "ArchivItem_Map_Header.h"
#include "ErrorCodes.h"
#include "fileFormatHelpers.h"
#include "oem.h"
#include "libendian/EndianIStreamAdapter.h"
#include "libendian/EndianOStreamAdapter.h"
#include <array>
#include <iostream>
#include <stdexcept>

/** @class libsiedler2::ArchivItem_Map_Header
 *
 *  Klasse für einen Mapheader.
 */

const char VALID_ID[11] = "WORLD_V1.0";

libsiedler2::ArchivItem_Map_Header::ArchivItem_Map_Header() : ArchivItem(BobType::MapHeader) {}

libsiedler2::ArchivItem_Map_Header::~ArchivItem_Map_Header() = default;

/**
 *  lädt den Mapheader aus einer Datei.
 *
 *  @param[in] file Dateihandle der Datei
 *
 *  @return liefert Null bei Erfolg, ungleich Null bei Fehler
 */
int libsiedler2::ArchivItem_Map_Header::load(std::istream& file)
{
    if(!file)
        return ErrorCode::FILE_NOT_ACCESSIBLE;

    std::array<char, 10> id;

    libendian::EndianIStreamAdapter<false, std::istream&> fs(file);
    // Signatur einlesen
    fs >> id;

    // und prüfen
    if(!fs || !isChunk(id, VALID_ID))
        return ErrorCode::WRONG_HEADER;

    // Read name. Either 20B + width/height (2B each) or 24B
    // Ignore it for now and come back later
    long namePos = fs.getPosition();
    fs.setPositionRel(20);
    uint16_t posWidth, posHeight;
    fs >> posWidth >> posHeight;

    fs >> gfxset >> numPlayers;

    // 20B, should include nullptr
    std::array<char, 21> author;
    fs.readRaw(author.data(), 20);
    author.back() = '\0';
    this->author_ = OemToAnsi(author.data());

    fs >> playerHQx >> playerHQy;

    fs >> isInvalid; // This should be checked, but it seems some editors wrongly leave it set

    fs >> playerFaces >> areaInfos;

    uint16_t headerSig;
    fs >> headerSig;
    if(headerSig == 0) // On the map "Das Dreieck" there are 2 extra bytes here. This is a bug in the map!
    {
        fs >> headerSig;
        hasExtraWord_ = true;
    } else
        hasExtraWord_ = false;
    if(headerSig != 0x2711)
        return ErrorCode::WRONG_FORMAT;

    uint32_t unknown; // Might be switch for int16_t or long header blocks in WORLD#.DAT files
    fs >> unknown;
    assert(unknown == 0);

    fs >> width >> height;

    // What name format do we have?
    size_t nameLen;
    if(width == posWidth && height == posHeight)
        nameLen = 20;
    else
        nameLen = 24;
    long curPos = fs.getPosition();
    fs.setPosition(namePos);
    // It should include a nullptr terminator, but we have to make sure
    std::array<char, 25> name;
    fs.readRaw(name.data(), nameLen);
    name[nameLen] = '\0';
    fs.setPosition(curPos);

    setName(OemToAnsi(name.data()));

    return (!file) ? ErrorCode::UNEXPECTED_EOF : ErrorCode::NONE;
}

/**
 *  schreibt den Mapheader in eine Datei.
 *
 *  @param[in] file Dateihandle der Datei
 *
 *  @return liefert Null bei Erfolg, ungleich Null bei Fehler
 */
int libsiedler2::ArchivItem_Map_Header::write(std::ostream& file) const
{
    if(!file)
        return ErrorCode::FILE_NOT_ACCESSIBLE;

    libendian::EndianOStreamAdapter<false, std::ostream&> fs(file);
    // Signatur
    std::array<char, 10> id;
    setChunkId(id, VALID_ID);
    fs << id;

    // Name einlesen
    std::string tmpName = AnsiToOem(getName().substr(0, 23));
    std::array<char, 24> name;
    std::copy(tmpName.begin(), tmpName.end(), name.begin());
    std::fill(name.begin() + tmpName.length(), name.end(), '\0');
    fs << name;
    // Actual map name length is only 20 bytes. The width and height is used for unlimited play only to display the size
    // We use it if the name (including trailing zero) fit into this space, otherwise we overwrite the size with the map
    // name
    if(tmpName.length() < 20)
    {
        fs.setPositionRel(-4);
        fs << width << height;
    }

    fs << gfxset << numPlayers;

    // Autor einlesen
    tmpName = AnsiToOem(author_.substr(0, 19));
    std::array<char, 20> author;
    std::copy(tmpName.begin(), tmpName.end(), author.begin());
    std::fill(author.begin() + tmpName.length(), author.end(), '\0');
    fs << author << playerHQx << playerHQy;

    fs << isInvalid; // This should be checked, but it seems some editors wrongly leave it set

    fs << playerFaces << areaInfos;

    // Header sig
    fs << uint16_t(0x2711) << uint32_t(0);

    // Breite einlesen
    fs << width;

    // Höhe einlesen
    fs << height;

    return (!file) ? ErrorCode::UNEXPECTED_EOF : ErrorCode::NONE;
}

/**
 *  liefert die Breite der Map.
 */
uint16_t libsiedler2::ArchivItem_Map_Header::getWidth() const
{
    return width;
}

/**
 *  setzt die Breite der Map.
 *
 *  @param[in] width Breite der Map
 */
void libsiedler2::ArchivItem_Map_Header::setWidth(uint16_t width)
{
    this->width = width;
}

/**
 *  liefert die Höhe der Map.
 */
uint16_t libsiedler2::ArchivItem_Map_Header::getHeight() const
{
    return height;
}

/**
 *  setzt die Höhe der Map.
 *
 *  @param[in] height Höhe der Map
 */
void libsiedler2::ArchivItem_Map_Header::setHeight(uint16_t height)
{
    this->height = height;
}

/**
 *  liefert das GfxSet der Map.
 */
uint8_t libsiedler2::ArchivItem_Map_Header::getGfxSet() const
{
    return gfxset;
}

/**
 *  setzt das GfxSet der Map.
 *
 *  @param[in] gfxset GfxSet der Map
 */
void libsiedler2::ArchivItem_Map_Header::setGfxSet(uint8_t gfxset)
{
    this->gfxset = gfxset;
}

/**
 *  liefert die Spielerzahl der Map.
 */
uint8_t libsiedler2::ArchivItem_Map_Header::getNumPlayers() const
{
    return numPlayers;
}

/**
 *  setzt die Spielerzahl der Map.
 *
 *  @param[in] player Spielerzahl der Map
 */
void libsiedler2::ArchivItem_Map_Header::setNumPlayers(uint8_t numPlayers)
{
    this->numPlayers = numPlayers;
}

/**
 *  liefert den Autor der Map.
 */
const std::string& libsiedler2::ArchivItem_Map_Header::getAuthor() const
{
    return author_;
}

/**
 *  setzt den Autor der Map.
 *
 *  @param[in] author Autor der Map
 */
void libsiedler2::ArchivItem_Map_Header::setAuthor(const std::string& author)
{
    this->author_ = author;
}

/**
 * Sets position of the headquarter for the specified player.
 *
 * @param player player number (0-6)
 * @param x x-coordinate of the headquarter
 * @param y y-coordinate of the headquarter
 */
void libsiedler2::ArchivItem_Map_Header::setPlayerHQ(const uint32_t player, const uint16_t x, const uint16_t y)
{
    if(player >= playerHQx.size())
        throw std::range_error("Only " + std::to_string(maxPlayers) + " players allowed!");
    playerHQx[player] = x;
    playerHQy[player] = y;
}

/**
 * Gets position of the headquarter for the specified player.
 *
 * @param player player number (0-6)
 * @param[out] x x-coordinate of the headquarter
 * @param[out] y y-coordinate of the headquarter
 */
void libsiedler2::ArchivItem_Map_Header::getPlayerHQ(const uint32_t player, uint16_t& x, uint16_t& y) const
{
    if(player >= playerHQx.size())
        throw std::range_error("Only " + std::to_string(maxPlayers) + " players allowed!");
    x = playerHQx[player];
    y = playerHQy[player];
}
