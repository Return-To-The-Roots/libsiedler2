// $Id: WriteBBM.cpp 9359 2014-04-25 15:37:22Z FloSoft $
//
// Copyright (c) 2005 - 2011 Settlers Freaks (sf-team at siedler25.org)
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
 *  schreibt ein ArchivInfo in eine BBM-File.
 *
 *  @param[in] file    Dateiname der BBM-File
 *  @param[in] items   ArchivInfo-Struktur, von welcher gelesen wird
 *
 *  @return Null bei Erfolg, ein Wert ungleich Null bei Fehler
 *
 *  @author FloSoft
 */
int libsiedler2::loader::WriteBBM(const char* file, const ArchivInfo* items)
{
    FILE* bbm;
    char header[5] = "FORM", pbm[5] = "PBM ", cmap[5] = "CMAP";
    unsigned long count = 0;
    unsigned int length = 0;

    if(file == NULL || items == NULL)
        return 1;

    // Anzahl Paletten in ArchivInfo suchen
    for(unsigned long i = 0; i < items->getCount(); ++i)
    {
        if(!items->get(i))
            continue;
        if(items->get(i)->getBobType() == BOBTYPE_PALETTE)
            ++count;
    }

    // Datei zum schreiben öffnen
    bbm = fopen(file, "wb");

    // hat das geklappt?
    if(bbm == NULL)
        return 2;

    // Header schreiben
    if(libendian::le_write_c(header, 4, bbm) != 4)
        return 3;

    // Länge schreiben
    length = 4 + count * (256 * 3 + 8);
    if(libendian::le_write_ui(length, bbm) != 0)
        return 4;

    // Typ schreiben
    if(libendian::le_write_c(pbm, 4, bbm) != 4)
        return 5;

    for(unsigned long i = 0; i < items->getCount(); ++i)
    {
        ArchivItem_Palette* palette = (ArchivItem_Palette*)items->get(i);
        if(palette->getBobType() == BOBTYPE_PALETTE)
        {
            // Chunk schreiben
            if(libendian::be_write_c(cmap, 4, bbm) != 4)
                return 6;

            // Länge schreiben
            length = 256 * 3;
            if(libendian::be_write_ui(length, bbm) != 0)
                return 7;

            // Farbpalette zuweisen
            unsigned char colors[256][3];
            for(unsigned int k = 0; k < 256; ++k)
                palette->get(k, &colors[k][0], &colors[k][1], &colors[k][2]);

            // Farbpalette schreiben
            if(libendian::le_write_uc(colors[0], 256 * 3, bbm) != 256 * 3)
                return 8;
        }
    }

    // Datei schliessen
    fclose(bbm);

    // alles ok
    return 0;
}
