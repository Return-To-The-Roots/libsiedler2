// $Id: WriteLST.cpp 9359 2014-04-25 15:37:22Z FloSoft $
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
#include "ArchivInfo.h"
#include "prototypen.h"
#include <libendian.h>
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
 *  schreibt ein ArchivInfo in eine LST-File.
 *
 *  @param[in] file    Dateiname der LST-File
 *  @param[in] palette Grundpalette der LST-File
 *  @param[in] items   ArchivInfo-Struktur, welche gefüllt wird
 *
 *  @return Null bei Erfolg, ein Wert ungleich Null bei Fehler
 *
 *  @author FloSoft
 */
int libsiedler2::loader::WriteLST(const std::string& file, const ArchivItem_Palette* palette, const ArchivInfo& items)
{
    short header = 0x204E;
    unsigned int count = items.size();

    if(file.empty())
        return 1;

    // Datei zum schreiben öffnen
    boost::scoped_ptr<FILE> lst(fopen(file.c_str(), "wb"));

    // hat das geklappt?
    if(!lst)
        return 2;

    // Header schreiben
    if(libendian::be_write_s(header, lst.get()) != 0)
        return 3;

    // Anzahl schreiben
    if(libendian::le_write_ui(count, lst.get()) != 0)
        return 5;

    // items schreiben
    for(unsigned int i = 0; i < count; ++i)
    {
        unsigned short used = 0x0100;

        const ArchivItem* item = items.get(i);

        if(item == NULL)
            used = 0x0000;

        // use-Flag schreiben
        if(libendian::be_write_us(used, lst.get()) != 0)
            return 6;

        if(!item)
            continue;

        BOBTYPES bobtype = item->getBobType();

        // bobtype des Items schreiben
        if(libendian::le_write_us(bobtype, lst.get()) != 0)
            return 7;

        // Daten von Item schreiben
        if(WriteType(bobtype, lst.get(), palette, *item) != 0)
            return 8;
    }

    // alles ok
    return 0;
}
