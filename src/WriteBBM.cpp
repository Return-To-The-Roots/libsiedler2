// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "Archiv.h"
#include "ArchivItem_Palette.h"
#include "ErrorCodes.h"
#include "prototypen.h"
#include "libendian/EndianOStreamAdapter.h"
#include <boost/nowide/fstream.hpp>

/**
 *  schreibt ein Archiv in eine BBM-File.
 *
 *  @param[in] file    Dateiname der BBM-File
 *  @param[in] items   Archiv-Struktur, von welcher gelesen wird
 *
 *  @return Null bei Erfolg, ein Wert ungleich Null bei Fehler
 */
int libsiedler2::loader::WriteBBM(const boost::filesystem::path& filepath, const Archiv& items)
{
    std::array<char, 4> header = {'F', 'O', 'R', 'M'}, pbm = {'P', 'B', 'M', ' '}, cmap = {'C', 'M', 'A', 'P'};

    if(filepath.empty())
        return ErrorCode::INVALID_BUFFER;

    uint32_t numPalettes = 0;
    // Anzahl Paletten in Archiv suchen
    for(size_t i = 0; i < items.size(); ++i)
    {
        if(dynamic_cast<const ArchivItem_Palette*>(items[i]))
            ++numPalettes; //-V127
    }

    // Datei zum schreiben öffnen
    libendian::EndianOStreamAdapter<true, boost::nowide::ofstream> fs(filepath, std::ios_base::binary);

    // hat das geklappt?
    if(!fs)
        return ErrorCode::FILE_NOT_ACCESSIBLE;

    uint32_t length = 4 + numPalettes * (256 * 3 + 8);

    fs << header << length << pbm;

    for(size_t i = 0; i < items.size(); ++i)
    {
        const auto* palette = dynamic_cast<const ArchivItem_Palette*>(items[i]);
        if(palette)
        {
            fs << cmap << uint32_t(256 * 3);

            if(int ec = palette->write(fs.getStream(), false))
                return ec;
        }
    }

    return (!fs) ? ErrorCode::UNEXPECTED_EOF : ErrorCode::NONE;
}
