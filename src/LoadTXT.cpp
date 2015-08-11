// $Id: LoadTXT.cpp 9359 2014-04-25 15:37:22Z FloSoft $
//
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
#include "ArchivItem_Text.h"
#include "ArchivInfo.h"
#include "prototypen.h"
#include "types.h"
#include <libendian.h>
#include <boost/scoped_ptr.hpp>
#include <boost/scoped_array.hpp>
#include <vector>
#include <cstring>

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#if defined _WIN32 && defined _DEBUG && defined _MSC_VER
#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////////
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
 *
 *  @author FloSoft
 */
int libsiedler2::loader::LoadTXT(const std::string& file, ArchivInfo& items, bool conversion)
{
    short header;

    if(file.empty())
        return 1;

    // Datei zum lesen öffnen
    boost::scoped_ptr<FILE> txt(fopen(file.c_str(), "rb"));

    // hat das geklappt?
    if(!txt)
        return 2;

    // Länge bestimmen
    size_t length = getFileLength(txt.get());

    // Header einlesen
    if(libendian::be_read_s(&header, txt.get()) != 0)
        return 3;

    items.clear();

    // ist es eine TXT-File? (Header 0xE7FD)
    if( header != (short)0xE7FD )
    {
        // den Header zurückspringen
        fseek(txt.get(), -2, SEEK_CUR);

        ArchivItem_Text* item = (ArchivItem_Text*)getAllocator().create(BOBTYPE_TEXT, 0);
        item->load(txt.get(), conversion);

        items.push(item);
    }
    else
    {
        // "archiviert"
        unsigned short count, unknown;
        unsigned int size;

        if(libendian::le_read_us(&count, txt.get()) != 0)
            return 4;

        if(libendian::le_read_us(&unknown, txt.get()) != 0)
            return 5;

        if(libendian::le_read_ui(&size, txt.get()) != 0)
            return 6;

        if(size == 0)
            size = length;
        else
            size += 10;

        std::vector<int> starts(count);

        // Starts einlesen
        for(unsigned short x = 0; x < count; ++x)
        {
            int s;
            if(libendian::le_read_i(&s, txt.get()) != 0)
                return 7;

            if(s != 0)
                starts[x] = s + 10;
        }

        // Daten einlesen, zwecks Längenbestimmung
        unsigned int pos = ftell(txt.get());
        unsigned int rest = size - pos;
        boost::scoped_array<char> buffer(new char[rest + 1]);
        if(libendian::le_read_c(buffer.get(), rest, txt.get()) != (int)rest)
            return 8;

        for(unsigned short x = 0; x < count; ++x)
        {
            int i = starts[x];

            if(i != 0)
            {
                // An Start springen
                fseek(txt.get(), i, SEEK_SET);

                // einlesen
                ArchivItem_Text* item = (ArchivItem_Text*)getAllocator().create(BOBTYPE_TEXT, 0);
                item->load(txt.get(), conversion, (unsigned int)strlen(&buffer[i - pos]));

                items.push(item);
            }
            else
                items.push((ArchivItem_Text*)getAllocator().create(BOBTYPE_TEXT, 0));
        }
    }

    // alles ok
    return 0;
}
