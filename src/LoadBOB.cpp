// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "Archiv.h"
#include "ArchivItem_Bob.h"
#include "ErrorCodes.h"
#include "IAllocator.h"
#include "OpenMemoryStream.h"
#include "libsiedler2.h"
#include "prototypen.h"
#include "libendian/EndianIStreamAdapter.h"
#include <boost/filesystem/path.hpp>

/**
 *  lädt eine BOB-File in ein Archiv.
 *
 *  @param[in]  filepath    Dateiname der BOB-File
 *  @param[out] items   Archiv-Struktur, welche gefüllt wird
 *
 *  @return Null bei Erfolg, ein Wert ungleich Null bei Fehler
 */
int libsiedler2::loader::LoadBOB(const boost::filesystem::path& filepath, Archiv& items,
                                 const ArchivItem_Palette* palette)
{
    if(palette == nullptr)
        return ErrorCode::PALETTE_MISSING;

    MMStream mmapStream;
    if(int ec = openMemoryStream(filepath, mmapStream))
        return ec;
    libendian::EndianIStreamAdapter<false, MMStream&> bob(mmapStream);

    // Header einlesen
    uint16_t header;

    bob >> header;

    // ist es eine BOB-File?
    if(!bob || header != 0x01F6)
        return ErrorCode::WRONG_HEADER;

    auto item = getAllocator().create<ArchivItem_Bob>(BobType::Bob);

    if(filepath.has_filename())
        item->setName(filepath.filename().string()); //-V522

    if(int ec = item->load(bob.getStream(), palette))
        return ec;

    // Item alloziieren und zuweisen
    items.clear();
    items.push(std::move(item));

    return ErrorCode::NONE;
}
