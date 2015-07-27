// $Id: WriteTXT.cpp 9359 2014-04-25 15:37:22Z FloSoft $
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
 *  schreibt eine GER/ENG-File aus einem ArchivInfo.
 *
 *  @param[in] file       Dateiname der GER/ENG-File
 *  @param[in] items      ArchivInfo-Struktur, welche geschrieben wird
 *  @param[in] conversion Soll ggf. ANSI-Charset in OEM umgewandelt werden?
 *
 *  @return Null bei Erfolg, ein Wert ungleich Null bei Fehler
 *
 *  @author FloSoft
 */
int libsiedler2::loader::WriteTXT(const char* file, const ArchivInfo* items, bool conversion)
{
    FILE* txt;

    if(file == NULL || items == NULL)
        return 1;

    if(items->getCount() == 0)
        return 2;

    for(unsigned long i = 0; i < items->getCount(); ++i)
    {
        const ArchivItem* item = items->get(i);
        if(item)
        {
            if(item->getBobType() != BOBTYPE_TEXT)
                return 3;
        }
    }

    // Datei zum lesen öffnen
    txt = fopen(file, "wb");

    // hat das geklappt?
    if(txt == NULL)
        return 2;

    // Plain-Text ?
    if(items->getCount() == 1)
    {
        const ArchivItem_Text* item = dynamic_cast<const ArchivItem_Text*>(items->get(0));
        if(item->write(txt, conversion) != 0)
            return 4;
    }
    else
    {
        // "archiviert"
        unsigned short header = 0xE7FD;
        unsigned short count = (unsigned short)items->getCount();
        unsigned short unknown = 0x0001;

        // Header schreiben
        if(libendian::be_write_us(header, txt) != 0)
            return 5;

        // Anzahl schreiben
        if(libendian::le_write_us(count, txt) != 0)
            return 6;

        // Unbekannte Bytes schreiben
        if(libendian::le_write_us(unknown, txt) != 0)
            return 7;

        int* starts = new int[count];
        memset(starts, 0, sizeof(int)*count);

        unsigned int size = count * 4;
        for(unsigned long i = 0; i < count; ++i)
        {
            const ArchivItem_Text* item = dynamic_cast<const ArchivItem_Text*>(items->get(i));

            if(item)
            {
                if(item->getLength() != 0)
                {
                    starts[i] = size;
                    size += item->getLength() + 1;
                }
            }
        }

        // Größe schreiben
        if(libendian::le_write_ui(size, txt) != 0)
            return 8;

        // Starts schreiben
        for(unsigned long i = 0; i < count; ++i)
        {
            if(libendian::le_write_i(starts[i], txt) != 0)
                return 9;
        }

        // Texte schreiben
        for(unsigned long i = 0; i < count; ++i)
        {
            const ArchivItem_Text* item = dynamic_cast<const ArchivItem_Text*>(items->get(i));

            if(item)
            {
                if(item->write(txt, conversion) != 0)
                    return 10;
            }
        }

        delete[] starts;
    }

    // Datei schliessen
    fclose(txt);

    return 0;
}
