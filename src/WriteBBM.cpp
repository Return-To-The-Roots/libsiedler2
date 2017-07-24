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
    uint32_t count = 0;
    uint32_t length = 0;

    if(file.empty())
        return 1;

    // Anzahl Paletten in ArchivInfo suchen
    for(size_t i = 0; i < items.size(); ++i)
    {
        if(!items.get(i))
            continue;
        if(items.get(i)->getBobType() == BOBTYPE_PALETTE)
            ++count;
    }

    // Datei zum schreiben öffnen
    libendian::EndianOStreamAdapter<true, bfs::ofstream> bbm(file, std::ios_base::binary);

    // hat das geklappt?
    if(!bbm)
        return 2;

    // Header schreiben
    bbm << header;

    // Länge schreiben
    length = 4 + count * (256 * 3 + 8);
    bbm << length;

    // Typ schreiben
    bbm << pbm;

    for(size_t i = 0; i < items.size(); ++i)
    {
        ArchivItem_Palette* palette = (ArchivItem_Palette*)items.get(i);
        if(palette->getBobType() == BOBTYPE_PALETTE)
        {
            // Chunk schreiben
            bbm << cmap;

            // Länge schreiben
            length = 256 * 3;
            bbm << length;

            palette->write(bbm.getStream(), false);
        }
    }

    // alles ok
    return (!bbm) ? 99 : 0;
}
