// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "Archiv.h"
#include "ArchivItem.h"
#include "ErrorCodes.h"
#include "GetIStreamSize.h"
#include "OpenMemoryStream.h"
#include "prototypen.h"
#include "libendian/EndianIStreamAdapter.h"
#include <boost/filesystem.hpp>

namespace bfs = boost::filesystem;

/**
 *  lädt eine DAT/IDX-File in ein Archiv.
 *
 *  @param[in]  filepath    Dateiname der DAT/IDX-File
 *  @param[in]  palette Grundpalette der DAT/IDX-File
 *  @param[out] items   Archiv-Struktur, welche gefüllt wird
 *
 *  @return Null bei Erfolg, ein Wert ungleich Null bei Fehler
 */
int libsiedler2::loader::LoadDATIDX(const boost::filesystem::path& filepath, Archiv& items,
                                    const ArchivItem_Palette* palette)
{
    if(filepath.empty())
        return ErrorCode::INVALID_BUFFER;

    if(!bfs::exists(filepath))
        return ErrorCode::FILE_NOT_FOUND;

    const bfs::path datFilepath = bfs::path(filepath).replace_extension("DAT");
    const bfs::path idxFilepath = bfs::path(filepath).replace_extension("IDX");
    // Both must exist or it is not a DATIDX file
    if(!bfs::exists(datFilepath) || !bfs::exists(idxFilepath))
        return ErrorCode::WRONG_HEADER;

    MMStream mmapStream, mmapStreamIdx;
    if(int ec = openMemoryStream(datFilepath, mmapStream))
        return ec;

    if(int ec = openMemoryStream(idxFilepath, mmapStreamIdx))
        return ec;

    libendian::EndianIStreamAdapter<false, MMStream&> dat(mmapStream);
    libendian::EndianIStreamAdapter<false, MMStream&> idx(mmapStreamIdx);

    // Anzahl einlesen
    uint32_t count;
    if(!(idx >> count))
        return ErrorCode::WRONG_HEADER;

    // Platz für items anlegen
    items.alloc(count);

    const auto datFileSize = static_cast<uint32_t>(getIStreamSize(dat.getStream()));

    // items einlesen
    for(uint32_t i = 0; i < count; ++i)
    {
        std::array<char, 16> name;
        uint32_t offset;
        std::array<uint8_t, 6> unknown;
        int16_t idxbobtype;
        int16_t bobtype_s;

        idx >> name >> offset >> unknown >> idxbobtype;
        if(!idx)
            return ErrorCode::UNEXPECTED_EOF;
        if(offset + sizeof(bobtype_s) > datFileSize)
            return ErrorCode::WRONG_FORMAT;

        // Zum Offset springen
        dat.setPosition(offset);

        // BobType einlesen
        dat >> bobtype_s;

        if(!dat)
            return ErrorCode::UNEXPECTED_EOF;

        if(idxbobtype != bobtype_s)
        {
            assert(false); // Is this even valid?
            continue;
        }
        auto bobtype = static_cast<BobType>(bobtype_s);

        // Daten von Item auswerten
        std::unique_ptr<ArchivItem> item;
        if(int ec = LoadType(bobtype, dat.getStream(), item, palette))
            return ec;

        // Name setzen
        if(item)
            item->setName(std::string(name.begin(), name.end()));
        items.set(i, std::move(item));
    }

    return ErrorCode::NONE;
}
