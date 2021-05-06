// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "Archiv.h"
#include "ArchivItem_Palette.h"
#include "ErrorCodes.h"
#include "GetIStreamSize.h"
#include "IAllocator.h"
#include "OpenMemoryStream.h"
#include "libsiedler2.h"
#include "prototypen.h"

/**
 *  lädt eine ACT-File in ein Archiv.
 *
 *  @param[in]  filepath    Dateiname der ACT-File
 *  @param[out] items   Archiv-Struktur, welche gefüllt wird
 *
 *  @return Null bei Erfolg, ein Wert ungleich Null bei Fehler
 */
int libsiedler2::loader::LoadACT(const boost::filesystem::path& filepath, Archiv& items)
{
    MMStream act;
    if(int ec = openMemoryStream(filepath, act))
        return ec;

    size_t size = getIStreamSize(act);
    // sind es 256*3 Bytes, also somit 8bit-RGB?
    if(size != 256 * 3)
        return ErrorCode::WRONG_HEADER;

    auto palette = getAllocator().create<ArchivItem_Palette>(BobType::Palette);
    if(int ec = palette->load(act, false))
        return ec;

    // einlesen
    items.clear();
    items.push(std::move(palette));

    // Alles OK
    return ErrorCode::NONE;
}
