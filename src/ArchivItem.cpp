// $Id: ArchivItem.cpp 9359 2014-04-25 15:37:22Z FloSoft $
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
#include "ArchivItem.h"

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#if defined _WIN32 && defined _DEBUG && defined _MSC_VER
#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////////
/** @class libsiedler2::ArchivItem
 *
 *  Basisklasse der Archivelemente.
 *
 *  @author FloSoft
 */

///////////////////////////////////////////////////////////////////////////////
/** @var libsiedler2::ArchivItem::bobtype
 *
 *  Bobtype des Elements.
 *
 *  @author FloSoft
 */

///////////////////////////////////////////////////////////////////////////////
/** @var libsiedler2::ArchivItem::name
 *
 *  Name des Elements.
 *
 *  @author FloSoft
 */

///////////////////////////////////////////////////////////////////////////////
/**
 *  Konstruktor von @p ArchivItem.
 *
 *  @author FloSoft
 */
libsiedler2::ArchivItem::ArchivItem(void)
{
    bobtype = BOBTYPE_NONE;
    setName("untitled");
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  virtueller Destruktor von @p ArchivItem.
 *
 *  @author FloSoft
 */
libsiedler2::ArchivItem::~ArchivItem(void)
{
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  setzt den Bobtype des Items.
 *
 *  @param[in] bobtype zu setzender Bobtype
 *
 *  @author FloSoft
 */

///////////////////////////////////////////////////////////////////////////////
/**
 *  liefert den BobType des Items.
 *
 *  @return BobType des Items
 *
 *  @author FloSoft
 */

///////////////////////////////////////////////////////////////////////////////
/**
 *  setzt den Namen des Items.
 *
 *  @param[in] name zu setzender Name
 *
 *  @author FloSoft
 */

///////////////////////////////////////////////////////////////////////////////
/**
 *  liefert den Namen des Items.
 *
 *  @return Name des Items
 *
 *  @author FloSoft
 */

///////////////////////////////////////////////////////////////////////////////
/**
 *  lädt die Daten aus einer Datei.
 *
 *  @param[in] file Dateihandle der Datei
 *
 *  @return liefert Null bei Erfolg, ungleich Null bei Fehler
 *
 *  @author FloSoft
 */
int libsiedler2::ArchivItem::load(FILE* file)
{
    return 256;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  schreibt die Daten in eine Datei.
 *
 *  @param[in] file Dateihandle der Datei
 *
 *  @return liefert Null bei Erfolg, ungleich Null bei Fehler
 *
 *  @author FloSoft
 */
int libsiedler2::ArchivItem::write(FILE* file) const
{
    return 256;
}
