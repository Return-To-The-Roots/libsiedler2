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
#include "ArchivItem_Raw.h"
#include "ErrorCodes.h"
#include "libendian/src/EndianIStreamAdapter.h"
#include "libendian/src/EndianOStreamAdapter.h"
#include <iostream>

namespace libsiedler2 {
/** @class baseArchivItem_Raw
 *
 *  Basisklasse für Rawdaten.
 */

ArchivItem_Raw::ArchivItem_Raw() : ArchivItem(BOBTYPE_RAW)
{
}

ArchivItem_Raw::ArchivItem_Raw(const std::vector<uint8_t>& initialData) : ArchivItem(BOBTYPE_RAW), data(initialData)
{
}

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
