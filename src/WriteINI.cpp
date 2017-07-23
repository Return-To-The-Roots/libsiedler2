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
#include "ArchivItem_Ini.h"
#include "ArchivInfo.h"
#include "prototypen.h"
#include <fstream>

/**
 *  schreibt ein ArchivInfo in eine INI-File.
 *
 *  @param[in]  file    Dateiname der INI-File
 *  @param[in] items    ArchivInfo-Struktur, welche gefüllt wird
 *
 *  @return Null bei Erfolg, ein Wert ungleich Null bei Fehler
 */
int libsiedler2::loader::WriteINI(const std::string& file, const ArchivInfo& items)
{
    if(file.empty())
        return 1;

    // Datei zum schreiben öffnen
    std::ofstream ini(file.c_str(), std::ios_base::binary);
    if (!ini)
        return 2;

    for(size_t i = 0; i < items.size(); ++i)
    {
        const ArchivItem_Ini* item = dynamic_cast<const ArchivItem_Ini*>(items.get(i));

        if(item && item->write(ini) != 0)
            return 99;
    }

    // alles ok
    return 0;
}
