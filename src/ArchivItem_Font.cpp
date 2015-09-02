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
#include "ArchivItem_Font.h"
#include "prototypen.h"
#include "types.h"
#include <fstream>
#include <EndianStream.h>

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#if defined _WIN32 && defined _DEBUG && defined _MSC_VER
#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////////
/** @class libsiedler2::ArchivItem_Font
 *
 *  Klasse für Fonts.
 *
 *  @author FloSoft
 */

///////////////////////////////////////////////////////////////////////////////
/** @var libsiedler2::ArchivItem_Font::dx
 *
 *  X-Buchstabenabstand.
 *
 *  @author FloSoft
 */

///////////////////////////////////////////////////////////////////////////////
/** @var libsiedler2::ArchivItem_Font::dy
 *
 *  Y-Buchstabenabstand.
 *
 *  @author FloSoft
 */

///////////////////////////////////////////////////////////////////////////////
/**
 *  Konstruktor von @p ArchivItem_Font.
 *
 *  @author FloSoft
 */
libsiedler2::ArchivItem_Font::ArchivItem_Font(void) : ArchivItem(), ArchivInfo(), dx(0), dy(0)
{
    setBobType(BOBTYPE_FONT);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Kopierkonstruktor von @p ArchivItem_Font.
 *
 *  @param[in] item Quellitem
 *
 *  @author FloSoft
 */
libsiedler2::ArchivItem_Font::ArchivItem_Font(const ArchivItem_Font& item) : ArchivItem( item ), ArchivInfo( item ), dx(item.dx), dy(item.dy)
{
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  lädt die Fontdaten aus einer Datei.
 *
 *  @param[in] file    Dateihandle der Datei
 *  @param[in] palette Grundpalette
 *
 *  @return liefert Null bei Erfolg, ungleich Null bei Fehler
 *
 *  @author FloSoft
 */
int libsiedler2::ArchivItem_Font::load(std::istream& file, const ArchivItem_Palette* palette)
{
    if(!file || palette == NULL)
        return 1;

    libendian::LittleEndianIStreamRef fs(file);
    // Spacing einlesen
    fs >> dx >> dy;

    // Speicher für Buchstaben alloziieren
    alloc(256);

    // Buchstaben einlesen
    for(unsigned long i = 32; i < 256; ++i)
    {
        short bobtype_s;

        // bobtype des Items einlesen
        fs >> bobtype_s;
        BOBTYPES bobtype = static_cast<BOBTYPES>(bobtype_s);

        if(bobtype == BOBTYPE_NONE)
            continue;

        // Daten von Item auswerten
        ArchivItem* item;
        if(loader::LoadType(bobtype, file, palette, item) != 0)
            return 5;
        set(i, item);
    }

    return 0;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  schreibt die Fontdaten in eine Datei.
 *
 *  @param[in] file    Dateihandle der Datei
 *  @param[in] palette Grundpalette
 *
 *  @return liefert Null bei Erfolg, ungleich Null bei Fehler
 *
 *  @author FloSoft
 */
int libsiedler2::ArchivItem_Font::write(std::ostream& file, const ArchivItem_Palette* palette) const
{
    if(!file || palette == NULL)
        return 1;

    libendian::LittleEndianOStreamRef fs(file);
    
    // Spacing schreiben
    fs << dx << dy;

    // Buchstaben schreiben
    for(unsigned long i = 32; i < 256; ++i)
    {
        const ArchivItem* item = get(i);
        BOBTYPES bobtype = BOBTYPE_NONE;

        if(item)
            bobtype = item->getBobType();

        // bobtype des Items schreiben
        fs << bobtype;

        if(item == NULL)
            continue;

        // Daten von Item auswerten
        if(loader::WriteType(bobtype, file, palette, *item) != 0)
            return 5;
    }

    return 0;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  liefert den X-Buchstabenabstand.
 *
 *  @return liefert den X-Buchstabenabstand.
 *
 *  @author FloSoft
 */

///////////////////////////////////////////////////////////////////////////////
/**
 *  liefert den Y-Buchstabenabstand.
 *
 *  @return liefert den Y-Buchstabenabstand.
 *
 *  @author FloSoft
 */

///////////////////////////////////////////////////////////////////////////////
/**
 *  setzt den X-Buchstabenabstand.
 *
 *  @param[in] dx X-Buchstabenabstand
 *
 *  @author FloSoft
 */

///////////////////////////////////////////////////////////////////////////////
/**
 *  setzt den Y-Buchstabenabstand.
 *
 *  @param[in] dy Y-Buchstabenabstand
 *
 *  @author FloSoft
 */
