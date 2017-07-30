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
#include "ArchivItem_Bitmap_Raw.h"
#include "ArchivItem_Palette.h"
#include "libendian/src/EndianIStreamAdapter.h"
#include "libendian/src/EndianOStreamAdapter.h"
#include <iostream>
#include <vector>
namespace libsiedler2 { class ArchivItem_Palette; }
/** @class libsiedler2::baseArchivItem_Bitmap_Raw
 *
 *  Basisklasse für Raw-Bitmaps.
 */

/** @class libsiedler2::ArchivItem_Bitmap_Raw
 *
 *  Klasse für Raw-Bitmaps.
 */

libsiedler2::baseArchivItem_Bitmap_Raw::baseArchivItem_Bitmap_Raw()
{
    setBobType(BOBTYPE_BITMAP_RAW);
}

libsiedler2::baseArchivItem_Bitmap_Raw::baseArchivItem_Bitmap_Raw(const baseArchivItem_Bitmap_Raw& item) : baseArchivItem_Bitmap(item)
{
    setBobType(BOBTYPE_BITMAP_RAW);
}

/**
 *  Konstruktor von @p baseArchivItem_Bitmap_Raw mit Laden der Bilddaten aus
 *  einer Datei.
 *
 *  @param[in] file    Dateihandle der Datei
 *  @param[in] palette Grundpalette
 */
libsiedler2::baseArchivItem_Bitmap_Raw::baseArchivItem_Bitmap_Raw(std::istream& file, const ArchivItem_Palette* palette) : baseArchivItem_Bitmap()
{
    setBobType(BOBTYPE_BITMAP_RAW);
    load(file, palette);
}

libsiedler2::baseArchivItem_Bitmap_Raw::~baseArchivItem_Bitmap_Raw()
{
}

/**
 *  lädt die Bilddaten aus einer Datei.
 *
 *  @param[in] file    Dateihandle der Datei
 *  @param[in] palette Grundpalette
 *
 *  @return liefert Null bei Erfolg, ungleich Null bei Fehler
 */
int libsiedler2::baseArchivItem_Bitmap_Raw::load(std::istream& file, const ArchivItem_Palette* palette)
{
    if(!file)
        return 1;
    if(palette == NULL)
        palette = getPalette();
    if(palette == NULL)
        return 2;

    libendian::EndianIStreamAdapter<false, std::istream&> fs(file);
    // Unbekannte Daten überspringen
    fs.ignore(2);

    // Länge einlesen
    uint32_t length;
    fs >> length;

    std::vector<uint8_t> data(length);
    uint16_t width, height;
    // Daten einlesen
    fs >> data >> nx_ >> ny_ >> width >> height;

    if(length != width * height)
        return 4;

    // Speicher anlegen
    if(length > 0 && !create(width, height, &data[0], width, height, FORMAT_PALETTED, palette))
        return 5;

    // Unbekannte Daten überspringen
    fs.ignore(8);

    return (!fs) ? 99 : 0;
}

/**
 *  schreibt die Bilddaten in eine Datei.
 *
 *  @param[in] file    Dateihandle der Datei
 *  @param[in] palette Grundpalette
 *
 *  @return liefert Null bei Erfolg, ungleich Null bei Fehler
 */
int libsiedler2::baseArchivItem_Bitmap_Raw::write(std::ostream& file, const ArchivItem_Palette* palette) const
{
    if(!file)
        return 1;
    if(palette == NULL)
        palette = getPalette();

    const uint16_t width = getWidth(), height = getHeight();

    if(width == 0 ||height == 0)
        return 2;

    libendian::EndianOStreamAdapter<false, std::ostream&> fs(file);
    uint32_t length = width * height;
    std::vector<uint8_t> buffer(length, libsiedler2::TRANSPARENT_INDEX);
    if(!print(&buffer[0], width, height, FORMAT_PALETTED, palette))
        return 3;

    char unknown[2] = {0x01, 0x00};
    uint8_t unknown2[8] = { 0x00, 0x00, 0x02, 0x01, 0xF4, 0x06, 0x70, 0x00 };
    fs << unknown << length << buffer << nx_ << ny_ << width << height << unknown2;

    return (!fs) ? 99 : 0;
}
