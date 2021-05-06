// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "ArchivItem_Bitmap_Shadow.h"
#include "ArchivItem_Palette.h"
#include "ErrorCodes.h"
#include "PixelBufferPaletted.h"
#include "libendian/EndianIStreamAdapter.h"
#include "libendian/EndianOStreamAdapter.h"
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
    bobtype_ = BobType::BitmapShadow;
}

libsiedler2::baseArchivItem_Bitmap_Shadow::baseArchivItem_Bitmap_Shadow(const baseArchivItem_Bitmap_Shadow& item)
    : ArchivItem_BitmapBase(item), baseArchivItem_Bitmap(item)
{
    bobtype_ = BobType::BitmapShadow;
}

libsiedler2::baseArchivItem_Bitmap_Shadow::~baseArchivItem_Bitmap_Shadow() = default;

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
    if(palette == nullptr)
        return ErrorCode::PALETTE_MISSING;

    clear();

    libendian::EndianIStreamAdapter<false, std::istream&> fs(file);
    uint16_t width, height, unknown2;
    uint32_t unknown1, length;

    fs >> nx_ >> ny_ >> unknown1 >> width >> height >> unknown2 >> length;

    if(!fs || unknown1 != 0 || unknown2 != 1)
        return ErrorCode::WRONG_HEADER;

    std::vector<uint8_t> data(length);
    // Daten einlesen
    fs >> data;

    uint8_t gray = palette->lookup(ColorRGB(255, 255, 255));

    if(length == 0)
    {
        // Speicher anlegen
        init(width, height, TextureFormat::Paletted, palette);
    } else
    {
        uint32_t position = height * 2;
        PixelBufferPaletted buffer(width, height, palette->getTransparentIdx());

        // Einlesen
        for(uint16_t y = 0; y < height; ++y)
        {
            uint16_t x = 0;

            // Solange Zeile einlesen, bis x voll ist
            while(x < width && position + 2 < data.size())
            {
                // graue Pixel setzen
                uint8_t count = data[position++];
                for(uint8_t i = 0; i < count; ++i, ++x)
                    buffer.set(x, y, gray);

                // Transparent pixels
                count = data[position++];
                // Buffer is already transparent by default -> Just increase x
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
        int ec = create(buffer, palette);
        if(ec)
            return ec;
        ec = convertFormat(getWantedFormat(TextureFormat::Paletted));
        if(ec)
            return ec;
        if(getFormat() == TextureFormat::BGRA)
            removePalette();
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
    if(palette == nullptr)
        palette = this->getPalette();
    if(!palette)
        return ErrorCode::PALETTE_MISSING;

    libendian::EndianOStreamAdapter<false, std::ostream&> fs(file);
    const uint16_t width = getWidth(), height = getHeight();

    fs << nx_ << ny_ << uint32_t(0) << width << height << uint16_t(1);

    // Maximum size: 1 byte/pixel + (1 byte FF + 2 extra bytes for first transparent and last solid pixel) per row + 1
    // byte FF final
    std::vector<uint8_t> image(width * height + height * 3 + 1);

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
            uint8_t count;
            for(count = 0; x < width; ++count, ++x)
            {
                uint8_t color = getPixelClrIdx(x, y, palette);
                if(palette->isTransparent(color))
                    break;
            }
            image[position++] = count;

            // transparente Pixel schreiben
            for(count = 0; x < width; ++count, ++x)
            {
                uint8_t color = getPixelClrIdx(x, y, palette);
                if(!palette->isTransparent(color))
                    break;
            }
            image[position++] = count;
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
