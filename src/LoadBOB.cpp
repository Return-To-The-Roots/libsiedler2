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

///////////////////////////////////////////////////////////////////////////////
// Header
#include "main.h"
#include "ArchivItem_Bob.h"
#include "ArchivItem_Palette.h"
#include "ArchivInfo.h"
#include "prototypen.h"
#include "types.h"
#include <EndianStream.h>
#include <boost/iostreams/device/mapped_file.hpp>
#include <boost/iostreams/stream.hpp>

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#if defined _WIN32 && defined _DEBUG && defined _MSC_VER
#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////////
/**
 *  lädt eine BOB-File in ein ArchivInfo.
 *
 *  @param[in]  file    Dateiname der BOB-File
 *  @param[out] items   ArchivInfo-Struktur, welche gefüllt wird
 *
 *  @return Null bei Erfolg, ein Wert ungleich Null bei Fehler
 *
 *  @author FloSoft
 */
int libsiedler2::loader::LoadBOB(const std::string& file, const ArchivItem_Palette* palette, ArchivInfo& items)
{
    unsigned int header;

    if(file.empty() || palette == NULL)
        return 1;

    // Datei zum lesen öffnen
    boost::iostreams::mapped_file_source mmapFile(file);
    typedef boost::iostreams::stream<boost::iostreams::mapped_file_source> MMStream;
    MMStream mmapStream(mmapFile);
    libendian::EndianIStream<false, MMStream& > bob(mmapStream);

    // hat das geklappt?
    if(!bob)
        return 2;

    // Header einlesen
    bob >> header;

    // ist es eine BOB-File? (Header 0xF601F501)
    if(header != 0x01F501F6)
        return 4;

    ArchivItem_Bob* item = dynamic_cast<ArchivItem_Bob*>(getAllocator().create(BOBTYPE_BOB));

    size_t nameIdx = file.find_last_of('/');
    if(nameIdx != std::string::npos)
        item->setName(file.substr(nameIdx + 1));

    if(item->load(bob.getStream(), palette) != 0){
        delete item;
        return 5;
    }

    // Item alloziieren und zuweisen
    items.clear();
    items.push(item);

    return 0;
}
