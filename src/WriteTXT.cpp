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
#include "ArchivItem_Text.h"
#include "ErrorCodes.h"
#include "prototypen.h"
#include "libendian/src/EndianOStreamAdapter.h"
#include <boost/filesystem/fstream.hpp>

/**
 *  schreibt eine GER/ENG-File aus einem ArchivInfo.
 *
 *  @param[in] file       Dateiname der GER/ENG-File
 *  @param[in] items      ArchivInfo-Struktur, welche geschrieben wird
 *  @param[in] conversion Soll ggf. ANSI-Charset in OEM umgewandelt werden?
 *
 *  @return Null bei Erfolg, ein Wert ungleich Null bei Fehler
 */
int libsiedler2::loader::WriteTXT(const std::string& file, const ArchivInfo& items, bool conversion)
{
    if(file.empty())
        return ErrorCode::INVALID_BUFFER;

    // All entries have to be texts or empty
    for(size_t i = 0; i < items.size(); ++i)
    {
        const ArchivItem* item = items[i];
        if(item && !dynamic_cast<const ArchivItem_Text*>(item))
            return ErrorCode::WRONG_ARCHIV;
    }

    // Datei zum lesen öffnen
    libendian::EndianOStreamAdapter<false, bfs::ofstream> fs(file, std::ios_base::binary);

    // hat das geklappt?
    if(!fs)
        return ErrorCode::FILE_NOT_ACCESSIBLE;

    // Plain-Text ?
    if(items.size() == 1)
        return dynamic_cast<const ArchivItem_Text*>(items[0])->write(fs.getStream(), conversion);
    else
    {
        // "archiviert"
        uint16_t header = 0xFDE7;
        uint16_t count = (uint16_t)items.size();
        uint16_t unknown = 1;

        fs << header << count << unknown;

        std::vector<uint32_t> starts(count);

        uint32_t size = count * sizeof(uint32_t);
        for(uint32_t i = 0; i < count; ++i)
        {
            const ArchivItem_Text* item = dynamic_cast<const ArchivItem_Text*>(items[i]);

            if(item && !item->getText().empty())
            {
                starts[i] = size;
                size += static_cast<uint32_t>(item->getFileText(conversion).size());
            }
        }

        fs << size << starts;

        // Texte schreiben
        for(uint32_t i = 0; i < count; ++i)
        {
            const ArchivItem_Text* item = dynamic_cast<const ArchivItem_Text*>(items[i]);

            if(item)
            {
                if(int ec = item->write(fs.getStream(), conversion))
                    return ec;
            }
        }
    }

    return (!fs) ? ErrorCode::UNEXPECTED_EOF : ErrorCode::NONE;
    ;
}
