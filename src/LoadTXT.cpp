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

#include "Archiv.h"
#include "ArchivItem_Text.h"
#include "ErrorCodes.h"
#include "GetIStreamSize.h"
#include "IAllocator.h"
#include "OpenMemoryStream.h"
#include "libsiedler2.h"
#include "prototypen.h"
#include "libendian/EndianIStreamAdapter.h"
#include <algorithm>
#include <vector>

/**
 *  lädt eine GER/ENG-File in ein Archiv.
 *
 *  @param[in]  filepath       Dateiname der GER/ENG-File
 *  @param[out] items      Archiv-Struktur, welche gefüllt wird
 *  @param[in]  conversion Soll ggf. OEM-Charset in ANSI umgewandelt werden?
 *
 *  @return Null bei Erfolg, ein Wert ungleich Null bei Fehler
 *
 *  @bug Keine Erkennung ob Plain-Text oder "Irgendwas".
 */
int libsiedler2::loader::LoadTXT(const boost::filesystem::path& filepath, Archiv& items, bool conversion)
{
    MMStream mmapStream;
    if(int ec = openMemoryStream(filepath, mmapStream))
        return ec;
    libendian::EndianIStreamAdapter<false, MMStream&> fs(mmapStream);

    const size_t fileSize = getIStreamSize(fs.getStream());
    assert(fileSize < std::numeric_limits<uint32_t>::max());

    uint16_t header;
    // Header einlesen
    if(fileSize < sizeof(header))
        header = 0;
    else if(!(fs >> header))
        return ErrorCode::WRONG_HEADER;

    items.clear();

    // ist es eine TXT-File? (Header 0xE7FD)
    if(header != 0xFDE7u)
    {
        // den Header zurückspringen
        if(fileSize >= sizeof(header))
            fs.setPositionRel(-2);

        auto item = getAllocator().create<ArchivItem_Text>(BobType::Text);
        int ec = item->load(fs.getStream(), conversion);
        if(ec)
            return ec;
        items.push(std::move(item));
    } else
    {
        // "archiviert"
        uint16_t count, unknown;
        uint32_t dataSize;

        constexpr size_t headerSize = sizeof(header) + sizeof(count) + sizeof(unknown) + sizeof(dataSize);

        if(!(fs >> count >> unknown >> dataSize))
            return ErrorCode::WRONG_HEADER;

        if(dataSize == 0)
            dataSize = static_cast<uint32_t>(fileSize - headerSize);

        const auto offsetsSize = count * sizeof(uint32_t);
        if(dataSize < offsetsSize)
            return ErrorCode::WRONG_FORMAT;

        std::vector<uint32_t> offsets(count);
        if(!(fs >> offsets))
            return ErrorCode::UNEXPECTED_EOF;

        // Read all strings to memory
        std::vector<char> stringData(dataSize - offsetsSize);
        if(!(fs >> stringData))
            return ErrorCode::UNEXPECTED_EOF;

        for(const auto itemPos : offsets)
        {
            if(!itemPos)
                items.push(nullptr);
            else if(itemPos < offsetsSize)
                return ErrorCode::WRONG_FORMAT;
            else
            {
                const auto itStart = stringData.cbegin() + (itemPos - offsetsSize);
                // All strings are NULL terminated.
                const auto itEnd = std::find(itStart, stringData.cend(), '\0');
                assert(itEnd != stringData.end());
                // Load element
                auto item = getAllocator().create<ArchivItem_Text>(BobType::Text);
                item->setText(std::string(itStart, itEnd), conversion);
                items.push(std::move(item));
            }
        }
    }

    return (!fs) ? ErrorCode::UNEXPECTED_EOF : ErrorCode::NONE;
}
