// Copyright (c) 2005 - 2020 Settlers Freaks (sf-team at siedler25.org)
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

#include "ArchivItem_Font.h"
#include "ErrorCodes.h"
#include "prototypen.h"
#include "libendian/EndianIStreamAdapter.h"
#include "libendian/EndianOStreamAdapter.h"
#include <iostream>
#include <sstream>
#include <stdexcept>

/** @class libsiedler2::ArchivItem_Font
 *
 *  Klasse für Fonts.
 */

/** @var libsiedler2::ArchivItem_Font::dx
 *
 *  X-Buchstabenabstand.
 */

/** @var libsiedler2::ArchivItem_Font::dy
 *
 *  Y-Buchstabenabstand.
 */

libsiedler2::ArchivItem_Font::ArchivItem_Font() : ArchivItem(BOBTYPE_FONT), isUnicode(false), dx(0), dy(0) {}

/**
 *  lädt die Fontdaten aus einer Datei.
 *
 *  @param[in] file    Dateihandle der Datei
 *  @param[in] palette Grundpalette
 *
 *  @return liefert Null bei Erfolg, ungleich Null bei Fehler
 */
int libsiedler2::ArchivItem_Font::load(std::istream& file, const ArchivItem_Palette* palette)
{
    if(!file)
        return ErrorCode::FILE_NOT_ACCESSIBLE;
    if(!palette)
        return ErrorCode::PALETTE_MISSING;

    libendian::EndianIStreamAdapter<false, std::istream&> fs(file);
    // Spacing einlesen
    if(!(fs >> dx >> dy))
        return ErrorCode::UNEXPECTED_EOF;

    isUnicode = (dx == 255 && dy == 255);
    uint32_t numChars;
    if(isUnicode)
    {
        fs >> numChars >> dx >> dy;
        if(!fs)
            return ErrorCode::UNEXPECTED_EOF;
    } else
        numChars = 256;

    // Speicher für Buchstaben alloziieren
    alloc(numChars);

    // Buchstaben einlesen
    for(uint32_t i = 32; i < numChars; ++i)
    {
        int16_t bobtype_s;

        // bobtype des Items einlesen
        if(!(fs >> bobtype_s))
            return ErrorCode::UNEXPECTED_EOF;
        auto bobtype = static_cast<BobType>(bobtype_s);

        if(bobtype == BOBTYPE_NONE)
            continue;

        // Daten von Item auswerten
        std::unique_ptr<ArchivItem> item;
        int ec = loader::LoadType(bobtype, file, item, palette);
        if(ec)
            return ec;
        std::stringstream name;
        name << "U+" << std::hex << i;
        item->setName(name.str());
        set(i, std::move(item));
    }

    return (!file) ? ErrorCode::UNEXPECTED_EOF : ErrorCode::NONE;
}

/**
 *  schreibt die Fontdaten in eine Datei.
 *
 *  @param[in] file    Dateihandle der Datei
 *  @param[in] palette Grundpalette
 *
 *  @return liefert Null bei Erfolg, ungleich Null bei Fehler
 */
int libsiedler2::ArchivItem_Font::write(std::ostream& file, const ArchivItem_Palette* palette) const
{
    if(!file)
        return ErrorCode::FILE_NOT_ACCESSIBLE;
    if(!palette)
        return ErrorCode::PALETTE_MISSING;

    size_t numChars = size();
    if(numChars > 256 && !isUnicode)
        throw std::runtime_error("Trying to save a non-unicode font with more than 256 glyphs");

    libendian::EndianOStreamAdapter<false, std::ostream&> fs(file);

    if(isUnicode)
        fs << uint16_t(0xFFFF) << static_cast<uint32_t>(size());
    else
        numChars = 256;
    // Spacing schreiben
    fs << dx << dy;

    // Buchstaben schreiben
    for(size_t i = 32; i < numChars; ++i)
    {
        const ArchivItem* item = get(i);
        BobType bobtype = BOBTYPE_NONE;

        if(item)
            bobtype = item->getBobType();

        // bobtype des Items schreiben
        fs << (int16_t)bobtype;

        if(item == nullptr)
            continue;

        // Daten von Item auswerten
        if(int ec = loader::WriteType(bobtype, file, *item, palette))
            return ec;
    }

    return (!file) ? ErrorCode::UNEXPECTED_EOF : ErrorCode::NONE;
}
