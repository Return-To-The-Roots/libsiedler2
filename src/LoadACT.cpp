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
#include "main.h" // IWYU pragma: keep
#include "ArchivItem_Palette.h"
#include "ArchivInfo.h"
#include "prototypen.h"
#include "libsiedler2.h"
#include "IAllocator.h"
#include <boost/iostreams/device/mapped_file.hpp>
#include <boost/iostreams/stream.hpp>
#include <iostream>

// Include last!
#include "DebugNew.h" // IWYU pragma: keep

///////////////////////////////////////////////////////////////////////////////
/**
 *  lädt eine ACT-File in ein ArchivInfo.
 *
 *  @param[in]  file    Dateiname der ACT-File
 *  @param[out] items   ArchivInfo-Struktur, welche gefüllt wird
 *
 *  @return Null bei Erfolg, ein Wert ungleich Null bei Fehler
 *
 *  @author FloSoft
 *  @author OLiver
 */
int libsiedler2::loader::LoadACT(const std::string& file, ArchivInfo& items)
{
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
     MMStream act(mmapFile);

    // hat das geklappt?
    if(!act)
        return 2;

    size_t size = getIStreamSize(act);
    // sind es 256*3 Bytes, also somit 8bit-RGB?
    if(size != 256*3)
        return 3;

    ArchivItem_Palette* palette = (ArchivItem_Palette*)getAllocator().create(BOBTYPE_PALETTE);
    if(palette->load(act, false) != 0){
        delete palette;
        return 4;
    }

    // einlesen
    items.clear();
    items.push(palette);

    // Alles OK
    return 0;
}
