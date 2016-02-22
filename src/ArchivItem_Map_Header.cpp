// Copyright (c) 2005 - 2015 Settlers Freaks (sf-team at siedler25.org)
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

///////////////////////////////////////////////////////////////////////////////
// Header
#include "main.h" // IWYU pragma: keep
#include "ArchivItem_Map_Header.h"
#include "oem.h"
#include <fstream>
#include "EndianStream.h"
#include <cstring>

// Include last!
#include "DebugNew.h" // IWYU pragma: keep

///////////////////////////////////////////////////////////////////////////////
/** @class libsiedler2::ArchivItem_Map_Header
 *
 *  Klasse für einen Mapheader.
 *
 *  @author FloSoft
 */

///////////////////////////////////////////////////////////////////////////////
/**
 *  Konstruktor von @p ArchivItem_Map_Header.
 *
 *  @author FloSoft
 */
libsiedler2::ArchivItem_Map_Header::ArchivItem_Map_Header(void)
    : ArchivItem(),
      width(0), height(0), gfxset(0), player(0), hasExtraWord_(false)
{
    setBobType(BOBTYPE_MAP_HEADER);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Destruktor von @p ArchivItem_Map_Header.
 *
 *  @author FloSoft
 */
libsiedler2::ArchivItem_Map_Header::~ArchivItem_Map_Header(void)
{
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  lädt den Mapheader aus einer Datei.
 *
 *  @param[in] file Dateihandle der Datei
 *
 *  @return liefert Null bei Erfolg, ungleich Null bei Fehler
 *
 *  @author FloSoft
 */
int libsiedler2::ArchivItem_Map_Header::load(std::istream& file)
{
    if(!file)
        return 1;

    const char VALID_ID[10] = {'W', 'O', 'R', 'L', 'D', '_', 'V', '1', '.', '0'};
    char id[10];

    libendian::LittleEndianIStreamRef fs(file);
    // Signatur einlesen
    fs >> id;

    // und prüfen
    if(memcmp(id, VALID_ID, 10) != 0)
        return 3;

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

    fs.ignore(7*2*sizeof(uint16_t)); // HQPos: 7 * X+Y

    char isInvalid;
    fs >> isInvalid; // This should be checked, but it seems some editors wrongly leave it set

    fs.ignore(7*sizeof(uint8_t)); // Player faces/race * 7
    fs.ignore(250*9); // Water and land masses, 250 structs with 9 bytes size: Type ID (1 byte): 0 = unused, 1 = land, 2 = water; X/Y (2 bytes each), Total mass (4 bytes)

    uint16_t headerSig;
    fs >> headerSig;
    if(headerSig == 0) // On the map "Das Dreieck" there are 2 extra bytes here. This is a bug in the map!
    {
        fs >> headerSig;
        hasExtraWord_ = true;
    }else
        hasExtraWord_ = false;
    if(headerSig != 0x2711)
        return 4;

    uint32_t unknown; // Might be switch for short or long header blocks in WORLD#.DAT files
    fs >> unknown;
    assert(unknown == 0);

    // Breite einlesen
    fs >> width;

    // Höhe einlesen
    fs >> height;

    return 0;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  schreibt den Mapheader in eine Datei.
 *
 *  @param[in] file Dateihandle der Datei
 *
 *  @return liefert Null bei Erfolg, ungleich Null bei Fehler
 *
 *  @author FloSoft
 */
int libsiedler2::ArchivItem_Map_Header::write(std::ostream& file) const
{
    if(!file)
        return 1;

    return 256;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  liefert den Namen der Map.
 *
 *  @author FloSoft
 */
const std::string& libsiedler2::ArchivItem_Map_Header::getName(void) const
{
    return name_;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  setzt den Namen der Map.
 *
 *  @param[in] name Name der Map
 *
 *  @author FloSoft
 */
void libsiedler2::ArchivItem_Map_Header::setName(const std::string& name)
{
    this->name_ = name;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  liefert die Breite der Map.
 *
 *  @author FloSoft
 */
unsigned short libsiedler2::ArchivItem_Map_Header::getWidth(void) const
{
    return width;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  setzt die Breite der Map.
 *
 *  @param[in] width Breite der Map
 *
 *  @author FloSoft
 */
void libsiedler2::ArchivItem_Map_Header::setWidth(unsigned short width)
{
    this->width = width;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  liefert die Höhe der Map.
 *
 *  @author FloSoft
 */
unsigned short libsiedler2::ArchivItem_Map_Header::getHeight(void) const
{
    return height;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  setzt die Höhe der Map.
 *
 *  @param[in] height Höhe der Map
 *
 *  @author FloSoft
 */
void libsiedler2::ArchivItem_Map_Header::setHeight(unsigned short height)
{
    this->height = height;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  liefert das GfxSet der Map.
 *
 *  @author FloSoft
 */
unsigned char libsiedler2::ArchivItem_Map_Header::getGfxSet(void) const
{
    return gfxset;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  setzt das GfxSet der Map.
 *
 *  @param[in] gfxset GfxSet der Map
 *
 *  @author FloSoft
 */
void libsiedler2::ArchivItem_Map_Header::setGfxSet(unsigned char gfxset)
{
    this->gfxset = gfxset;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  liefert die Spielerzahl der Map.
 *
 *  @author FloSoft
 */
unsigned char libsiedler2::ArchivItem_Map_Header::getPlayer(void) const
{
    return player;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  setzt die Spielerzahl der Map.
 *
 *  @param[in] player Spielerzahl der Map
 *
 *  @author FloSoft
 */
void libsiedler2::ArchivItem_Map_Header::setPlayer(unsigned char player)
{
    this->player = player;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  liefert den Autor der Map.
 *
 *  @author FloSoft
 */
const std::string& libsiedler2::ArchivItem_Map_Header::getAuthor(void) const
{
    return author_;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  setzt den Autor der Map.
 *
 *  @param[in] author Autor der Map
 *
 *  @author FloSoft
 */
void libsiedler2::ArchivItem_Map_Header::setAuthor(const std::string& author)
{
    this->author_ = author;
}
