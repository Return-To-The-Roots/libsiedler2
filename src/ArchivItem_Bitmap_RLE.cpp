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
#include "ArchivItem_Bitmap_RLE.h"
#include "ArchivItem_Palette.h"
#include "libsiedler2.h"
#include "libendian/src/EndianIStreamAdapter.h"
#include "libendian/src/EndianOStreamAdapter.h"
#include <iostream>
#include <vector>

/** @class libsiedler2::baseArchivItem_Bitmap_RLE
 *
 *  Basisklasse für RLE-Bitmaps.
 */

/** @class libsiedler2::ArchivItem_Bitmap_RLE
 *
 *  Klasse für RLE-Bitmaps.
 */

libsiedler2::baseArchivItem_Bitmap_RLE::baseArchivItem_Bitmap_RLE()
{
    setBobType(BOBTYPE_BITMAP_RLE);
}

libsiedler2::baseArchivItem_Bitmap_RLE::baseArchivItem_Bitmap_RLE(const baseArchivItem_Bitmap_RLE& item) : baseArchivItem_Bitmap(item)
{
    setBobType(BOBTYPE_BITMAP_RLE);
}

/**
 *  Konstruktor von @p baseArchivItem_Bitmap_Raw mit Laden der Bilddaten aus
 *  einer Datei.
 *
 *  @param[in] file    Dateihandle der Datei
 *  @param[in] palette Grundpalette
 */
libsiedler2::baseArchivItem_Bitmap_RLE::baseArchivItem_Bitmap_RLE(std::istream& file, const ArchivItem_Palette* palette) : baseArchivItem_Bitmap()
{
    setBobType(BOBTYPE_BITMAP_RLE);
    load(file, palette);
}

libsiedler2::baseArchivItem_Bitmap_RLE::~baseArchivItem_Bitmap_RLE()
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
int libsiedler2::baseArchivItem_Bitmap_RLE::load(std::istream& file, const ArchivItem_Palette* palette)
{
    if(!file)
        return 1;
    if(palette == NULL)
        palette = getPalette();
    else
        setPalette(*palette);
    if(palette == NULL)
        return 2;

    tex_clear();

    libendian::EndianIStreamAdapter<false, std::istream&> fs(file);
    uint16_t width, height;
    char unknown1[4], unknown2[2];
    uint32_t length;

    fs >> nx_ >> ny_ >> unknown1 >> width >> height >> unknown2 >> length;

    if(!fs)
        return 3;

    std::vector<uint8_t> data(length);
    // Daten einlesen
    fs >> data;

    // Speicher anlegen
    tex_alloc(width, height, getTextureFormat());

    if(length != 0)
    {
        size_t position = height * 2;

        // Einlesen
        for(uint16_t y = 0; y < height; ++y)
        {
            uint16_t x = 0;

            // Solange Zeile einlesen, bis x voll ist
            while(x < width)
            {
                // farbige Pixel setzen
                uint8_t count = data[position++];
                for(uint8_t i = 0; i < count; ++i, ++x)
                    tex_setPixel(x, y, data[position++]);

                // transparente Pixel setzen
                count = data[position++];
                for(uint8_t i = 0; i < count; ++i, ++x)
                    tex_setPixel(x, y, TRANSPARENT_INDEX);
            }

            // FF überspringen
            ++position;
        }

        // FF überspringen
        ++position;

        if(position != length)
            return 8;
    }

    return (!fs) ? 99 : 0;
}

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
 */
int libsiedler2::baseArchivItem_Bitmap_RLE::write(std::ostream& file, const ArchivItem_Palette* palette) const
{
    if(!file)
        return 1;
    if(palette == NULL)
        palette = getPalette();
    if(palette == NULL)
        return 2;

    libendian::EndianOStreamAdapter<false, std::ostream&> fs(file);
    char unknown[4] = { 0x00, 0x00, 0x00, 0x00 };
    char unknown2[2] = { 0x01, 0x00 };
    const uint16_t width = getWidth(), height = getHeight();

    fs << nx_ << ny_ << unknown << width << height << unknown2;

    // maximale größe von RLE: width*height*2
    std::vector<uint8_t> image(width * height * 2);

    // Startadressen
    std::vector<uint16_t> starts(height);

    // RLE kodieren
    uint32_t position = 0;
    for(uint16_t y = 0; y < height; ++y)
    {
        uint16_t x = 0;

        // Startadresse setzen
        if((uint16_t)(position + height * 2) < (position + height * 2))
            starts[y] = 0xFFFF;
        else
            starts[y] = (uint16_t)(position + height * 2);

        // Solange Zeile nicht voll
        while(x < width)
        {
            uint16_t count;
            uint8_t color;

            // farbige Pixel schreiben
            for(count = 0; count < width - x; ++count)
            {
                color = tex_getPixel(x + count, y, palette);
                if(color == TRANSPARENT_INDEX)
                    break;
                image[position + 1 + count] = color;
                if(count == 0x7F)
                    break;
            }
            image[position] = (uint8_t)count;
            position += 1 + count;

            x += count;

            // transparente Pixel schreiben
            for(count = 0; count < width - x; ++count)
            {
                color = tex_getPixel(x + count, y, palette);
                if(color != TRANSPARENT_INDEX || count == 0xFF)
                    break;
            }
            image[position++] = (uint8_t)count;

            x += count;
        }

        image[position++] = 0xFF;
    }
    image[position++] = 0xFF;

    uint32_t length = position + height * 2;

    // Länge schreiben
    fs << length;

    // Daten schreiben
    fs << starts << image;

    return (!fs) ? 99 : 0;
}
