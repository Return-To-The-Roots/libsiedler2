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
#include "ArchivItem.h"
#include "ErrorCodes.h"
#include "OpenMemoryStream.h"
#include "prototypen.h"
#include "libendian/EndianIStreamAdapter.h"
#include <boost/filesystem.hpp>

/**
 *  lädt eine DAT/IDX-File in ein Archiv.
 *
 *  @param[in]  file    Dateiname der DAT/IDX-File
 *  @param[in]  palette Grundpalette der DAT/IDX-File
 *  @param[out] items   Archiv-Struktur, welche gefüllt wird
 *
 *  @return Null bei Erfolg, ein Wert ungleich Null bei Fehler
 */
int libsiedler2::loader::LoadDATIDX(const std::string& file, Archiv& items, const ArchivItem_Palette* palette)
{
    if(file.empty())
        return ErrorCode::INVALID_BUFFER;

    if(!bfs::exists(file))
        return ErrorCode::FILE_NOT_FOUND;

    bfs::path filepath = file;
    bfs::path datFilepath = filepath.replace_extension("DAT");
    bfs::path idxFilepath = filepath.replace_extension("IDX");
    // Both must exist or it is not a DATIDX file
    if(!bfs::exists(datFilepath) || !bfs::exists(idxFilepath))
        return ErrorCode::WRONG_HEADER;

    MMStream mmapStream, mmapStreamIdx;
    if(int ec = openMemoryStream(datFilepath.string(), mmapStream))
        return ec;

    if(int ec = openMemoryStream(idxFilepath.string(), mmapStreamIdx))
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
        char name[16];
        uint32_t offset;
        uint8_t unknown[6];
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
        ArchivItem* item;
        if(int ec = LoadType(bobtype, dat.getStream(), item, palette))
            return ec;

        // Name setzen
        if(item)
            item->setName(std::string(name, sizeof(name)));
        items.set(i, item);
    }

    return ErrorCode::NONE;
}
