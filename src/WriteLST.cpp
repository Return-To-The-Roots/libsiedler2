// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "Archiv.h"
#include "ArchivItem.h"
#include "ErrorCodes.h"
#include "prototypen.h"
#include "libendian/EndianOStreamAdapter.h"
#include <boost/nowide/fstream.hpp>
#include <limits>

/**
 *  schreibt ein Archiv in eine LST-File.
 *
 *  @param[in] file    Dateiname der LST-File
 *  @param[in] palette Grundpalette der LST-File
 *  @param[in] items   Archiv-Struktur, welche gefüllt wird
 *
 *  @return Null bei Erfolg, ein Wert ungleich Null bei Fehler
 */
int libsiedler2::loader::WriteLST(const boost::filesystem::path& filepath, const Archiv& items,
                                  const ArchivItem_Palette* palette)
{
    if(filepath.empty())
        return ErrorCode::INVALID_BUFFER;

    // Datei zum schreiben öffnen
    libendian::EndianOStreamAdapter<false, boost::nowide::ofstream> fs(filepath, std::ios_base::binary);

    // hat das geklappt?
    if(!fs)
        return ErrorCode::FILE_NOT_ACCESSIBLE;

    int16_t header = 0x4E20;
    assert(items.size() < std::numeric_limits<uint32_t>::max());
    auto count = static_cast<uint32_t>(items.size());

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
        {
            // If custom error store the index too
            if(ec == ErrorCode::CUSTOM)
                ec = ErrorCode::CUSTOM + std::min<uint32_t>(i, std::numeric_limits<int>::max() - ErrorCode::CUSTOM);
            return ec;
        }
    }

    return (!fs) ? ErrorCode::UNEXPECTED_EOF : ErrorCode::NONE;
}
