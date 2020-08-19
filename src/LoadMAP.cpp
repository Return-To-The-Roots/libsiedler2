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
#include "ArchivItem_Map.h"
#include "ErrorCodes.h"
#include "IAllocator.h"
#include "OpenMemoryStream.h"
#include "libsiedler2.h"
#include "prototypen.h"

/**
 *  lädt eine MAP-File in ein Archiv.
 *
 *  @param[in]  filepath    Dateiname der MAP-File
 *  @param[out] items   Archiv-Struktur, welche gefüllt wird
 *
 *  @return Null bei Erfolg, ein Wert ungleich Null bei Fehler
 */
int libsiedler2::loader::LoadMAP(const boost::filesystem::path& filepath, Archiv& items, bool only_header)
{
    MMStream map;
    if(int ec = openMemoryStream(filepath, map))
        return ec;

    auto item = getAllocator().create<ArchivItem_Map>(BobType::Map);
    if(int ec = item->load(map, only_header)) //-V522
        return ec;

    items.clear();
    items.push(std::move(item));

    return ErrorCode::NONE;
}
