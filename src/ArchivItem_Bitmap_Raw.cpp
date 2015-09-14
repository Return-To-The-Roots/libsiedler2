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
#include "main.h"
#include "ArchivItem_Bitmap_Raw.h"
#include <fstream>
#include <EndianStream.h>
#include <vector>

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#if defined _WIN32 && defined _DEBUG && defined _MSC_VER
#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////////
/** @class libsiedler2::baseArchivItem_Bitmap_Raw
 *
 *  Basisklasse für Raw-Bitmaps.
 *
 *  @author FloSoft
 */

///////////////////////////////////////////////////////////////////////////////
/** @class libsiedler2::ArchivItem_Bitmap_Raw
 *
 *  Klasse für Raw-Bitmaps.
 *
 *  @author FloSoft
 */

///////////////////////////////////////////////////////////////////////////////
/**
 *  Konstruktor von @p baseArchivItem_Bitmap_Raw.
 *
 *  @author FloSoft
 */
libsiedler2::baseArchivItem_Bitmap_Raw::baseArchivItem_Bitmap_Raw(void) : baseArchivItem_Bitmap()
{
    setBobType(BOBTYPE_BITMAP_RAW);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Kopierkonstruktor von @p baseArchivItem_Bitmap_Raw.
 *
 *  @param[in] item Quellitem
 *
 *  @author FloSoft
 */
libsiedler2::baseArchivItem_Bitmap_Raw::baseArchivItem_Bitmap_Raw(const baseArchivItem_Bitmap_Raw& item) : baseArchivItem_Bitmap(item)
{
    setBobType(BOBTYPE_BITMAP_RAW);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Konstruktor von @p baseArchivItem_Bitmap_Raw mit Laden der Bilddaten aus
 *  einer Datei.
 *
 *  @param[in] file    Dateihandle der Datei
 *  @param[in] palette Grundpalette
 *
 *  @author FloSoft
 */
libsiedler2::baseArchivItem_Bitmap_Raw::baseArchivItem_Bitmap_Raw(std::istream& file, const ArchivItem_Palette* palette) : baseArchivItem_Bitmap()
{
    setBobType(BOBTYPE_BITMAP_RAW);
    load(file, palette);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Destruktor von @p baseArchivItem_Bitmap_Raw.
 *
 *  @author FloSoft
 */
libsiedler2::baseArchivItem_Bitmap_Raw::~baseArchivItem_Bitmap_Raw(void)
{
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  lädt die Bilddaten aus einer Datei.
 *
 *  @param[in] file    Dateihandle der Datei
 *  @param[in] palette Grundpalette
 *
 *  @return liefert Null bei Erfolg, ungleich Null bei Fehler
 *
 *  @author FloSoft
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
    libendian::LittleEndianIStreamRef fs(file);
    // Unbekannte Daten überspringen
    fs.ignore(2);

    // Länge einlesen
    unsigned int length;
    fs >> length;

    std::vector<unsigned char> data(length);
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
        for(unsigned short y = 0; y < height_; ++y)
        {
            for(unsigned short x = 0; x < width_; ++x)
            {
                // Pixel setzen
                tex_setPixel(x, y, data[y * width_ + x], palette);
            }
        }
    }

    // Unbekannte Daten überspringen
    fs.ignore(8);

    return 0;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  schreibt die Bilddaten in eine Datei.
 *
 *  @param[in] file    Dateihandle der Datei
 *  @param[in] palette Grundpalette
 *
 *  @return liefert Null bei Erfolg, ungleich Null bei Fehler
 *
 *  @author FloSoft
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

    libendian::LittleEndianOStreamRef fs(file);
    // Unbekannte Daten schreiben
    char unknown[2] = {0x01, 0x00};
    fs.write(unknown, sizeof(unknown));

    // Länge schreiben
    unsigned int length = width_ * height_;
    fs << length;

    for(unsigned short y = 0; y < height_; ++y)
    {
        for(unsigned short x = 0; x < width_; ++x)
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
    unsigned char unknown2[8] = {0x00, 0x00, 0x02, 0x01, 0xF4, 0x06, 0x70, 0x00};
    fs.write(unknown2, sizeof(unknown2));

    return 0;
}
