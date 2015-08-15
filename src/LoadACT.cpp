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
#include "ArchivItem_Palette.h"
#include "ArchivInfo.h"
#include "prototypen.h"
#include "types.h"
#include <fstream>

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
int libsiedler2::loader::LoadACT(const std::string& file, ArchivInfo& items)
{
     if(file.empty())
        return 1;

    // Datei zum lesen öffnen
    std::ifstream act(file, std::ios_base::binary);

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
