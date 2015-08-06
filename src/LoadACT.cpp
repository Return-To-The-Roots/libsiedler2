// $Id: LoadACT.cpp 9359 2014-04-25 15:37:22Z FloSoft $
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
int libsiedler2::loader::LoadACT(const char* file, ArchivInfo* items)
{
    FILE* act;
    long size;

    if(file == NULL || items == NULL)
        return 1;

    // Datei zum lesen öffnen
    act = fopen(file, "rb");

    // hat das geklappt?
    if(act == NULL)
        return 2;

    fseek(act, 0, SEEK_END);
    size = ftell(act);
    fseek(act, 0, SEEK_SET);

    // sind es 256*3 Bytes, also somit 8bit-RGB?
    if(size != 768)
        return 3;

    ArchivItem_Palette* palette = (ArchivItem_Palette*)allocator->create(BOBTYPE_PALETTE, 0);
    if(palette->load(act, false) != 0){
        delete palette;
        return 4;
    }

    // einlesen
    items->alloc(1);
    items->set(0, palette);

    // Datei schliessen
    fclose(act);

    // Alles OK
    return 0;
}
