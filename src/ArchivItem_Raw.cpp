// $Id: ArchivItem_Raw.cpp 9359 2014-04-25 15:37:22Z FloSoft $
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
#include "ArchivItem_Raw.h"

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#if defined _WIN32 && defined _DEBUG && defined _MSC_VER
#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////////
/** @class libsiedler2::baseArchivItem_Raw
 *
 *  Basisklasse für Rawdaten.
 *
 *  @author FloSoft
 */

///////////////////////////////////////////////////////////////////////////////
/**
 *  Konstruktor von @p baseArchivItem_Raw.
 *
 *  @author FloSoft
 */
libsiedler2::baseArchivItem_Raw::baseArchivItem_Raw(void)
    : ArchivItem()
{
    setBobType(BOBTYPE_RAW);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Destruktor von @p baseArchivItem_Raw.
 *
 *  @author FloSoft
 */
libsiedler2::baseArchivItem_Raw::~baseArchivItem_Raw(void)
{}

///////////////////////////////////////////////////////////////////////////////
/**
 *  lädt die Rawdaten aus einer Datei.
 *
 *  @param[in] file   Dateihandle der Datei
 *  @param[in] length Länge des Blocks (aus Datei lesen = 0xFFFFFFFF)
 *
 *  @return liefert Null bei Erfolg, ungleich Null bei Fehler
 *
 *  @author FloSoft
 */
int libsiedler2::baseArchivItem_Raw::load(FILE* file, unsigned int length)
{
    if(file == NULL)
        return 1;

    clear();

    if(length == 0xFFFFFFFF)
    {
        if(libendian::le_read_ui(&length, file) != 0)
            return 2;
    }

    data.resize(length);

    if(length > 0)
    {
        if(libendian::le_read_uc(&data.front(), length, file) != (int)length)
            return 3;
    }

    return 0;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  schreibt die Rawdaten in eine Datei.
 *
 *  @param[in] file        Dateihandle der Datei
 *  @param[in] with_length Soll die Blocklänge mitgeschrieben werden?
 *
 *  @return liefert Null bei Erfolg, ungleich Null bei Fehler
 *
 *  @author FloSoft
 */
int libsiedler2::baseArchivItem_Raw::write(FILE* file, bool with_length) const
{
    if(file == NULL)
        return 1;

    if(with_length)
    {
        if(libendian::le_write_ui(data.size(), file) != 0)
            return 2;
    }

    if(data.size() > 0)
    {
        if(libendian::le_write_uc(&data.front(), data.size(), file) != (int)data.size())
            return 3;
    }

    return 0;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  liefert die Länge des Datenblocks.
 *
 *  @author FloSoft
 */
unsigned int libsiedler2::baseArchivItem_Raw::getLength(void) const
{
    return data.size();
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  liefert die Daten zurück (ro).
 *
 *  @author FloSoft
 */
const std::vector<unsigned char>& libsiedler2::baseArchivItem_Raw::getData(void) const
{
    return data;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  liefert die Daten zurück (rw).
 *
 *  @author FloSoft
 */
std::vector<unsigned char>& libsiedler2::baseArchivItem_Raw::getData(void)
{
    return data;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  löscht den Datenblock.
 *
 *  @author FloSoft
 */
void libsiedler2::baseArchivItem_Raw::clear(void)
{
    data.clear();
}
