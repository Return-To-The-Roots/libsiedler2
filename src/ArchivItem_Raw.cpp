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

///////////////////////////////////////////////////////////////////////////////
// Header
#include "main.h" // IWYU pragma: keep
#include "ArchivItem_Raw.h"
#include <fstream>
#include "EndianStream.h"

// Include last!
#include "DebugNew.h" // IWYU pragma: keep

///////////////////////////////////////////////////////////////////////////////
/** @class libsiedler2::baseArchivItem_Raw
 *
 *  Basisklasse für Rawdaten.
 *
 *  @author FloSoft
 */

libsiedler2::baseArchivItem_Raw::baseArchivItem_Raw()
    : ArchivItem()
{
    setBobType(BOBTYPE_RAW);
}

libsiedler2::baseArchivItem_Raw::~baseArchivItem_Raw()
{}

///////////////////////////////////////////////////////////////////////////////
/**
 *  lädt die Rawdaten aus einer Datei.
 *
 *  @param[in] file   Dateihandle der Datei
 *  @param[in] length Länge des Blocks (aus Datei lesen = 0xFFFFFFFF)
 *
 *  @return liefert Null bei Erfolg, ungleich Null bei Fehler
 *
 *  @author FloSoft
 */
int libsiedler2::baseArchivItem_Raw::load(std::istream& file, unsigned int length)
{
    if(!file)
        return 1;

    clear();

    libendian::LittleEndianIStreamRef fs(file);
    if(length == 0xFFFFFFFF)
    {
        fs >> length;
    }

    data.resize(length);
    fs >> data;

    return 0;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  schreibt die Rawdaten in eine Datei.
 *
 *  @param[in] file        Dateihandle der Datei
 *  @param[in] with_length Soll die Blocklänge mitgeschrieben werden?
 *
 *  @return liefert Null bei Erfolg, ungleich Null bei Fehler
 *
 *  @author FloSoft
 */
int libsiedler2::baseArchivItem_Raw::write(std::ostream& file, bool with_length) const
{
    if(!file)
        return 1;

    libendian::LittleEndianOStreamRef fs(file);
    if(with_length)
    {
        // Convert to unsigned first
        fs << static_cast<uint32_t>(data.size());
    }

    fs << data;

    return 0;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  liefert die Daten zurück (ro).
 *
 *  @author FloSoft
 */
const std::vector<unsigned char>& libsiedler2::baseArchivItem_Raw::getData() const
{
    return data;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  liefert die Daten zurück (rw).
 *
 *  @author FloSoft
 */
std::vector<unsigned char>& libsiedler2::baseArchivItem_Raw::getData()
{
    return data;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  löscht den Datenblock.
 *
 *  @author FloSoft
 */
void libsiedler2::baseArchivItem_Raw::clear()
{
    data.clear();
}
