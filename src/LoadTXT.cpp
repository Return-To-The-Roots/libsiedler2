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
#include "ArchivItem_Text.h"
#include "ArchivInfo.h"
#include "prototypen.h"
#include "libsiedler2.h"
#include "IAllocator.h"
#include "EndianStream.h"
#include <boost/iostreams/device/mapped_file.hpp>
#include <boost/iostreams/stream.hpp>
#include <vector>
#include <iostream>

/**
 *  lädt eine GER/ENG-File in ein ArchivInfo.
 *
 *  @param[in]  file       Dateiname der GER/ENG-File
 *  @param[out] items      ArchivInfo-Struktur, welche gefüllt wird
 *  @param[in]  conversion Soll ggf. OEM-Charset in ANSI umgewandelt werden?
 *
 *  @return Null bei Erfolg, ein Wert ungleich Null bei Fehler
 *
 *  @bug Keine Erkennung ob Plain-Text oder "Irgendwas".
 */
int libsiedler2::loader::LoadTXT(const std::string& file, ArchivInfo& items, bool conversion)
{
    short header;

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
    libendian::EndianIStream<false, MMStream& > fs(mmapStream);

    // hat das geklappt?
    if(!fs)
        return 2;

    size_t length = getIStreamSize(fs.getStream());
    assert(length < std::numeric_limits<unsigned>::max());

    // Header einlesen
    fs >> header;

    items.clear();

    // ist es eine TXT-File? (Header 0xE7FD)
    if( header != (short)0xFDE7 )
    {
        // den Header zurückspringen
        fs.setPositionRel(-2);

        ArchivItem_Text* item = (ArchivItem_Text*)getAllocator().create(BOBTYPE_TEXT);
        item->load(fs.getStream(), conversion);

        items.push(item);
    }
    else
    {
        // "archiviert"
        unsigned short count, unknown;
        unsigned int size;

        fs >> count;

        fs >> unknown;

        fs >> size;

        if(size == 0)
            size = static_cast<unsigned>(length);
        else
            size += 10;

        std::vector<unsigned> starts(count);

        // Starts einlesen
        for(unsigned short x = 0; x < count; ++x)
        {
            uint32_t s;
            fs >> s;

            if(s != 0)
                starts[x] = s + 10;
        }

        // Daten einlesen, zwecks Längenbestimmung
        size_t pos = fs.getPosition();
        size_t rest = size - pos;
        std::vector<char> buffer(rest + 1);
        buffer.resize(rest);
        fs >> buffer;
        buffer.push_back(0);

        for(unsigned short x = 0; x < count; ++x)
        {
            unsigned i = starts[x];

            if(i != 0)
            {
                // An Start springen
                fs.setPosition(i);

                // einlesen
                ArchivItem_Text* item = (ArchivItem_Text*)getAllocator().create(BOBTYPE_TEXT);
                assert(i >= pos);
                item->load(fs.getStream(), conversion, (unsigned int)strlen(&buffer[i - pos]));

                items.push(item);
            }
            else
                items.push(getAllocator().create(BOBTYPE_TEXT));
        }
    }

    // alles ok
    return 0;
}
