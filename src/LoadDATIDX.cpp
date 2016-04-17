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

#include "main.h" // IWYU pragma: keep
#include "ArchivInfo.h"
#include "prototypen.h"
#include "EndianStream.h"
#include <boost/iostreams/device/mapped_file.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/filesystem.hpp>
#include <iostream>

// Include last!
#include "DebugNew.h" // IWYU pragma: keep

///////////////////////////////////////////////////////////////////////////////
/**
 *  lädt eine DAT/IDX-File in ein ArchivInfo.
 *
 *  @param[in]  file    Dateiname der DAT/IDX-File
 *  @param[in]  palette Grundpalette der DAT/IDX-File
 *  @param[out] items   ArchivInfo-Struktur, welche gefüllt wird
 *
 *  @return Null bei Erfolg, ein Wert ungleich Null bei Fehler
 *
 *  @author FloSoft
 *  @author OLiver
 */
int libsiedler2::loader::LoadDATIDX(const std::string& file, const ArchivItem_Palette* palette, ArchivInfo& items)
{
    if(file.empty())
        return 1;

    boost::filesystem::path filePath = file;
    std::string datfile = filePath.replace_extension("DAT").string();
    std::string idxfile = filePath.replace_extension("IDX").string();;
    unsigned int count;

    // Datei zum lesen öffnen
    boost::iostreams::mapped_file_source mmapFile;
    try{
        mmapFile.open(datfile);
    }catch(std::exception& e){
        std::cerr << "Could not open '" << file << "': " << e.what() << std::endl;
        return 2;
    }
    typedef boost::iostreams::stream<boost::iostreams::mapped_file_source> MMStream;
    MMStream mmapStream(mmapFile);
    libendian::EndianIStream<false, MMStream& > dat(mmapStream);

    // hat das geklappt?
    if(!dat)
        return 2;

    // IDX-Datei zum lesen öffnen
    boost::iostreams::mapped_file_source mmapFileIdx;
    try{
        mmapFileIdx.open(idxfile);
    }catch(std::exception& e){
        std::cerr << "Could not open '" << file << "': " << e.what() << std::endl;
        return 2;
    }
    typedef boost::iostreams::stream<boost::iostreams::mapped_file_source> MMStream;
    MMStream mmapStreamIdx(mmapFileIdx);
    libendian::EndianIStream<false, MMStream& > idx(mmapStreamIdx);

    // hat das geklappt?
    if(!idx)
        return 3;

    // Anzahl einlesen
    idx >> count;

    // Platz für items anlegen
    items.alloc(count);

    // items einlesen
    for(unsigned int i = 0; i < count; ++i)
    {
        char name[16];
        unsigned int offset;
        short idxbobtype;
        short bobtype_s;

        // Name einlesen
        idx >> name;

        // Offset einlesen
        idx >> offset;

        // Unbekannte Daten überspringen
        idx.ignore(6);

        // BobType einlesen
        idx >> idxbobtype;

        // Zum Offset springen
        dat.setPosition(offset);

        // BobType einlesen
        dat >> bobtype_s;

        if(idxbobtype != bobtype_s)
            continue;
        BOBTYPES bobtype = static_cast<BOBTYPES>(bobtype_s);

        // Daten von Item auswerten
        ArchivItem* item;
        if(LoadType(bobtype, dat.getStream(), palette, item) != 0)
            return 10;

        // Name setzen
        if(item)
            item->setName(std::string(name, sizeof(name)));
        items.set(i, item);
    }

    return 0;
}
