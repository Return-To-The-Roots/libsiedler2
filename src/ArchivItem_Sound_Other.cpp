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

#include "main.h" // IWYU pragma: keep
#include "ArchivItem_Sound_Other.h"
#include <fstream>
#include "EndianStream.h"

// Include last!
#include "DebugNew.h" // IWYU pragma: keep

/** @class libsiedler2::baseArchivItem_Sound_Other
 *
 *  Klasse für Other-Sounds.
 *
 *  @author FloSoft
 */

libsiedler2::baseArchivItem_Sound_Other::baseArchivItem_Sound_Other() : baseArchivItem_Sound()
{
    setType(SOUNDTYPE_OTHER);
}

libsiedler2::baseArchivItem_Sound_Other::~baseArchivItem_Sound_Other()
{}

/**
 *  lädt die Daten aus einer Datei.
 *
 *  @param[in] file    Dateihandle der Datei
 *  @param[in] length  Länge der Daten
 *
 *  @return liefert Null bei Erfolg, ungleich Null bei Fehler
 *
 *  @author FloSoft
 */
int libsiedler2::baseArchivItem_Sound_Other::load(std::istream& file, unsigned int length)
{
    if(!file || length == 0)
        return 1;

    data.resize(length);
    libendian::LittleEndianIStreamRef fs(file);

    fs >> data;

    return 0;
}

/**
 *  schreibt die Daten in eine Datei.
 *
 *  @param[in] file        Dateihandle der Datei
 *
 *  @return liefert Null bei Erfolg, ungleich Null bei Fehler
 *
 *  @author FloSoft
 */
int libsiedler2::baseArchivItem_Sound_Other::write(std::ostream& file) const
{
    if(!file)
        return 1;

    libendian::LittleEndianOStreamRef fs(file);
    fs << static_cast<uint32_t>(data.size()) << data;

    return 0;
}

/**
 *  räumt den Soundspeicher auf.
 *
 *  @author FloSoft
 */
void libsiedler2::baseArchivItem_Sound_Other::clear()
{
    data.resize(0);
}
