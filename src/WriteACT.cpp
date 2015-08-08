// $Id: WriteACT.cpp 9359 2014-04-25 15:37:22Z FloSoft $
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
 *  schreibt ein ArchivInfo in eine ACT-File.
 *
 *  @param[in] file    Dateiname der ACT-File
 *  @param[in] items   ArchivInfo-Struktur, von welcher gelesen wird
 *  @param[in] nr      Nummer der Palette die geschrieben werden soll
                       (@p -1 erste Palette die gefunden wird)
 *
 *  @return Null bei Erfolg, ein Wert ungleich Null bei Fehler
 *
 *  @author FloSoft
 */
int libsiedler2::loader::WriteACT(const char* file, const ArchivInfo* items, long nr)
{
    FILE* act;
    if(file == NULL || items == NULL)
        return 1;

    if(nr == -1)
    {
        // Palette in ArchivInfo suchen, erste Palette wird geschrieben
        for(unsigned long i = 0; i < items->getCount(); ++i)
        {
            if(!items->get(i))
                continue;
            if(items->get(i)->getBobType() == BOBTYPE_PALETTE)
            {
                nr = i;
                break;
            }

        }
    }

    // Haben wir eine gefunden?
    if(nr == -1)
        return 2;

    // Datei zum schreiben öffnen
    act = fopen(file, "wb");

    // hat das geklappt?
    if(act == NULL)
        return 3;

    // Farben schreiben
    ArchivItem_Palette* palette = (ArchivItem_Palette*)items->get(nr);
    if(palette->write(act) != 0)
        return 4;

    // Datei schliessen
    fclose(act);

    // alles ok
    return 0;
}
