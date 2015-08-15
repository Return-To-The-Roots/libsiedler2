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
#include "ArchivItem_Palette.h"
#include "ArchivInfo.h"
#include "prototypen.h"
#include "types.h"
#include <fstream>
#include <EndianStream.h>
#include <sstream>
#include <cstring>

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
int libsiedler2::loader::LoadBBM(const std::string& file, ArchivInfo& items)
{
    char header[4], pbm[4], chunk[4];
    unsigned int i = 0;

    if(file.empty())
        return 1;

    // Datei zum lesen öffnen
    libendian::BigEndianIFStream fs(file);

    // hat das geklappt?
    if(!fs)
        return 2;

    // Header einlesen
    fs >> header;

    // ist es eine BBM-File? (Header "FORM")
    if(strncmp(header, "FORM", 4) != 0)
        return 4;

    // Länge einlesen
    unsigned length;
    fs >> length;

    // Typ einlesen
    fs >> pbm;

    // ist es eine BBM-File? (Typ "PBM ")
    if(strncmp(pbm, "PBM ", 4) != 0)
        return 7;

    // Chunks einlesen
    while(fs.readNoExcept(chunk, sizeof(chunk)))
    {
        if(strncmp(chunk, "CMAP", 4) == 0)
        {
            fs >> length;

            // Bei ungerader Zahl aufrunden
            if(length & 1)
                ++length;

            // Ist Länge wirklich so groß wie Farbtabelle?
            if(length != 256 * 3)
                return 10;

            // Daten von Item auswerten
            ArchivItem_Palette* palette = (ArchivItem_Palette*)getAllocator().create(BOBTYPE_PALETTE);
            items.push(palette);

            size_t namePos = file.find_last_of('/');
            if(namePos != std::string::npos)
            {
                std::stringstream rName;
                rName << file.substr(namePos+1) << "(" << i << ")";
                palette->setName(rName.str());
            }

            palette->load(fs.getStream(), false);

            ++i;
        }else
        {
            fs >> length;

            // Bei ungerader Zahl aufrunden
            if(length & 1)
                ++length;

            fs.ignore(length);
        }
    }

    if(items.size() == 0 || !fs.isEOF())
        return 14;

    // alles ok
    return 0;
}
