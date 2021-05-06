// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

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
 *  @param[in]  filepath    Dateiname der INI-File
 *  @param[out] items   Archiv-Struktur, welche gefüllt wird
 *
 *  @return Null bei Erfolg, ein Wert ungleich Null bei Fehler
 */
int libsiedler2::loader::LoadINI(const boost::filesystem::path& filepath, Archiv& items)
{
    MMStream ini;
    if(int ec = openMemoryStream(filepath, ini))
        return ec;

    while(!ini.eof())
    {
        auto item = getAllocator().create<ArchivItem_Ini>(BobType::Ini);

        if(int ec = item->load(ini))
            return ec;

        items.push(std::move(item));
    }

    return ErrorCode::NONE;
}
