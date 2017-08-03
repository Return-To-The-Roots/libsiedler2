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
#include "ArchivItem_Bitmap_Shadow.h"
#include "ArchivItem_Palette.h"
#include "ErrorCodes.h"
#include "libendian/src/EndianIStreamAdapter.h"
#include "libendian/src/EndianOStreamAdapter.h"
#include <iostream>
#include <vector>

/** @class libsiedler2::baseArchivItem_Bitmap_Shadow
 *
 *  Basisklasse für Shadow-Bitmaps.
 */

/** @class libsiedler2::ArchivItem_Bitmap_Shadow
 *
 *  Klasse für Shadow-Bitmaps.
 */

libsiedler2::baseArchivItem_Bitmap_Shadow::baseArchivItem_Bitmap_Shadow()
{
    bobtype_ = BOBTYPE_BITMAP_SHADOW;
}

libsiedler2::baseArchivItem_Bitmap_Shadow::baseArchivItem_Bitmap_Shadow(const baseArchivItem_Bitmap_Shadow& item) : baseArchivItem_Bitmap(item)
{
    bobtype_ = BOBTYPE_BITMAP_SHADOW;
}

/**
 *  Konstruktor von @p baseArchivItem_Bitmap_Raw mit Laden der Bilddaten aus
 *  einer Datei.
 *
 *  @param[in] file    Dateihandle der Datei
 *  @param[in] palette Grundpalette
 */
libsiedler2::baseArchivItem_Bitmap_Shadow::baseArchivItem_Bitmap_Shadow(std::istream& file, const ArchivItem_Palette* palette) : baseArchivItem_Bitmap()
{
    bobtype_ = BOBTYPE_BITMAP_SHADOW;
    load(file, palette);
}

libsiedler2::baseArchivItem_Bitmap_Shadow::~baseArchivItem_Bitmap_Shadow()
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
int libsiedler2::baseArchivItem_Bitmap_Shadow::load(std::istream& file, const ArchivItem_Palette* palette)
{
    if(!file)
        return ErrorCode::FILE_NOT_ACCESSIBLE;
    if(palette == NULL)
        palette = getPalette();
    if(palette == NULL)
        return ErrorCode::PALETTE_MISSING;

    tex_clear();

    libendian::EndianIStreamAdapter<false, std::istream&> fs(file);
    uint16_t width, height, unknown2;
    uint32_t unknown1, length;

    fs >> nx_ >> ny_ >> unknown1 >> width >> height >> unknown2 >> length;

    if(!fs || unknown1 != 0 || unknown2 != 1)
        return ErrorCode::WRONG_HEADER;

    std::vector<uint8_t> data(length);
    // Daten einlesen
    fs >> data;

    // Speicher anlegen
    tex_alloc(width, height, FORMAT_PALETTED);

    uint8_t gray = palette->lookup(ColorRGB(255, 255, 255));

    if(length != 0)
    {
        uint32_t position = height * 2;
        std::vector<uint8_t> buffer(width * height);
        unsigned bufPos = 0;

        // Einlesen
        for(uint16_t y = 0; y < height; ++y, bufPos += width)
        {
            uint16_t x = 0;

            // Solange Zeile einlesen, bis x voll ist
            while(x < width)
            {
                // graue Pixel setzen
                uint8_t count = data[position++];
                for(uint8_t i = 0; i < count; ++i, ++x)
                    buffer[bufPos + x] = gray;

                // transparente Pixel setzen
                count = data[position++];
                for(uint8_t i = 0; i < count; ++i, ++x)
                    buffer[bufPos + x] = TRANSPARENT_INDEX;
            }

            // FF überspringen
            ++position;
        }

        // FF überspringen
        ++position;

        if(position != length )
            return ErrorCode::WRONG_FORMAT;
        int ec = create(width, height, &buffer[0], width, height, FORMAT_PALETTED, palette);
        if(ec)
            return ec;
    }

    return ErrorCode::NONE;
}

/**
 *  schreibt die Bilddaten in eine Datei.
 *
 *  @param[in] file    Dateihandle der Datei
 *  @param[in] palette Grundpalette
 *
 *  @return liefert Null bei Erfolg, ungleich Null bei Fehler
 */
int libsiedler2::baseArchivItem_Bitmap_Shadow::write(std::ostream& file, const ArchivItem_Palette* palette) const
{
    if(!file)
        return ErrorCode::FILE_NOT_ACCESSIBLE;
    if(palette == NULL)
        palette = this->getPalette();
    assert(palette);

    libendian::EndianOStreamAdapter<false, std::ostream&> fs(file);
    const uint16_t width = getWidth(), height = getHeight();

    fs << nx_ << ny_ << uint32_t(0) << width << height << uint16_t(1);

    // maximale größe von RLE: width*height*2
    std::vector<uint8_t> image(width * height * 2);

    // Startadressen
    std::vector<uint16_t> starts(height);

    // Schattendaten kodieren
    uint16_t position = 0;
    for(uint16_t y = 0; y < height; ++y)
    {
        uint16_t x = 0;

        // Startadresse setzen
        starts[y] = position + height * 2;

        // Solange Zeile nicht voll
        while(x < width)
        {
            // graue Pixel schreiben
            uint8_t count, color;
            for(count = 0; x + count < width; ++count)
            {
                color = getPixelClrIdx(x + count, y, palette);
                if(color == TRANSPARENT_INDEX)
                    break;
            }
            image[position++] = count;

            x += count;

            // transparente Pixel schreiben
            for(count = 0; x + count < width; ++count)
            {
                color = getPixelClrIdx(x + count, y, palette);
                if(color != TRANSPARENT_INDEX)
                    break;
            }
            image[position++] = count;

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

    return (!fs) ? ErrorCode::UNEXPECTED_EOF : ErrorCode::NONE;
}
