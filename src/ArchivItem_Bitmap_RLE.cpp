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
#include "ArchivItem_Bitmap_RLE.h"
#include <fstream>
#include <EndianStream.h>
#include <vector>

// Include last!
#include "DebugNew.h"

///////////////////////////////////////////////////////////////////////////////
/** @class libsiedler2::baseArchivItem_Bitmap_RLE
 *
 *  Basisklasse für RLE-Bitmaps.
 *
 *  @author FloSoft
 */

///////////////////////////////////////////////////////////////////////////////
/** @class libsiedler2::ArchivItem_Bitmap_RLE
 *
 *  Klasse für RLE-Bitmaps.
 *
 *  @author FloSoft
 */

///////////////////////////////////////////////////////////////////////////////
/**
 *  Konstruktor von @p baseArchivItem_Bitmap_RLE.
 *
 *  @author FloSoft
 */
libsiedler2::baseArchivItem_Bitmap_RLE::baseArchivItem_Bitmap_RLE(void) : baseArchivItem_Bitmap()
{
    setBobType(BOBTYPE_BITMAP_RLE);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Kopierkonstruktor von @p baseArchivItem_Bitmap_RLE.
 *
 *  @param[in] item Quellitem
 *
 *  @author FloSoft
 */
libsiedler2::baseArchivItem_Bitmap_RLE::baseArchivItem_Bitmap_RLE(const baseArchivItem_Bitmap_RLE& item) : baseArchivItem_Bitmap(item)
{
    setBobType(BOBTYPE_BITMAP_RLE);
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
libsiedler2::baseArchivItem_Bitmap_RLE::baseArchivItem_Bitmap_RLE(std::istream& file, const ArchivItem_Palette* palette) : baseArchivItem_Bitmap()
{
    setBobType(BOBTYPE_BITMAP_RLE);
    load(file, palette);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Destruktor von @p baseArchivItem_Bitmap_RLE.
 *
 *  @author FloSoft
 */
libsiedler2::baseArchivItem_Bitmap_RLE::~baseArchivItem_Bitmap_RLE(void)
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
int libsiedler2::baseArchivItem_Bitmap_RLE::load(std::istream& file, const ArchivItem_Palette* palette)
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
    unsigned length;
    fs >> length;

    std::vector<unsigned char> data(length);
    // Daten einlesen
    fs >> data;

    // Speicher anlegen
    tex_alloc();

    if(length != 0)
    {
        size_t position = height_ * 2;

        // Einlesen
        for(unsigned short y = 0; y < height_; ++y)
        {
            unsigned short x = 0;

            // Solange Zeile einlesen, bis x voll ist
            while(x < width_)
            {
                // farbige Pixel setzen
                unsigned char count = data[position++];
                for(unsigned char i = 0; i < count; ++i, ++x)
                    tex_setPixel(x, y, data[position++], palette);

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

        if(position != length)
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
 *  @bug Bei zu großen Bilddaten gibts einen Überlauf der Zeilenstartadressen,
 *       im Moment wird dann der Zeilenstart auf 0xFFFF gesetzt.
 *
 *  @author FloSoft
 */
int libsiedler2::baseArchivItem_Bitmap_RLE::write(std::ostream& file, const ArchivItem_Palette* palette) const
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

    // RLE kodieren
    unsigned int position = 0;
    for(unsigned short y = 0; y < height_; ++y)
    {
        unsigned short x = 0;

        // Startadresse setzen
        if((unsigned short)(position + height_ * 2) < (position + height_ * 2))
            starts[y] = 0xFFFF;
        else
            starts[y] = (unsigned short)(position + height_ * 2);

        // Solange Zeile nicht voll
        while(x < width_)
        {
            unsigned short count;
            unsigned char color;

            // farbige Pixel schreiben
            for(count = 0; count < width_ - x; ++count)
            {
                color = tex_getPixel(x + count, y, palette);
                if(color == TRANSPARENT_INDEX)
                    break;
                image[position + 1 + count] = color;
                if(count == 0x7F)
                    break;
            }
            image[position] = (unsigned char)count;
            position += 1 + count;

            x += count;

            // transparente Pixel schreiben
            for(count = 0; count < width_ - x; ++count)
            {
                color = tex_getPixel(x + count, y, palette);
                if(color != TRANSPARENT_INDEX || count == 0xFF)
                    break;
            }
            image[position++] = (unsigned char)count;

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
