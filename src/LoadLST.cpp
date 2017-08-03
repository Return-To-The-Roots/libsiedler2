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
#include "ArchivInfo.h"
#include "prototypen.h"
#include "ErrorCodes.h"
#include "OpenMemoryStream.h"
#include "libendian/src/EndianIStreamAdapter.h"

/**
 *  lädt eine LST-File in ein ArchivInfo.
 *
 *  @param[in]  file    Dateiname der LST-File
 *  @param[in]  palette Grundpalette der LST-File
 *  @param[out] items   ArchivInfo-Struktur, welche gefüllt wird
 *
 *  @return Null bei Erfolg, ein Wert ungleich Null bei Fehler
 */
int libsiedler2::loader::LoadLST(const std::string& file, ArchivInfo& items, const ArchivItem_Palette* palette)
{
    MMStream mmapStream;
    if(int ec = openMemoryStream(file, mmapStream))
        return ec;

    libendian::EndianIStreamAdapter<false, MMStream& > lst(mmapStream);

    uint16_t header;
    uint32_t count;

    // Header einlesen
    lst >> header;

    // ist es eine GER/ENG-File? (Header 0xE7FD)
    if(header == 0xFDE7)
        return LoadTXT(file, items);

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
            items.push(NULL);
            continue;
        }

        // bobtype des Items einlesen
        lst >> bobtype_s;
        BobType bobtype = static_cast<BobType>(bobtype_s);

        // Daten von Item auswerten
        ArchivItem* item;
        if(int ec = LoadType(bobtype, lst.getStream(), item, palette))
            return ec;
        items.push(item);
    }

    return ErrorCode::NONE;
}
