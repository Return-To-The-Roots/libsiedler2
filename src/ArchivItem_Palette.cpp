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

#include "ArchivItem_Palette.h"
#include "ColorBGRA.h"
#include "ErrorCodes.h"
#include "libendian/EndianIStreamAdapter.h"
#include "libendian/EndianOStreamAdapter.h"
#include <iostream>
#include <sstream>
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
    : ArchivItem(BobType::Palette), transparentIdx(DEFAULT_TRANSPARENT_IDX)
{}

libsiedler2::ArchivItem_Palette::~ArchivItem_Palette() = default;

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
        return ErrorCode::FILE_NOT_ACCESSIBLE;

    libendian::EndianIStreamAdapter<false, std::istream&> fs(file);
    if(skip)
    {
        uint16_t numColors;
        if(!(fs >> numColors) || numColors != 256)
            return ErrorCode::WRONG_HEADER;
    }

    static_assert(sizeof(colors) == 256u * 3u, "Color array has alignment. Cannot read it in whole");
    fs.read(&colors[0].r, sizeof(colors));

    setDefaultTransparentIdx();

    return (!file) ? ErrorCode::UNEXPECTED_EOF : ErrorCode::NONE;
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
        return ErrorCode::FILE_NOT_ACCESSIBLE;

    libendian::EndianOStreamAdapter<false, std::ostream&> fs(file);
    if(skip)
        fs << int16_t(256);

    static_assert(sizeof(colors) == 256u * 3u, "Color array has alignment. Cannot write it in whole");
    fs.write(&colors[0].r, sizeof(colors));

    return (!file) ? ErrorCode::UNEXPECTED_EOF : ErrorCode::NONE;
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

bool libsiedler2::ArchivItem_Palette::lookup(const ColorRGB& clr, uint8_t& clrIdx) const
{
    for(unsigned i = 0; i < 256; ++i)
    {
        if(i != transparentIdx && colors[i] == clr)
        {
            clrIdx = static_cast<uint8_t>(i);
            return true;
        }
    }
    if(hasTransparency() && colors[transparentIdx] == clr)
    {
        clrIdx = getTransparentIdx();
        return true;
    }
    return false;
}

uint8_t libsiedler2::ArchivItem_Palette::lookup(const ColorRGB& clr) const
{
    uint8_t result;
    if(!lookup(clr, result))
    {
        std::stringstream str;
        str << "Color(" << uint32_t(clr.r) << "," << uint32_t(clr.g) << "," << uint32_t(clr.b) << ")"
            << " not found in palette!";
        throw std::runtime_error(str.str());
    }
    return result;
}

uint8_t libsiedler2::ArchivItem_Palette::lookupOrDef(const ColorRGB& clr, uint8_t defaultVal) const
{
    uint8_t result = defaultVal;
    lookup(clr, result);
    return result;
}

/**
 *  Index-Operator von @p ArchivItem_Palette.
 *
 *  @param[in] index Index des zu liefernden Eintrags
 *
 *  @return Bei Erfolg einen Pointer auf einen, ansonsten nullptr
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
void libsiedler2::ArchivItem_Palette::copyToBGRA(uint8_t* buffer, size_t bufSize) const
{
    if(bufSize < 256 * 4)
        throw std::runtime_error("Buffer is to small!");

    // Farben kopieren
    for(uint16_t i = 0; i < 256; ++i)
        ColorBGRA(colors[i]).toBGRA(&buffer[i * 4]);
}

void libsiedler2::ArchivItem_Palette::setDefaultTransparentIdx()
{
    // If the palette contains the transparent color, we use that, otherwise we use the default.
    // This is mostly for backwards compatibility (we used to use index 254 which is that pink in PAL5) and ease of use
    // (black as transparent color in bmps might be confusing)
    transparentIdx = lookupOrDef(TRANSPARENT_COLOR, DEFAULT_TRANSPARENT_IDX);
}
