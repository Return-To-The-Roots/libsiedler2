// $Id: ArchivItem_Bitmap_Raw.cpp 9359 2014-04-25 15:37:22Z FloSoft $
//
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
libsiedler2::baseArchivItem_Bitmap_Raw::baseArchivItem_Bitmap_Raw(FILE* file, const ArchivItem_Palette* palette) : baseArchivItem_Bitmap()
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
int libsiedler2::baseArchivItem_Bitmap_Raw::load(FILE* file, const ArchivItem_Palette* palette)
{
    unsigned char* data = NULL;

    if(file == NULL)
        return 1;
    if(palette == NULL)
        palette = this->palette;
    if(palette == NULL)
        return 2;

    tex_clear();

    // Unbekannte Daten überspringen
    fseek(file, 2, SEEK_CUR);

    // Länge einlesen
    if(libendian::le_read_ui(&length, file) != 0)
        return 2;

    // Daten einlesen
    if(length != 0)
    {
        data = new unsigned char[length];
        if(libendian::le_read_uc(data, length, file) != (int)length)
            return 3;
    }

    // Nullpunkt X einlesen
    if(libendian::le_read_s(&nx, file) != 0)
        return 4;

    // Nullpunkt Y einlesen
    if(libendian::le_read_s(&ny, file) != 0)
        return 5;

    // Breite einlesen
    if(libendian::le_read_us(&width, file) != 0)
        return 6;

    // Höhe einlesen
    if(libendian::le_read_us(&height, file) != 0)
        return 7;

    // Speicher anlegen
    tex_alloc();

    if(length != 0 && data)
    {
        for(unsigned short y = 0; y < height; ++y)
        {
            for(unsigned short x = 0; x < width; ++x)
            {
                // Pixel setzen
                tex_setPixel(x, y, data[y * width + x], palette);
            }
        }

        delete[] data;
    }

    // Unbekannte Daten überspringen
    fseek(file, 8, SEEK_CUR);

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
int libsiedler2::baseArchivItem_Bitmap_Raw::write(FILE* file, const ArchivItem_Palette* palette) const
{
    if(file == NULL)
        return 1;
    if(palette == NULL)
        palette = this->palette;
    if(palette == NULL)
        return 2;

    if(width == 0 || height == 0)
        return 2;

    // Unbekannte Daten schreiben
    char unknown[2] = {0x01, 0x00};
    if(libendian::le_write_c(unknown, 2, file) != 2)
        return 3;

    // Länge schreiben
    unsigned int length = width * height;
    if(libendian::le_write_ui(length, file) != 0)
        return 4;

    for(unsigned short y = 0; y < height; ++y)
    {
        for(unsigned short x = 0; x < width; ++x)
        {
            // Pixel holen und schreiben
            unsigned char color = tex_getPixel(x, y, palette);

            if(libendian::le_write_uc(&color, 1, file) != 1)
                return 5;
        }
    }

    // Nullpunkt X schreiben
    if(libendian::le_write_s(nx, file) != 0)
        return 6;

    // Nullpunkt Y schreiben
    if(libendian::le_write_s(ny, file) != 0)
        return 7;

    // Breite schreiben
    if(libendian::le_write_us(width, file) != 0)
        return 8;

    // Höhe schreiben
    if(libendian::le_write_us(height, file) != 0)
        return 9;

    // Unbekannte Daten schreiben
    unsigned char unknown2[8] = {0x00, 0x00, 0x02, 0x01, 0xF4, 0x06, 0x70, 0x00};
    if(libendian::le_write_uc(unknown2, 8, file) != 8)
        return 10;

    return 0;
}
