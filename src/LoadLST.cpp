// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "Archiv.h"
#include "ArchivItem.h"
#include "ErrorCodes.h"
#include "OpenMemoryStream.h"
#include "prototypen.h"
#include "libendian/EndianIStreamAdapter.h"

/**
 *  lädt eine LST-File in ein Archiv.
 *
 *  @param[in]  filepath    Dateiname der LST-File
 *  @param[in]  palette Grundpalette der LST-File
 *  @param[out] items   Archiv-Struktur, welche gefüllt wird
 *
 *  @return Null bei Erfolg, ein Wert ungleich Null bei Fehler
 */
int libsiedler2::loader::LoadLST(const boost::filesystem::path& filepath, Archiv& items,
                                 const ArchivItem_Palette* palette)
{
    MMStream mmapStream;
    if(int ec = openMemoryStream(filepath, mmapStream))
        return ec;

    libendian::EndianIStreamAdapter<false, MMStream&> lst(mmapStream);

    uint16_t header;
    uint32_t count;

    // Header einlesen
    lst >> header;

    // ist es eine GER/ENG-File? (Header 0xE7FD)
    if(header == 0xFDE7)
        return LoadTXT(filepath, items, true);

    // ist es eine LST-File? (Header 0x204E)
    if(header != 0x4E20)
        return ErrorCode::WRONG_HEADER;

    // Anzahl einlesen
    lst >> count;

    if(!lst)
        return ErrorCode::WRONG_FORMAT;

    items.clear();

    // items einlesen
    for(uint32_t i = 0; i < count; ++i)
    {
        int16_t used;
        int16_t bobtype_s;

        // use-Flag einlesen
        if(!(lst >> used))
            return ErrorCode::UNEXPECTED_EOF;

        // ist das Item benutzt?
        if(used != 1)
        {
            items.push(nullptr);
            continue;
        }

        // bobtype des Items einlesen
        lst >> bobtype_s;
        auto bobtype = static_cast<BobType>(bobtype_s);

        // Daten von Item auswerten
        std::unique_ptr<ArchivItem> item;
        if(int ec = LoadType(bobtype, lst.getStream(), item, palette))
            return ec;
        items.push(std::move(item));
    }

    return ErrorCode::NONE;
}
