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

#include "main.h" // IWYU pragma: keep
#include "ArchivItem_Map.h"
#include "ArchivInfo.h"
#include "prototypen.h"
#include <fstream>

// Include last!
#include "DebugNew.h" // IWYU pragma: keep

///////////////////////////////////////////////////////////////////////////////
/**
 *  schreibt ein ArchivInfo in eine SWD/WSD-File.
 *
 *  @param[in] file    Dateiname der SWD/WSD-File
 *  @param[in] items   ArchivInfo-Struktur, von welcher gelesen wird
 *
 *  @return Null bei Erfolg, ein Wert ungleich Null bei Fehler
 *
 *  @author FloSoft
 */
int libsiedler2::loader::WriteMAP(const std::string& file, const ArchivInfo& items, long nr)
{
    if(file.empty())
        return 1;

    if(nr == -1)
    {
        // Palette in ArchivInfo suchen, erste Palette wird geschrieben
        for(size_t i = 0; i < items.size(); ++i)
        {
            if(!items.get(i))
                continue;
            if(items.get(i)->getBobType() == BOBTYPE_MAP)
            {
                nr = static_cast<long>(i);
                break;
            }

        }
    }

    // Haben wir eine gefunden?
    if(nr == -1)
        return 2;

    // Datei zum lesen öffnen
    std::ofstream map(file.c_str(), std::ios_base::binary);

    // hat das geklappt?
    if(!map)
        return 2;

    const ArchivItem_Map* item = dynamic_cast<const ArchivItem_Map*>(items.get(nr));
    if(item->write(map) != 0)
        return 3;

    return 0;
}
