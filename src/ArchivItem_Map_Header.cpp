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

#include "libSiedler2Defines.h" // IWYU pragma: keep
#include "ArchivItem_Map_Header.h"
#include "ErrorCodes.h"
#include "fileFormatHelpers.h"
#include "oem.h"
#include "libendian/src/EndianIStreamAdapter.h"
#include "libendian/src/EndianOStreamAdapter.h"
#include <iostream>

/** @class libsiedler2::ArchivItem_Map_Header
 *
 *  Klasse für einen Mapheader.
 */

const char VALID_ID[11] = "WORLD_V1.0";

libsiedler2::ArchivItem_Map_Header::ArchivItem_Map_Header()
    : ArchivItem(), width(0), height(0), gfxset(0), player(0), isInvalid(0), hasExtraWord_(false)
{
    bobtype_ = BOBTYPE_MAP_HEADER;
}

libsiedler2::ArchivItem_Map_Header::~ArchivItem_Map_Header()
{
}

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

    char id[10];

    libendian::EndianIStreamAdapter<false, std::istream&> fs(file);
    // Signatur einlesen
    fs >> id;

    // und prüfen
    if(!fs || !isChunk(id, VALID_ID))
        return ErrorCode::WRONG_HEADER;

    // Name einlesen
    char name[24];
    fs >> name;
    OemToAnsi(name, name);
    this->name_ = name;

    // GFX-Set einlesen
    fs >> gfxset;

    // Spielerzahl einlesen
    fs >> player;

    // Autor einlesen
    char author[20];
    fs >> author;
    OemToAnsi(author, author);
    this->author_ = author;

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
    char id[10];
    setChunkId(id, VALID_ID);
    fs << id;

    // Name einlesen
    char name[24];
    std::string tmpName = name_.substr(0, 23);
    AnsiToOem(tmpName.c_str(), name);
    std::fill(name + tmpName.length(), name + sizeof(name), '\0');
    fs << name;
    // Actual map name length is only 20 bytes. The width and height is used for unlimited play only to display the size
    // We use it if the name (including trailing zero) fit into this space, otherwise we overwrite the size with the map name
    if(tmpName.length() < 20)
    {
        fs.setPositionRel(-4);
        fs << width << height;
    }

    fs << gfxset << player;

    // Autor einlesen
    char author[20];
    tmpName = author_.substr(0, 19);
    AnsiToOem(tmpName.c_str(), author);
    std::fill(author + tmpName.length(), author + sizeof(author), '\0');
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
 *  liefert den Namen der Map.
 */
const std::string& libsiedler2::ArchivItem_Map_Header::getName() const
{
    return name_;
}

/**
 *  setzt den Namen der Map.
 *
 *  @param[in] name Name der Map
 */
void libsiedler2::ArchivItem_Map_Header::setName(const std::string& name)
{
    this->name_ = name;
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
uint8_t libsiedler2::ArchivItem_Map_Header::getPlayer() const
{
    return player;
}

/**
 *  setzt die Spielerzahl der Map.
 *
 *  @param[in] player Spielerzahl der Map
 */
void libsiedler2::ArchivItem_Map_Header::setPlayer(uint8_t player)
{
    this->player = player;
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
void libsiedler2::ArchivItem_Map_Header::getPlayerHQ(const uint32_t player, uint16_t& x, uint16_t& y)
{
    x = playerHQx[player];
    y = playerHQy[player];
}
