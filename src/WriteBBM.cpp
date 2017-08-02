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
#include "ArchivItem_Palette.h"
#include "ArchivInfo.h"
#include "prototypen.h"
#include "ErrorCodes.h"
#include "libendian/src/EndianOStreamAdapter.h"
#include <boost/filesystem/fstream.hpp>

/**
 *  schreibt ein ArchivInfo in eine BBM-File.
 *
 *  @param[in] file    Dateiname der BBM-File
 *  @param[in] items   ArchivInfo-Struktur, von welcher gelesen wird
 *
 *  @return Null bei Erfolg, ein Wert ungleich Null bei Fehler
 */
int libsiedler2::loader::WriteBBM(const std::string& file, const ArchivInfo& items)
{
    char header[4] = {'F', 'O', 'R', 'M'}, pbm[4] = {'P', 'B', 'M', ' '}, cmap[4] = {'C', 'M', 'A', 'P'};

    if(file.empty())
        return ErrorCode::INVALID_BUFFER;

    uint32_t numPalettes = 0;
    // Anzahl Paletten in ArchivInfo suchen
    for(size_t i = 0; i < items.size(); ++i)
    {
        if(dynamic_cast<const ArchivItem_Palette*>(items[i]))
            ++numPalettes;
    }

    // Datei zum schreiben öffnen
    libendian::EndianOStreamAdapter<true, bfs::ofstream> fs(file, std::ios_base::binary);

    // hat das geklappt?
    if(!fs)
        return ErrorCode::FILE_NOT_ACCESSIBLE;

    uint32_t length = 4 + numPalettes * (256 * 3 + 8);

    fs << header << length << pbm;

    for(size_t i = 0; i < items.size(); ++i)
    {
        const ArchivItem_Palette* palette = dynamic_cast<const ArchivItem_Palette*>(items[i]);
        if(palette)
        {
            fs << cmap << uint32_t(256 * 3);

            if(int ec = palette->write(fs.getStream(), false))
                return ec;
        }
    }

    return (!fs) ? ErrorCode::UNEXPECTED_EOF : ErrorCode::NONE;
}
