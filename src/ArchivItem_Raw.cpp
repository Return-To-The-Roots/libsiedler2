// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "ArchivItem_Raw.h"
#include "ErrorCodes.h"
#include "libendian/EndianIStreamAdapter.h"
#include "libendian/EndianOStreamAdapter.h"
#include <iostream>
#include <utility>

namespace libsiedler2 {
/** @class baseArchivItem_Raw
 *
 *  Basisklasse für Rawdaten.
 */

ArchivItem_Raw::ArchivItem_Raw() : ArchivItem(BobType::Raw) {}

ArchivItem_Raw::ArchivItem_Raw(std::vector<uint8_t> initialData)
    : ArchivItem(BobType::Raw), data(std::move(initialData))
{}

/**
 *  lädt die Rawdaten aus einer Datei.
 *
 *  @param[in] file   Dateihandle der Datei
 *  @param[in] length Länge des Blocks (aus Datei lesen = 0xFFFFFFFF)
 *
 *  @return liefert Null bei Erfolg, ungleich Null bei Fehler
 */
int ArchivItem_Raw::load(std::istream& file, uint32_t length)
{
    if(!file)
        return ErrorCode::FILE_NOT_ACCESSIBLE;

    clear();

    libendian::EndianIStreamAdapter<false, std::istream&> fs(file);
    if(length == 0xFFFFFFFF)
    {
        if(!(fs >> length))
            return ErrorCode::UNEXPECTED_EOF;
    }

    data.resize(length);
    fs >> data;

    return (!file) ? ErrorCode::UNEXPECTED_EOF : ErrorCode::NONE;
}

/**
 *  schreibt die Rawdaten in eine Datei.
 *
 *  @param[in] file        Dateihandle der Datei
 *  @param[in] with_length Soll die Blocklänge mitgeschrieben werden?
 *
 *  @return liefert Null bei Erfolg, ungleich Null bei Fehler
 */
int ArchivItem_Raw::write(std::ostream& file, bool with_length) const
{
    if(!file)
        return ErrorCode::FILE_NOT_ACCESSIBLE;

    libendian::EndianOStreamAdapter<false, std::ostream&> fs(file);
    if(with_length)
    {
        // Convert to uint32_t first
        fs << static_cast<uint32_t>(data.size());
    }

    fs << data;

    return (!file) ? ErrorCode::UNEXPECTED_EOF : ErrorCode::NONE;
}

/**
 *  liefert die Daten zurück (ro).
 */
const std::vector<uint8_t>& ArchivItem_Raw::getData() const
{
    return data;
}

/**
 *  liefert die Daten zurück (rw).
 */
std::vector<uint8_t>& ArchivItem_Raw::getData()
{
    return data;
}

/**
 *  löscht den Datenblock.
 */
void ArchivItem_Raw::clear()
{
    data.clear();
}
} // namespace libsiedler2
