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

#include "libSiedler2Defines.h" // IWYU pragma: keep
#include "ArchivInfo.h"
#include "prototypen.h"
#include <fstream>
#include "libendian/src/EndianStream.h"
#include <limits>

/**
 *  schreibt ein ArchivInfo in eine LST-File.
 *
 *  @param[in] file    Dateiname der LST-File
 *  @param[in] palette Grundpalette der LST-File
 *  @param[in] items   ArchivInfo-Struktur, welche gefüllt wird
 *
 *  @return Null bei Erfolg, ein Wert ungleich Null bei Fehler
 */
int libsiedler2::loader::WriteLST(const std::string& file, const ArchivItem_Palette* palette, const ArchivInfo& items)
{
    short header = 0x4E20;
    assert(items.size() < std::numeric_limits<uint32_t>::max());
    uint32_t count = static_cast<uint32_t>(items.size());

    if(file.empty())
        return 1;

    // Datei zum schreiben öffnen
    libendian::LittleEndianOFStream lst(file);

    // hat das geklappt?
    if(!lst)
        return 2;

    // Header schreiben
    lst << header;

    // Anzahl schreiben
    lst << count;

    // items schreiben
    for(unsigned i = 0; i < count; ++i)
    {
        unsigned short used = 0x0001;

        const ArchivItem* item = items.get(i);

        if(item == NULL)
            used = 0x0000;

        // use-Flag schreiben
        lst << used;

        if(!item)
            continue;

        BOBTYPES bobtype = item->getBobType();

        // bobtype des Items schreiben
        lst << (int16_t)bobtype;

        // Daten von Item schreiben
        if(WriteType(bobtype, lst.getStream(), palette, *item) != 0)
            return 8;
    }

    // alles ok
    return 0;
}
