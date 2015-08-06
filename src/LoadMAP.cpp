// $Id: LoadMAP.cpp 9359 2014-04-25 15:37:22Z FloSoft $
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

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#if defined _WIN32 && defined _DEBUG && defined _MSC_VER
#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////////
/**
 *  lädt eine MAP-File in ein ArchivInfo.
 *
 *  @param[in]  file    Dateiname der MAP-File
 *  @param[out] items   ArchivInfo-Struktur, welche gefüllt wird
 *
 *  @return Null bei Erfolg, ein Wert ungleich Null bei Fehler
 *
 *  @author FloSoft
 */
int libsiedler2::loader::LoadMAP(const char* file, ArchivInfo* items, bool only_header)
{
    FILE* map;

    if(file == NULL || items == NULL)
        return 1;

    // Datei zum lesen öffnen
    map = fopen(file, "rb");

    // hat das geklappt?
    if(map == NULL)
        return 2;

    ArchivItem_Map* item = dynamic_cast<ArchivItem_Map*>(allocator->create(BOBTYPE_MAP, 0));
    if(item->load(map, only_header) != 0){
        delete item;
        return 3;
    }

    // Datei schliessen
    fclose(map);

    items->alloc(1);
    items->set(0, item);

    return 0;
}
