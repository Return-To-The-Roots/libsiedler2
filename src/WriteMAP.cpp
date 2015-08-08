// $Id: WriteMAP.cpp 9359 2014-04-25 15:37:22Z FloSoft $
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
 *  schreibt ein ArchivInfo in eine SWD/WSD-File.
 *
 *  @param[in] file    Dateiname der SWD/WSD-File
 *  @param[in] items   ArchivInfo-Struktur, von welcher gelesen wird
 *
 *  @return Null bei Erfolg, ein Wert ungleich Null bei Fehler
 *
 *  @author FloSoft
 */
int libsiedler2::loader::WriteMAP(const std::string& file, const ArchivInfo& items, long nr)
{
    if(file.empty())
        return 1;

    if(nr == -1)
    {
        // Palette in ArchivInfo suchen, erste Palette wird geschrieben
        for(unsigned long i = 0; i < items.size(); ++i)
        {
            if(!items.get(i))
                continue;
            if(items.get(i)->getBobType() == BOBTYPE_MAP)
            {
                nr = i;
                break;
            }

        }
    }

    // Haben wir eine gefunden?
    if(nr == -1)
        return 2;

    // Datei zum lesen öffnen
    boost::scoped_ptr<FILE> map(fopen(file.c_str(), "wb"));

    // hat das geklappt?
    if(!map)
        return 2;

    const ArchivItem_Map* item = dynamic_cast<const ArchivItem_Map*>(items.get(nr));
    if(item->write(map.get()) != 0)
        return 3;

    return 0;
}
