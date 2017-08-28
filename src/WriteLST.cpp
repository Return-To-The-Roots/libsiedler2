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
#include "ArchivInfo.h"
#include "ArchivItem.h"
#include "ErrorCodes.h"
#include "prototypen.h"
#include "libendian/src/EndianOStreamAdapter.h"
#include <boost/filesystem/fstream.hpp>
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
int libsiedler2::loader::WriteLST(const std::string& file, const ArchivInfo& items, const ArchivItem_Palette* palette)
{
    if(file.empty())
        return ErrorCode::INVALID_BUFFER;

    // Datei zum schreiben öffnen
    libendian::EndianOStreamAdapter<false, bfs::ofstream> fs(file, std::ios_base::binary);

    // hat das geklappt?
    if(!fs)
        return ErrorCode::FILE_NOT_ACCESSIBLE;

    int16_t header = 0x4E20;
    assert(items.size() < std::numeric_limits<uint32_t>::max());
    uint32_t count = static_cast<uint32_t>(items.size());

    // Header schreiben
    fs << header << count;

    // items schreiben
    for(uint32_t i = 0; i < count; ++i)
    {
        uint16_t used = 0x0001;

        const ArchivItem* item = items[i];

        if(!item)
            used = 0x0000;

        // use-Flag schreiben
        fs << used;

        if(!item)
            continue;

        BobType bobtype = item->getBobType();

        // bobtype des Items schreiben
        fs << (int16_t)bobtype;

        // Daten von Item schreiben
        if(int ec = WriteType(bobtype, fs.getStream(), *item, palette))
            return ec;
    }

    return (!fs) ? ErrorCode::UNEXPECTED_EOF : ErrorCode::NONE;
    ;
}
