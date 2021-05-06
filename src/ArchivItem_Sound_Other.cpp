// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "ArchivItem_Sound_Other.h"
#include "ErrorCodes.h"
#include "libendian/EndianIStreamAdapter.h"
#include "libendian/EndianOStreamAdapter.h"
#include <iostream>

namespace libsiedler2 {
/** @class baseArchivItem_Sound_Other
 *
 *  Klasse für Other-Sounds.
 */

ArchivItem_Sound_Other::ArchivItem_Sound_Other(SoundType sndType) : ArchivItem_Sound(sndType) {}

/**
 *  lädt die Daten aus einer Datei.
 *
 *  @param[in] file    Dateihandle der Datei
 *  @param[in] length  Länge der Daten
 *
 *  @return liefert Null bei Erfolg, ungleich Null bei Fehler
 */
int ArchivItem_Sound_Other::load(std::istream& file, uint32_t length)
{
    if(!file)
        return ErrorCode::FILE_NOT_ACCESSIBLE;
    if(length == 0)
        return ErrorCode::WRONG_HEADER;

    data.resize(length);
    libendian::EndianIStreamAdapter<false, std::istream&> fs(file);

    fs >> data;

    return (!file) ? ErrorCode::UNEXPECTED_EOF : ErrorCode::NONE;
}

/**
 *  schreibt die Daten in eine Datei.
 *
 *  @param[in] file        Dateihandle der Datei
 *
 *  @return liefert Null bei Erfolg, ungleich Null bei Fehler
 */
int ArchivItem_Sound_Other::write(std::ostream& file) const
{
    if(!file)
        return ErrorCode::FILE_NOT_ACCESSIBLE;

    libendian::EndianOStreamAdapter<false, std::ostream&> fs(file);
    fs << data;

    return (!file) ? ErrorCode::UNEXPECTED_EOF : ErrorCode::NONE;
}

/**
 *  räumt den Soundspeicher auf.
 */
void ArchivItem_Sound_Other::clear()
{
    data.resize(0);
}
} // namespace libsiedler2
