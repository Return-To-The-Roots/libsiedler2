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
#include <fstream>
#include "libendian/src/EndianIStreamAdapter.h"
#include "libendian/src/EndianOStreamAdapter.h"
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
        palette = this->palette_;
    if(palette == NULL)
        return 2;

    tex_clear();
    libendian::EndianIStreamAdapter<false, std::istream&> fs(file);
    // Unbekannte Daten überspringen
    fs.ignore(2);

    // Länge einlesen
    uint32_t length;
    fs >> length;

    std::vector<uint8_t> data(length);
    // Daten einlesen
    fs >> data;

    // Nullpunkt X einlesen
    fs >> nx_;

    // Nullpunkt Y einlesen
    fs >> ny_;

    // Breite einlesen
    fs >> width_;

    // Höhe einlesen
    fs >> height_;

    // Speicher anlegen
    tex_alloc();

    if(length != 0)
    {
        for(uint16_t y = 0; y < height_; ++y)
        {
            for(uint16_t x = 0; x < width_; ++x)
            {
                // Pixel setzen
                tex_setPixel(x, y, data[y * width_ + x], palette);
            }
        }
    }

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
        palette = this->palette_;
    if(palette == NULL)
        return 2;

    if(width_ == 0 || height_ == 0)
        return 2;

    libendian::EndianOStreamAdapter<false, std::ostream&> fs(file);
    // Unbekannte Daten schreiben
    char unknown[2] = {0x01, 0x00};
    fs.write(unknown, sizeof(unknown));

    // Länge schreiben
    uint32_t length = width_ * height_;
    fs << length;

    for(uint16_t y = 0; y < height_; ++y)
    {
        for(uint16_t x = 0; x < width_; ++x)
        {
            // Pixel holen und schreiben
            fs << tex_getPixel(x, y, palette);
        }
    }

    // Nullpunkt X schreiben
    fs << nx_;

    // Nullpunkt Y schreiben
    fs << ny_;

    // Breite schreiben
    fs << width_;

    // Höhe schreiben
    fs << height_;

    // Unbekannte Daten schreiben
    uint8_t unknown2[8] = {0x00, 0x00, 0x02, 0x01, 0xF4, 0x06, 0x70, 0x00};
    fs.write(unknown2, sizeof(unknown2));

    return (!fs) ? 99 : 0;
}
