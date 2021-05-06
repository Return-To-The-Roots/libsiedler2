// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "ArchivItem_Bitmap_RLE.h"
#include "ArchivItem_Palette.h"
#include "ErrorCodes.h"
#include "PixelBufferPaletted.h"
#include "libendian/EndianIStreamAdapter.h"
#include "libendian/EndianOStreamAdapter.h"
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
    bobtype_ = BobType::BitmapRLE;
}

libsiedler2::baseArchivItem_Bitmap_RLE::baseArchivItem_Bitmap_RLE(const baseArchivItem_Bitmap_RLE& item)
    : ArchivItem_BitmapBase(item), baseArchivItem_Bitmap(item)
{
    bobtype_ = BobType::BitmapRLE;
}

libsiedler2::baseArchivItem_Bitmap_RLE::~baseArchivItem_Bitmap_RLE() = default;

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
        return ErrorCode::FILE_NOT_ACCESSIBLE;
    if(palette == nullptr)
        return ErrorCode::PALETTE_MISSING;

    clear();

    libendian::EndianIStreamAdapter<false, std::istream&> fs(file);
    uint16_t width, height, unknown2;
    uint32_t unknown1, length;

    fs >> nx_ >> ny_ >> unknown1 >> width >> height >> unknown2 >> length;

    if(!fs)
        return ErrorCode::UNEXPECTED_EOF;
    if(unknown1 != 0 || unknown2 != 1)
        return ErrorCode::WRONG_HEADER;

    std::vector<uint8_t> data(length);
    // Daten einlesen
    fs >> data;

    // Speicher anlegen
    init(width, height, getWantedFormat(TextureFormat::Paletted), palette);

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
                if(position + count + 1 >= data.size())
                    return ErrorCode::WRONG_FORMAT;
                for(uint8_t i = 0; i < count; ++i, ++x)
                    setPixel(x, y, data[position++]);

                // transparente Pixel setzen
                count = data[position++];
                x += count;
            }

            if(position >= data.size())
                return ErrorCode::WRONG_FORMAT;
            // FF überspringen
            assert(data[position] == 0xFF);
            ++position;
        }

        if(position >= data.size())
            return ErrorCode::WRONG_FORMAT;
        // FF überspringen
        assert(data[position] == 0xFF);
        ++position;

        if(position != length)
            return ErrorCode::WRONG_FORMAT;
    }
    if(getFormat() == TextureFormat::BGRA)
        removePalette();

    return (!fs) ? ErrorCode::UNEXPECTED_EOF : ErrorCode::NONE;
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
        return ErrorCode::FILE_NOT_ACCESSIBLE;
    if(palette == nullptr)
        palette = getPalette();
    if(palette == nullptr)
        return ErrorCode::PALETTE_MISSING;

    libendian::EndianOStreamAdapter<false, std::ostream&> fs(file);
    std::array<char, 4> unknown = {0x00, 0x00, 0x00, 0x00};
    std::array<char, 2> unknown2 = {0x01, 0x00};
    const uint16_t width = getWidth(), height = getHeight();

    fs << nx_ << ny_ << unknown << width << height << unknown2;

    PixelBufferPaletted buffer(width, height, palette->getTransparentIdx());
    int ec = print(buffer, palette);
    if(ec)
        return ec;
    // Maximum size: 1-2 bytes per pixel + 1 byte FF per row + 1 byte FF end
    std::vector<uint8_t> image(width * height * 2 + height + 1);

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
                color = buffer.get(x + count, y);
                if(palette->isTransparent(color))
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
                color = buffer.get(x + count, y);
                if(!palette->isTransparent(color) || count == 0xFF)
                    break;
            }
            image[position++] = (uint8_t)count;

            x += count;
        }

        image[position++] = 0xFF;
    }
    image[position++] = 0xFF;
    image.resize(position);

    uint32_t length = position + height * 2;

    // Länge schreiben
    fs << length;

    // Daten schreiben
    fs << starts << image;

    return (!fs) ? ErrorCode::UNEXPECTED_EOF : ErrorCode::NONE;
}
