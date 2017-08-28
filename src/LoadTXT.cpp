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
#include "Archiv.h"
#include "ArchivItem_Text.h"
#include "ErrorCodes.h"
#include "IAllocator.h"
#include "OpenMemoryStream.h"
#include "libsiedler2.h"
#include "prototypen.h"
#include "libendian/src/EndianIStreamAdapter.h"
#include <vector>

/**
 *  lädt eine GER/ENG-File in ein ArchivInfo.
 *
 *  @param[in]  file       Dateiname der GER/ENG-File
 *  @param[out] items      ArchivInfo-Struktur, welche gefüllt wird
 *  @param[in]  conversion Soll ggf. OEM-Charset in ANSI umgewandelt werden?
 *
 *  @return Null bei Erfolg, ein Wert ungleich Null bei Fehler
 *
 *  @bug Keine Erkennung ob Plain-Text oder "Irgendwas".
 */
int libsiedler2::loader::LoadTXT(const std::string& file, Archiv& items, bool conversion)
{
    MMStream mmapStream;
    if(int ec = openMemoryStream(file, mmapStream))
        return ec;
    libendian::EndianIStreamAdapter<false, MMStream&> fs(mmapStream);

    const size_t fileSize = getIStreamSize(fs.getStream());
    assert(fileSize < std::numeric_limits<uint32_t>::max());

    int16_t header;
    // Header einlesen
    if(!(fs >> header))
        return ErrorCode::WRONG_HEADER;

    items.clear();

    // ist es eine TXT-File? (Header 0xE7FD)
    if(header != (int16_t)0xFDE7)
    {
        // den Header zurückspringen
        fs.setPositionRel(-2);

        ArchivItem_Text* item = (ArchivItem_Text*)getAllocator().create(BOBTYPE_TEXT);
        int ec = item->load(fs.getStream(), conversion);
        if(ec)
            return ec;
        items.push(item);
    } else
    {
        // "archiviert"
        uint16_t count, unknown;
        uint32_t size;

        BOOST_CONSTEXPR_OR_CONST size_t headerSize = sizeof(header) + sizeof(count) + sizeof(unknown) + sizeof(size);

        if(!(fs >> count >> unknown >> size))
            return ErrorCode::WRONG_HEADER;

        if(size == 0)
            size = static_cast<uint32_t>(fileSize) - headerSize;
        if(size < count * sizeof(uint32_t))
            return ErrorCode::WRONG_FORMAT;

        std::vector<uint32_t> starts(count);
        if(!(fs >> starts))
            return ErrorCode::UNEXPECTED_EOF;

        // Starts einlesen
        uint16_t lastStart = 0;
        for(uint16_t x = 0; x < count; ++x)
        {
            if(starts[x] != 0)
            {
                // Contiguous values and offset must be after the starts
                if(starts[x] < lastStart || starts[x] < count * sizeof(uint32_t))
                    return ErrorCode::WRONG_FORMAT;
                lastStart = starts[x];
                starts[x] += headerSize;
            }
        }

        // Add value for getting end position
        starts.push_back(size + headerSize);

        for(uint16_t x = 0; x < count; ++x)
        {
            uint32_t itemPos = starts[x];

            if(itemPos != 0)
            {
                uint32_t itemSize = 0;
                for(uint16_t j = x + 1; j <= count; ++j)
                {
                    if(starts[j])
                    {
                        itemSize = starts[j] - itemPos;
                        break;
                    }
                }
                // An Start springen
                fs.setPosition(itemPos);

                // einlesen
                ArchivItem_Text* item = (ArchivItem_Text*)getAllocator().create(BOBTYPE_TEXT);
                int res = item->load(fs.getStream(), conversion, itemSize);
                if(res)
                    return res;

                items.push(item);
            } else
                items.push(NULL);
        }
    }

    return (!fs) ? ErrorCode::UNEXPECTED_EOF : ErrorCode::NONE;
}
