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

#include "libSiedler2Defines.h" // IWYU pragma: keep
#include "ArchivItem_Palette.h"
#include "libendian/src/EndianIStreamAdapter.h"
#include "libendian/src/EndianOStreamAdapter.h"
#include <boost/static_assert.hpp>
#include <iostream>
#include <stdexcept>

/** @var TRANSPARENT_INDEX
 *
 *  Index des Transparenzwertes.
 */

/** @class libsiedler2::ArchivItem_Palette
 *
 *  Klasse für Paletten.
 */

libsiedler2::ArchivItem_Palette::ArchivItem_Palette()
{
    setBobType(BOBTYPE_PALETTE);
}

/**
 *  Konstruktor von @p ArchivItem_Palette mit Laden der Farbwerte aus
 *  einer Datei.
 *
 *  @param[in] file Dateihandle aus denen die Farbwerte geladen werden sollen
 *  @param[in] skip Sollen 2 Unbekannte Bytes übersprungen werden (bei LST) oder direkt?
 */
libsiedler2::ArchivItem_Palette::ArchivItem_Palette(std::istream& file, bool skip) : ArchivItem()
{
    setBobType(BOBTYPE_PALETTE);

    load(file, skip);
}

 libsiedler2::ArchivItem_Palette::~ArchivItem_Palette()
 {
 }

/**
 *  liest die Farbwerte aus einer Datei.
 *
 *  @param[in] file Dateihandle aus denen die Farbwerte geladen werden sollen
 *  @param[in] skip Sollen 2 Unbekannte Bytes übersprungen werden (bei LST) oder direkt?
 *
 *  @return liefert Null bei Erfolg, ungleich Null bei Fehler
 */
int libsiedler2::ArchivItem_Palette::load(std::istream& file, bool skip)
{
    if(!file)
        return 1;

    libendian::EndianIStreamAdapter<false, std::istream&> fs(file);
    if(skip)
    {
        // Unbekannte 2 Bytes überspringen
        fs.ignore(2);
    }

    BOOST_STATIC_ASSERT_MSG(sizeof(colors) == 256u * 3u, "Color array has alignment. Cannot read it in whole");
    fs.read(&colors[0].r, sizeof(colors));

    // alles ok
    return (!file) ? 99 : 0;
}

/**
 *  schreibt die Farbwerte in eine Datei
 *
 *  @param[in] file Dateihandle in die die Farbwerte geschrieben werden sollen
 *  @param[in] skip Sollen 2 Unbekannte Bytes geschrieben werden (bei LST) oder direkt?
 *
 *  @return liefert Null bei Erfolg, ungleich Null bei Fehler
 */
int libsiedler2::ArchivItem_Palette::write(std::ostream& file, bool skip) const
{
    if(!file)
        return 1;

    libendian::EndianOStreamAdapter<false, std::ostream&> fs(file);
    if(skip)
    {
        int16_t unknown = 0x0100;
        fs << unknown;
    }

    BOOST_STATIC_ASSERT_MSG(sizeof(colors) == 256u * 3u, "Color array has alignment. Cannot write it in whole");
    fs.write(&colors[0].r, sizeof(colors));

    // alles ok
    return (!file) ? 99 : 0;
}

/**
 *  setzt einen Farbwert am entsprechenden Index.
 *
 *  @param[in] index Index des Farbwertes
 *  @param[in] r     Roter Farbwert
 *  @param[in] g     Grüner Farbwert
 *  @param[in] b     Blauer Farbwert
 */
void libsiedler2::ArchivItem_Palette::set(uint8_t index, ColorRGB clr)
{
    colors[index] = clr;
}

/**
 *  liefert einen Index zum entsprechenden RGB-Wert.
 *
 *  @param[in] r     Roter Farbwert
 *  @param[in] g     Grüner Farbwert
 *  @param[in] b     Blauer Farbwert
 *
 *  @return Farbindex der RGB-Farbe
 */
uint8_t libsiedler2::ArchivItem_Palette::lookup(const ColorRGB& clr) const
{
    for(unsigned i = 0; i < 256; ++i)
    {
        if(colors[i] == clr)
            return static_cast<uint8_t>(i);
    }
    throw std::runtime_error("Color not found in palette!");
}

uint8_t libsiedler2::ArchivItem_Palette::lookupOrDef(const ColorRGB & clr, uint8_t defaultVal) const
{
    for(unsigned i = 0; i < 256; ++i)
    {
        if(colors[i] == clr)
            return static_cast<uint8_t>(i);
    }
    return defaultVal;
}

/**
 *  Index-Operator von @p ArchivItem_Palette.
 *
 *  @param[in] index Index des zu liefernden Eintrags
 *
 *  @return Bei Erfolg einen Pointer auf einen, ansonsten NULL
 */
const libsiedler2::ColorRGB& libsiedler2::ArchivItem_Palette::operator[](unsigned index) const
{
    if(index < 256)
        return colors[index];
    else
        throw std::out_of_range("index");
}

/**
 *  kopiert die Palette in einen Puffer (als BGRA)
 *
 *  @param[in,out] buffer Zielpuffer für die Palettendaten (muss 256*4-Byte groß sein)
 */
void libsiedler2::ArchivItem_Palette::copy(uint8_t* buffer, size_t bufSize, bool writeFakeTransparency) const
{
    if(bufSize < 256 * 4)
        throw std::runtime_error("Buffer is to small!");

    // Farben kopieren
    for(uint16_t i = 0; i < 256; ++i)
    {
        buffer[i * 4 + 0] = colors[i].b;
        buffer[i * 4 + 1] = colors[i].g;
        buffer[i * 4 + 2] = colors[i].r;
        buffer[i * 4 + 3] = 0xFF;
    }
    if(writeFakeTransparency)
    {
        buffer[TRANSPARENT_INDEX * 4 + 0] = TRANSPARENT_COLOR.b;
        buffer[TRANSPARENT_INDEX * 4 + 1] = TRANSPARENT_COLOR.g;
        buffer[TRANSPARENT_INDEX * 4 + 2] = TRANSPARENT_COLOR.r;
    } else
    {
        // Transparentes Element transparent machen
        buffer[TRANSPARENT_INDEX * 4 + 0] = 0x00;
        buffer[TRANSPARENT_INDEX * 4 + 1] = 0x00;
        buffer[TRANSPARENT_INDEX * 4 + 2] = 0x00;
        buffer[TRANSPARENT_INDEX * 4 + 3] = 0x00;
    }
}
