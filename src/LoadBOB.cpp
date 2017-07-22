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
#include "ArchivItem_Bob.h"
#include "ArchivInfo.h"
#include "prototypen.h"
#include "libsiedler2.h"
#include "IAllocator.h"
#include "libendian/src/EndianStream.h"
#include <boost/iostreams/device/mapped_file.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/filesystem.hpp>
#include <iostream>

/**
 *  lädt eine BOB-File in ein ArchivInfo.
 *
 *  @param[in]  file    Dateiname der BOB-File
 *  @param[out] items   ArchivInfo-Struktur, welche gefüllt wird
 *
 *  @return Null bei Erfolg, ein Wert ungleich Null bei Fehler
 */
int libsiedler2::loader::LoadBOB(const std::string& file, const ArchivItem_Palette* palette, ArchivInfo& items)
{
    unsigned header;

    if(file.empty() || palette == NULL)
        return 1;

    // Datei zum lesen öffnen
    boost::iostreams::mapped_file_source mmapFile;
    try{
        mmapFile.open(bfs::path(file));
    }catch(std::exception& e){
        std::cerr << "Could not open '" << file << "': " << e.what() << std::endl;
        return 2;
    }
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

    boost::filesystem::path filePath(file);
    if(filePath.has_filename())
        item->setName(filePath.filename().string());

    if(item->load(bob.getStream(), palette) != 0){
        delete item;
        return 5;
    }

    // Item alloziieren und zuweisen
    items.clear();
    items.push(item);

    return 0;
}
