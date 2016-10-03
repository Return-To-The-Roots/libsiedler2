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
#include "ArchivItem_Palette.h"
#include "ArchivInfo.h"
#include "prototypen.h"
#include "libsiedler2.h"
#include "IAllocator.h"
#include "EndianStream.h"
#include <boost/iostreams/device/mapped_file.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/filesystem.hpp>
#include <sstream>
#include <iostream>

/**
 *  lädt eine BBM-File in ein ArchivInfo.
 *
 *  @param[in]  file    Dateiname der BBM-File
 *  @param[out] items   ArchivInfo-Struktur, welche gefüllt wird
 *
 *  @return Null bei Erfolg, ein Wert ungleich Null bei Fehler
 */
int libsiedler2::loader::LoadBBM(const std::string& file, ArchivInfo& items)
{
    char header[4], pbm[4], chunk[4];
    unsigned int i = 0;

    if(file.empty())
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
    libendian::EndianIStream<true, MMStream& > fs(mmapStream);

    // hat das geklappt?
    if(!fs)
        return 2;

    // Header einlesen
    fs >> header;

    // ist es eine BBM-File? (Header "FORM")
    if(strncmp(header, "FORM", 4) != 0)
        return 4;

    // Länge einlesen
    unsigned length;
    fs >> length;

    // Typ einlesen
    fs >> pbm;

    // ist es eine BBM-File? (Typ "PBM ")
    if(strncmp(pbm, "PBM ", 4) != 0)
        return 7;

    // Chunks einlesen
    while(fs.readNoExcept(chunk, sizeof(chunk)))
    {
        if(strncmp(chunk, "CMAP", 4) == 0)
        {
            fs >> length;

            // Bei ungerader Zahl aufrunden
            if(length & 1)
                ++length;

            // Ist Länge wirklich so groß wie Farbtabelle?
            if(length != 256 * 3)
                return 10;

            // Daten von Item auswerten
            ArchivItem_Palette* palette = (ArchivItem_Palette*)getAllocator().create(BOBTYPE_PALETTE);
            items.push(palette);

            boost::filesystem::path filePath(file);
            if(filePath.has_filename())
            {
                std::stringstream rName;
                rName << filePath.filename().string() << "(" << i << ")";
                palette->setName(rName.str());
            }

            palette->load(fs.getStream(), false);

            ++i;
        }else
        {
            fs >> length;

            // Bei ungerader Zahl aufrunden
            if(length & 1)
                ++length;

            fs.ignore(length);
        }
    }

    if(items.empty() || !fs.isEOF())
        return 14;

    // alles ok
    return 0;
}
