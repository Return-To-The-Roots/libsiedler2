// $Id: LoadDATIDX.cpp 9359 2014-04-25 15:37:22Z FloSoft $
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

    std::string datfile = file;
    std::string idxfile = file;
    unsigned int count;

    idxfile[idxfile.size() - 3] = 'I';
    idxfile[idxfile.size() - 2] = 'D';
    idxfile[idxfile.size() - 1] = 'X';

    datfile[datfile.size() - 3] = 'D';
    datfile[datfile.size() - 2] = 'A';
    datfile[datfile.size() - 1] = 'T';

    // Datei zum lesen öffnen
    boost::scoped_ptr<FILE> dat(fopen(datfile.c_str(), "rb"));

    // hat das geklappt?
    if(!dat)
        return 2;

    // IDX-Datei zum lesen öffnen
     boost::scoped_ptr<FILE> idx(fopen(idxfile.c_str(), "rb"));

    // hat das geklappt?
    if(!idx)
        return 3;

    // Anzahl einlesen
    if(libendian::le_read_ui(&count, idx.get()) != 0)
        return 5;

    // Platz für items anlegen
    items.alloc(count);

    // items einlesen
    for(unsigned int i = 0; i < count; ++i)
    {
        char name[17];
        unsigned int offset;
        short idxbobtype;
        short bobtype;

        // Name einlesen
        if(libendian::le_read_c(name, 16, idx.get()) != 16)
            return 6;

        // Offset einlesen
        if(libendian::le_read_ui(&offset, idx.get()) != 0)
            return 7;

        // Unbekannte Daten überspringen
        fseek(idx.get(), 6, SEEK_CUR);

        // BobType einlesen
        if(libendian::le_read_s(&idxbobtype, idx.get()) != 0)
            return 8;

        // Zum Offset springen
        fseek(dat.get(), offset, SEEK_SET);

        // BobType einlesen
        if(libendian::le_read_s(&bobtype, dat.get()) != 0)
            return 9;

        if(idxbobtype != bobtype)
            continue;

        // Daten von Item auswerten
        ArchivItem* item;
        if(LoadType(bobtype, dat.get(), palette, item) != 0)
            return 10;

        // Name setzen
        if(item)
            item->setName(name);
        items.set(i, item);
    }

    return 0;
}
