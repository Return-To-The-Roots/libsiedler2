// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "ArchivItem_Bitmap_Raw.h"
#include "ErrorCodes.h"
#include "PixelBufferPaletted.h"
#include "libendian/EndianIStreamAdapter.h"
#include "libendian/EndianOStreamAdapter.h"
#include <iostream>
#include <vector>
namespace libsiedler2 {
class ArchivItem_Palette;
}
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
    bobtype_ = BobType::Bitmap;
}

libsiedler2::baseArchivItem_Bitmap_Raw::baseArchivItem_Bitmap_Raw(const baseArchivItem_Bitmap_Raw& item)
    : ArchivItem_BitmapBase(item), baseArchivItem_Bitmap(item)
{
    bobtype_ = BobType::Bitmap;
}

libsiedler2::baseArchivItem_Bitmap_Raw::~baseArchivItem_Bitmap_Raw() = default;

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
        return ErrorCode::FILE_NOT_ACCESSIBLE;
    if(palette == nullptr)
        return ErrorCode::PALETTE_MISSING;

    libendian::EndianIStreamAdapter<false, std::istream&> fs(file);
    uint16_t unknown1;
    uint32_t length;
    fs >> unknown1 >> length;
    if(unknown1 != 1)
        return ErrorCode::WRONG_HEADER;

    std::vector<uint8_t> data(length);
    uint16_t width, height;
    // Daten einlesen
    fs >> data >> nx_ >> ny_ >> width >> height;

    if(length != static_cast<uint32_t>(width * height))
        return ErrorCode::WRONG_FORMAT;

    TextureFormat outFormat = getWantedFormat(TextureFormat::Paletted);
    // Speicher anlegen
    if(length > 0)
    {
        int ec = create(&data[0], width, height, TextureFormat::Paletted, palette);
        if(ec)
            return ec;
        ec = convertFormat(outFormat);
        if(ec)
            return ec;
        if(getFormat() == TextureFormat::BGRA)
            removePalette();
    } else
        init(0, 0, outFormat, outFormat == TextureFormat::Paletted ? palette : nullptr);

    // Unbekannte Daten überspringen
    fs.ignore(8);

    return (!fs) ? ErrorCode::UNEXPECTED_EOF : ErrorCode::NONE;
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
        return ErrorCode::FILE_NOT_ACCESSIBLE;
    if(palette == nullptr)
        palette = getPalette();

    const uint16_t width = getWidth(), height = getHeight();

    libendian::EndianOStreamAdapter<false, std::ostream&> fs(file);
    PixelBufferPaletted buffer(width, height);
    int ec = print(buffer, palette);
    if(ec)
        return ec;

    std::array<uint8_t, 8> unknown2 = {0x00, 0x00, 0x02, 0x01, 0xF4, 0x06, 0x70, 0x00};
    fs << uint16_t(1) << uint32_t(buffer.getSizeInBytes()) << buffer.getPixels() << nx_ << ny_ << width << height
       << unknown2;

    return (!fs) ? ErrorCode::UNEXPECTED_EOF : ErrorCode::NONE;
}
