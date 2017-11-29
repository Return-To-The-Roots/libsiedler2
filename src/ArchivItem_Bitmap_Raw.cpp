// Copyright (c) 2005 - 2017 Settlers Freaks (sf-team at siedler25.org)
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
#include "ArchivItem_Palette.h"
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
    bobtype_ = BOBTYPE_BITMAP_RAW;
}

libsiedler2::baseArchivItem_Bitmap_Raw::baseArchivItem_Bitmap_Raw(const baseArchivItem_Bitmap_Raw& item) : baseArchivItem_Bitmap(item)
{
    bobtype_ = BOBTYPE_BITMAP_RAW;
}

libsiedler2::baseArchivItem_Bitmap_Raw::~baseArchivItem_Bitmap_Raw() {}

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
    if(palette == NULL)
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

    TextureFormat outFormat = getWantedFormat(FORMAT_PALETTED);
    // Speicher anlegen
    if(length > 0)
    {
        int ec = create(&data[0], width, height, FORMAT_PALETTED, palette);
        if(ec)
            return ec;
        ec = convertFormat(outFormat);
        if(ec)
            return ec;
        if(getFormat() == FORMAT_BGRA)
            removePalette();
    } else
        init(0, 0, outFormat, outFormat == FORMAT_PALETTED ? palette : NULL);

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
    if(palette == NULL)
        palette = getPalette();

    const uint16_t width = getWidth(), height = getHeight();

    libendian::EndianOStreamAdapter<false, std::ostream&> fs(file);
    PixelBufferPaletted buffer(width, height);
    int ec = print(buffer, palette);
    if(ec)
        return ec;

    uint8_t unknown2[8] = {0x00, 0x00, 0x02, 0x01, 0xF4, 0x06, 0x70, 0x00};
    fs << uint16_t(1) << uint32_t(buffer.getSize()) << buffer.getPixels() << nx_ << ny_ << width << height << unknown2;

    return (!fs) ? ErrorCode::UNEXPECTED_EOF : ErrorCode::NONE;
}
