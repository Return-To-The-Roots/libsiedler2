// $Id: LoadLST.cpp 9359 2014-04-25 15:37:22Z FloSoft $
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
#include <boost/scoped_ptr.hpp>

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#if defined _WIN32 && defined _DEBUG && defined _MSC_VER
#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

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
    boost::scoped_ptr<FILE> lst(fopen(file.c_str(), "rb"));

    // hat das geklappt?
    if(!lst)
        return 2;

    // Header einlesen
    if(libendian::be_read_us(&header, lst.get()) != 0)
        return 3;

    // ist es eine GER/ENG-File? (Header 0xE7FD)
    if(header == 0xE7FD)
    {
        return LoadTXT(file, items);
    }

    // ist es eine LST-File? (Header 0x204E)
    if(header != 0x204E)
        return 4;

    // Anzahl einlesen
    if(libendian::le_read_ui(&count, lst.get()) != 0)
        return 5;

    items.clear();

    // items einlesen
    for(unsigned int i = 0; i < count; ++i)
    {
        short used;
        short bobtype;

        // use-Flag einlesen
        if(libendian::be_read_s(&used, lst.get()) != 0)
            return 6;

        // ist das Item benutzt?
        if(used != 0x0100)
        {
            //fprintf(stderr, "unused %04X\n", used);
            items.push(NULL);
            continue;
        }

        // bobtype des Items einlesen
        if(libendian::le_read_s(&bobtype, lst.get()) != 0)
            return 7;

        //fprintf(stderr, "bobtype %d\n", bobtype);

        // Daten von Item auswerten
        ArchivItem* item;
        if(LoadType(bobtype, lst.get(), palette, item) != 0)
            return 8;
        items.push(item);
    }

    // alles ok
    return 0;
}
