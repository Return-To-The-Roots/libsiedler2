// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

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
