// $Id: LoadBBM.cpp 9359 2014-04-25 15:37:22Z FloSoft $
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
 *  lädt eine BBM-File in ein ArchivInfo.
 *
 *  @param[in]  file    Dateiname der BBM-File
 *  @param[out] items   ArchivInfo-Struktur, welche gefüllt wird
 *
 *  @return Null bei Erfolg, ein Wert ungleich Null bei Fehler
 *
 *  @author FloSoft
 *  @author OLiver
 */
int libsiedler2::loader::LoadBBM(const char* file, ArchivInfo* items)
{
    FILE* bbm;
    char header[4], pbm[4];
    unsigned int chunk;
    unsigned int i = 0;
    unsigned int length;
    long size;

    if(file == NULL || items == NULL)
        return 1;

    // Datei zum lesen öffnen
    bbm = fopen(file, "rb");

    // hat das geklappt?
    if(bbm == NULL)
        return 2;

    fseek(bbm, 0, SEEK_END);
    size = ftell(bbm);
    fseek(bbm, 0, SEEK_SET);

    // Header einlesen
    if(libendian::le_read_c(header, 4, bbm) != 4)
        return 3;

    // ist es eine BBM-File? (Header "FORM")
    if(strncmp(header, "FORM", 4) != 0)
        return 4;

    // Länge einlesen
    if(libendian::le_read_ui(&length, bbm) != 0)
        return 5;

    // Typ einlesen
    if(libendian::le_read_c(pbm, 4, bbm) != 4)
        return 6;

    // ist es eine BBM-File? (Typ "PBM ")
    if(strncmp(pbm, "PBM ", 4) != 0)
        return 7;

    // Chunks einlesen
    while(!feof(bbm) && ftell(bbm) < size)
    {
        // Chunk-Typ einlesen
        if(libendian::be_read_ui(&chunk, bbm) != 0)
            return 8;

        switch(chunk)
        {
            case 0x434D4150: // "CMAP"
            {
                // Länge einlesen
                if(libendian::be_read_ui(&length, bbm) != 0)
                    return 9;

                // Bei ungerader Zahl aufrunden
                if(length & 1)
                    ++length;

                // Ist Länge wirklich so groß wie Farbtabelle?
                if(length != 256 * 3)
                    return 10;

                // Daten von Item auswerten
                ArchivItem_Palette* palette = (ArchivItem_Palette*)allocator->create(BOBTYPE_PALETTE, 0);
                items->push(palette);

                const char* name = strrchr(file, '/');
                if(name)
                {
                    char rname[17];
                    snprintf(rname, 16, "%s(%d)", name + 1, i);
                    palette->setName(rname);
                }

                // Farbpalette lesen
                unsigned char colors[256][3];
                if(libendian::le_read_uc(colors[0], 256 * 3, bbm) != 256 * 3)
                    return 10;

                // Farbpalette zuweisen
                for(unsigned int k = 0; k < 256; ++k)
                    palette->set(k, colors[k][0], colors[k][1], colors[k][2]);

                ++i;
            } break;
            default:
            {
                // Länge einlesen
                if(libendian::be_read_ui(&length, bbm) != 0)
                    return 12;

                // Bei ungerader Zahl aufrunden
                if(length & 1)
                    ++length;

                if(length > 0)
                {
                    unsigned char* buffer = new unsigned char[length];

                    // überspringen
                    if(libendian::le_read_uc(buffer, length, bbm) != (int)length)
                        return 13;

                    delete[] buffer;
                }
            } break;
        }
    }

    // Datei schliessen
    fclose(bbm);

    if(items->getCount() == 0)
        return 14;

    // alles ok
    return 0;
}
