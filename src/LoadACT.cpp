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
 *  @param[in]  file    Dateiname der ACT-File
 *  @param[out] items   Archiv-Struktur, welche gefüllt wird
 *
 *  @return Null bei Erfolg, ein Wert ungleich Null bei Fehler
 */
int libsiedler2::loader::LoadACT(const std::string& file, Archiv& items)
{
    MMStream act;
    if(int ec = openMemoryStream(file, act))
        return ec;

    size_t size = getIStreamSize(act);
    // sind es 256*3 Bytes, also somit 8bit-RGB?
    if(size != 256 * 3)
        return ErrorCode::WRONG_HEADER;

    auto palette = getAllocator().create<ArchivItem_Palette>(BOBTYPE_PALETTE);
    if(int ec = palette->load(act, false))
        return ec;

    // einlesen
    items.clear();
    items.push(std::move(palette));

    // Alles OK
    return ErrorCode::NONE;
}
