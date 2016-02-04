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
#include "ArchivInfo.h"
#include "prototypen.h"
#include <EndianStream.h>
#include <boost/iostreams/device/mapped_file.hpp>
#include <boost/iostreams/stream.hpp>
#include <iostream>

// Include last!
#include "DebugNew.h"

///////////////////////////////////////////////////////////////////////////////
/**
 *  lädt eine LST-File in ein ArchivInfo.
 *
 *  @param[in]  file    Dateiname der LST-File
 *  @param[in]  palette Grundpalette der LST-File
 *  @param[out] items   ArchivInfo-Struktur, welche gefüllt wird
 *
 *  @return Null bei Erfolg, ein Wert ungleich Null bei Fehler
 *
 *  @author FloSoft
 *  @author OLiver
 */
int libsiedler2::loader::LoadLST(const std::string& file, const ArchivItem_Palette* palette, ArchivInfo& items)
{
    unsigned short header;
    unsigned int count;

    if(file.empty())
        return 1;

    // Datei zum lesen öffnen
    boost::iostreams::mapped_file_source mmapFile;
    try{
        mmapFile.open(file);
    }catch(std::exception& e){
        std::cerr << "Could not open '" << file << "': " << e.what() << std::endl;
        return 2;
    }
    typedef boost::iostreams::stream<boost::iostreams::mapped_file_source> MMStream;
    MMStream mmapStream(mmapFile);
    libendian::EndianIStream<false, MMStream& > lst(mmapStream);

    // hat das geklappt?
    if(!lst)
        return 2;

    // Header einlesen
    lst >> header;

    // ist es eine GER/ENG-File? (Header 0xE7FD)
    if(header == 0xFDE7)
    {
        return LoadTXT(file, items);
    }

    // ist es eine LST-File? (Header 0x204E)
    if(header != 0x4E20)
        return 4;

    // Anzahl einlesen
    lst >> count;

    items.clear();

    // items einlesen
    for(unsigned int i = 0; i < count; ++i)
    {
        short used;
        short bobtype_s;

        // use-Flag einlesen
        lst >> used;

        // ist das Item benutzt?
        if(used != 0x0001)
        {
            //fprintf(stderr, "unused %04X\n", used);
            items.push(NULL);
            continue;
        }

        // bobtype des Items einlesen
        lst >> bobtype_s;
        BOBTYPES bobtype = static_cast<BOBTYPES>(bobtype_s);
        //fprintf(stderr, "bobtype %d\n", bobtype);

        // Daten von Item auswerten
        ArchivItem* item;
        if(LoadType(bobtype, lst.getStream(), palette, item) != 0)
            return 8;
        items.push(item);
    }

    // alles ok
    return 0;
}
