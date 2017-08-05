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
#include "ArchivInfo.h"
#include "ArchivItem_Bob.h"
#include "ErrorCodes.h"
#include "IAllocator.h"
#include "OpenMemoryStream.h"
#include "libsiedler2.h"
#include "prototypen.h"
#include "libendian/src/EndianIStreamAdapter.h"
#include <boost/filesystem/path.hpp>

/**
 *  lädt eine BOB-File in ein ArchivInfo.
 *
 *  @param[in]  file    Dateiname der BOB-File
 *  @param[out] items   ArchivInfo-Struktur, welche gefüllt wird
 *
 *  @return Null bei Erfolg, ein Wert ungleich Null bei Fehler
 */
int libsiedler2::loader::LoadBOB(const std::string& file, ArchivInfo& items, const ArchivItem_Palette* palette)
{
    if(palette == NULL)
        return ErrorCode::PALETTE_MISSING;

    MMStream mmapStream;
    if(int ec = openMemoryStream(file, mmapStream))
        return ec;
    libendian::EndianIStreamAdapter<false, MMStream&> bob(mmapStream);

    // Header einlesen
    uint32_t header;

    bob >> header;

    // ist es eine BOB-File? (Header 0xF601F501)
    if(!bob || header != 0x01F501F6)
        return ErrorCode::WRONG_HEADER;

    ArchivItem_Bob* item = dynamic_cast<ArchivItem_Bob*>(getAllocator().create(BOBTYPE_BOB));

    boost::filesystem::path filePath(file);
    if(filePath.has_filename())
        item->setName(filePath.filename().string());

    if(int ec = item->load(bob.getStream(), palette))
    {
        delete item;
        return ec;
    }

    // Item alloziieren und zuweisen
    items.clear();
    items.push(item);

    return ErrorCode::NONE;
}
