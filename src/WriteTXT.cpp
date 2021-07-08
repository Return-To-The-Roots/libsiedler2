// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "Archiv.h"
#include "ArchivItem_Text.h"
#include "ErrorCodes.h"
#include "prototypen.h"
#include "libendian/EndianOStreamAdapter.h"
#include <boost/nowide/fstream.hpp>
#include <algorithm>
#include <limits>
#include <stdexcept>
#include <utility>

/**
 *  schreibt eine GER/ENG-File aus einem Archiv.
 *
 *  @param[in] file       Dateiname der GER/ENG-File
 *  @param[in] items      Archiv-Struktur, welche geschrieben wird
 *  @param[in] conversion Soll ggf. ANSI-Charset in OEM umgewandelt werden?
 *
 *  @return Null bei Erfolg, ein Wert ungleich Null bei Fehler
 */
int libsiedler2::loader::WriteTXT(const boost::filesystem::path& filepath, const Archiv& items, bool conversion)
{
    if(filepath.empty())
        return ErrorCode::INVALID_BUFFER;

    // All entries have to be texts or empty
    const auto isInvalidEntry = [](const auto& item) {
        return item && !dynamic_cast<const ArchivItem_Text*>(item.get());
    };
    if(std::find_if(begin(items), end(items), isInvalidEntry) != end(items))
        return ErrorCode::WRONG_ARCHIVE;

    // Datei zum lesen öffnen
    libendian::EndianOStreamAdapter<false, boost::nowide::ofstream> fs(filepath, std::ios_base::binary);

    // hat das geklappt?
    if(!fs)
        return ErrorCode::FILE_NOT_ACCESSIBLE;

    // Plain-Text (1 existing entry)?
    if(items.size() == 1 && items[0])
        return static_cast<const ArchivItem_Text*>(items[0])->write(fs.getStream(), conversion);
    else
    {
        // "archiviert"
        uint16_t header = 0xFDE7;
        if(items.size() > std::numeric_limits<uint16_t>::max())
            return ErrorCode::UNSUPPORTED_FORMAT;
        auto count = static_cast<uint16_t>(items.size());
        uint16_t unknown = 1;

        fs << header << count << unknown;

        std::vector<uint32_t> starts;
        std::vector<std::pair<std::string, uint32_t>> storedValuesToOffset;
        starts.reserve(count);
        storedValuesToOffset.reserve(count);

        size_t curOffset = count * sizeof(uint32_t);
        for(const auto& itemPtr : items)
        {
            if(!itemPtr)
                starts.push_back(0);
            else
            {
                const auto* item = static_cast<const ArchivItem_Text*>(itemPtr.get());
                auto text = item->getFileText(conversion);
                if(text.empty())
                {
                    // Only store NULL terminator (empty string)
                    starts.push_back(curOffset);
                    storedValuesToOffset.emplace_back(std::string{}, curOffset);
                    curOffset += 1;
                } else
                {
                    const auto itOldOffset = std::find_if(storedValuesToOffset.begin(), storedValuesToOffset.end(),
                                                          [&text](const auto& entry) { return entry.first == text; });
                    if(itOldOffset == storedValuesToOffset.end())
                    {
                        // New value
                        starts.push_back(curOffset);
                        storedValuesToOffset.emplace_back(std::move(text), curOffset);
                        curOffset += storedValuesToOffset.back().first.size() + 1u; // Include NULL terminator
                    } else
                        starts.push_back(itOldOffset->second); // Reuse
                }
            }
        }
        if(curOffset > std::numeric_limits<uint32_t>::max())
            return ErrorCode::UNSUPPORTED_FORMAT;
        const uint32_t size = curOffset;
        fs << size << starts;

        // Write texts
        for(const auto& item : storedValuesToOffset)
        {
            if(!fs.write(item.first.data(), item.first.size() + 1)) // Include NULL terminator
                return ErrorCode::UNEXPECTED_EOF;
        }
    }

    return (!fs) ? ErrorCode::UNEXPECTED_EOF : ErrorCode::NONE;
}
