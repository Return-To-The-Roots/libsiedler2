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
#include "ArchivItem_Ini.h"
#include "ErrorCodes.h"
#include "IAllocator.h"
#include "OpenMemoryStream.h"
#include "libsiedler2.h"
#include "prototypen.h"
#include <memory>

/**
 *  lädt eine INI-File in ein Archiv.
 *
 *  @param[in]  file    Dateiname der INI-File
 *  @param[out] items   Archiv-Struktur, welche gefüllt wird
 *
 *  @return Null bei Erfolg, ein Wert ungleich Null bei Fehler
 */
int libsiedler2::loader::LoadINI(const std::string& file, Archiv& items)
{
    MMStream ini;
    if(int ec = openMemoryStream(file, ini))
        return ec;

    while(!ini.eof())
    {
        std::unique_ptr<ArchivItem_Ini> item(dynamic_cast<ArchivItem_Ini*>(getAllocator().create(BOBTYPE_INI)));

        if(int ec = item->load(ini))
            return ec;

        items.push(item.release());
    }

    return ErrorCode::NONE;
}
