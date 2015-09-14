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
#include "ArchivItem_Bitmap_Shadow.h"
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
/** @class libsiedler2::baseArchivItem_Bitmap_Shadow
 *
 *  Basisklasse für Shadow-Bitmaps.
 *
 *  @author FloSoft
 */

///////////////////////////////////////////////////////////////////////////////
/** @class libsiedler2::ArchivItem_Bitmap_Shadow
 *
 *  Klasse für Shadow-Bitmaps.
 *
 *  @author FloSoft
 */

///////////////////////////////////////////////////////////////////////////////
/**
 *  Konstruktor von @p baseArchivItem_Bitmap_Shadow.
 *
 *  @author FloSoft
 */
libsiedler2::baseArchivItem_Bitmap_Shadow::baseArchivItem_Bitmap_Shadow(void) : baseArchivItem_Bitmap()
{
    setBobType(BOBTYPE_BITMAP_SHADOW);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Kopierkonstruktor von @p baseArchivItem_Bitmap_Shadow.
 *
 *  @param[in] item Quellitem
 *
 *  @author FloSoft
 */
libsiedler2::baseArchivItem_Bitmap_Shadow::baseArchivItem_Bitmap_Shadow(const baseArchivItem_Bitmap_Shadow& item) : baseArchivItem_Bitmap(item)
{
    setBobType(BOBTYPE_BITMAP_SHADOW);
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
libsiedler2::baseArchivItem_Bitmap_Shadow::baseArchivItem_Bitmap_Shadow(std::istream& file, const ArchivItem_Palette* palette) : baseArchivItem_Bitmap()
{
    setBobType(BOBTYPE_BITMAP_SHADOW);
    load(file, palette);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Destruktor von @p baseArchivItem_Bitmap_Shadow.
 *
 *  @author FloSoft
 */
libsiedler2::baseArchivItem_Bitmap_Shadow::~baseArchivItem_Bitmap_Shadow(void)
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
int libsiedler2::baseArchivItem_Bitmap_Shadow::load(std::istream& file, const ArchivItem_Palette* palette)
{
    if(!file)
        return 1;
    if(palette == NULL)
        palette = this->palette_;
    if(palette == NULL)
        return 2;

    tex_clear();

    libendian::LittleEndianIStreamRef fs(file);
    // Nullpunkt X einlesen
    fs >> nx_;

    // Nullpunkt Y einlesen
    fs >> ny_;

    // Unbekannte Daten überspringen
    fs.ignore(4);

    // Breite einlesen
    fs >> width_;

    // Höhe einlesen
    fs >> height_;

    // Unbekannte Daten überspringen
    fs.ignore(2);

    // Länge einlesen
    unsigned int length;
    fs >> length;

    std::vector<unsigned char> data(length);
    // Daten einlesen
    fs >> data;

    // Speicher anlegen
    tex_alloc();

    unsigned char gray = palette->lookup(255, 255, 255);

    if(length != 0)
    {
        unsigned int position = height_ * 2;

        // Einlesen
        for(unsigned short y = 0; y < height_; ++y)
        {
            unsigned short x = 0;

            // Solange Zeile einlesen, bis x voll ist
            while(x < width_)
            {
                // graue Pixel setzen
                unsigned char count = data[position++];
                for(unsigned char i = 0; i < count; ++i, ++x)
                    tex_setPixel(x, y, gray, palette);

                // transparente Pixel setzen
                count = data[position++];
                for(unsigned char i = 0; i < count; ++i, ++x)
                    tex_setPixel(x, y, TRANSPARENT_INDEX, palette);
            }

            // FF überspringen
            ++position;
        }

        // FF überspringen
        ++position;

        if(position != length )
            return 8;
    }

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
int libsiedler2::baseArchivItem_Bitmap_Shadow::write(std::ostream& file, const ArchivItem_Palette* palette) const
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
    // Nullpunkt X schreiben
    fs << nx_;

    // Nullpunkt Y schreiben
    fs << ny_;

    // Unbekannte Daten schreiben
    char unknown[4] = {0x00, 0x00, 0x00, 0x00};
    fs.write(unknown, sizeof(unknown));

    // Breite schreiben
    fs << width_;

    // Höhe einlesen
    fs << height_;

    // Unbekannte Daten schreiben
    char unknown2[2] = {0x01, 0x00};
    fs.write(unknown2, sizeof(unknown2));

    // maximale größe von RLE: width*height*2
    std::vector<unsigned char> image(width_ * height_ * 2);

    // Startadressen
    std::vector<unsigned short> starts(height_);

    // Schattendaten kodieren
    unsigned short position = 0;
    for(unsigned short y = 0; y < height_; ++y)
    {
        unsigned short x = 0;

        // Startadresse setzen
        starts[y] = position + height_ * 2;

        // Solange Zeile nicht voll
        while(x < width_)
        {
            // graue Pixel schreiben
            unsigned char count, color;
            for(count = 0; count < width_ - x; ++count)
            {
                color = tex_getPixel(x + count, y, palette);
                if(color == TRANSPARENT_INDEX)
                    break;
            }
            image[position++] = count;

            x += count;

            // transparente Pixel schreiben
            for(count = 0; count < width_ - x; ++count)
            {
                color = tex_getPixel(x + count, y, palette);
                if(color != TRANSPARENT_INDEX)
                    break;
            }
            image[position++] = count;

            x += count;
        }

        image[position++] = 0xFF;
    }
    image[position++] = 0xFF;

    unsigned int length = position + height_ * 2;

    // Länge schreiben
    fs << length;

    // Daten schreiben
    fs << starts << image;

    return 0;
}
