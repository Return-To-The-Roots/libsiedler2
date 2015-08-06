// $Id: ArchivItem_Palette.cpp 9359 2014-04-25 15:37:22Z FloSoft $
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

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#if defined _WIN32 && defined _DEBUG && defined _MSC_VER
#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////////
/** @var TRANSPARENT_INDEX
 *
 *  Index des Transparenzwertes.
 *
 *  @author FloSoft
 */

///////////////////////////////////////////////////////////////////////////////
/** @class libsiedler2::ArchivItem_Palette
 *
 *  Klasse für Paletten.
 *
 *  @author FloSoft
 */

///////////////////////////////////////////////////////////////////////////////
/**
 *  Konstruktor von @p ArchivItem_Palette.
 *
 *  @author FloSoft
 */
libsiedler2::ArchivItem_Palette::ArchivItem_Palette() : ArchivItem()
{
    memset(colors, 0, 256 * 3);

    setBobType(5);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Kopierkonstruktor von @p ArchivItem_Palette.
 *
 *  @param[in] item Quellitem
 *
 *  @author FloSoft
 */
libsiedler2::ArchivItem_Palette::ArchivItem_Palette(const ArchivItem_Palette& item) : ArchivItem( item )
{
    setBobType(5);

    memcpy(colors, item.colors, 3 * 256);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Konstruktor von @p ArchivItem_Palette mit Laden der Farbwerte aus
 *  einer Datei.
 *
 *  @param[in] file Dateihandle aus denen die Farbwerte geladen werden sollen
 *  @param[in] skip Sollen 2 Unbekannte Bytes übersprungen werden (bei LST) oder direkt?
 *
 *  @author FloSoft
 */
libsiedler2::ArchivItem_Palette::ArchivItem_Palette(FILE* file, bool skip) : ArchivItem()
{
    setBobType(5);

    load(file, skip);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  liest die Farbwerte aus einer Datei.
 *
 *  @param[in] file Dateihandle aus denen die Farbwerte geladen werden sollen
 *  @param[in] skip Sollen 2 Unbekannte Bytes übersprungen werden (bei LST) oder direkt?
 *
 *  @return liefert Null bei Erfolg, ungleich Null bei Fehler
 *
 *  @author FloSoft
 */
int libsiedler2::ArchivItem_Palette::load(FILE* file, bool skip)
{
    if(file == NULL)
        return 1;

    if(skip)
    {
        // Unbekannte 2 Bytes überspringen
        fseek(file, 2, SEEK_CUR);
    }

    // Farben einlesen
    if(libendian::le_read_uc(colors[0], 256 * 3, file) != 256 * 3)
        return 2;

    // alles ok
    return 0;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  schreibt die Farbwerte in eine Datei
 *
 *  @param[in] file Dateihandle in die die Farbwerte geschrieben werden sollen
 *  @param[in] skip Sollen 2 Unbekannte Bytes geschrieben werden (bei LST) oder direkt?
 *
 *  @return liefert Null bei Erfolg, ungleich Null bei Fehler
 *
 *  @author FloSoft
 */
int libsiedler2::ArchivItem_Palette::write(FILE* file, bool skip) const
{
    if(file == NULL)
        return 1;

    if(skip)
    {
        short unknown = 0x0100;
        if(libendian::le_write_s(unknown, file) != 0)
            return 2;
    }

    // Farben schreiben
    if(fwrite(colors, 3, 256, file) != 256)
        return 3;

    // alles ok
    return 0;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  setzt einen Farbwert am entsprechenden Index.
 *
 *  @param[in] index Index des Farbwertes
 *  @param[in] r     Roter Farbwert
 *  @param[in] g     Grüner Farbwert
 *  @param[in] b     Blauer Farbwert
 *
 *  @author FloSoft
 */
void libsiedler2::ArchivItem_Palette::set(unsigned char index, unsigned char r, unsigned char g, unsigned char b)
{
    colors[index][0] = r;
    colors[index][1] = g;
    colors[index][2] = b;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  liefert einen Farbwert am entsprechenden Index.
 *
 *  @param[in]  index Index des Farbwertes
 *  @param[out] r     Roter Farbwert
 *  @param[out] g     Grüner Farbwert
 *  @param[out] b     Blauer Farbwert
 *
 *  @author FloSoft
 */
void libsiedler2::ArchivItem_Palette::get(unsigned char index, unsigned char* r, unsigned char* g, unsigned char* b) const
{
    if(r)
        *r = colors[index][0];

    if(g)
        *g = colors[index][1];

    if(b)
        *b = colors[index][2];
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  liefert einen Index zum entsprechenden RGB-Wert.
 *
 *  @param[in] r     Roter Farbwert
 *  @param[in] g     Grüner Farbwert
 *  @param[in] b     Blauer Farbwert
 *
 *  @return Farbindex der RGB-Farbe
 *
 *  @bug Keine Fehlererkennung!
 *
 *  @author FloSoft
 */
unsigned char libsiedler2::ArchivItem_Palette::lookup(unsigned char r, unsigned char g, unsigned char b) const
{
    for(unsigned short i = 0; i < 256; ++i)
    {
        if(colors[i][0] == r && colors[i][1] == g && colors[i][2] == b)
            return static_cast<unsigned char>(i);
    }
    return 0;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Index-Operator von @p ArchivItem_Palette.
 *
 *  @param[in] index Index des zu liefernden Eintrags
 *
 *  @return Bei Erfolg einen Pointer auf einen, ansonsten NULL
 *
 *  @author FloSoft
 */
const unsigned char* const libsiedler2::ArchivItem_Palette::operator[](int index)
{
    if( index < 256 && index >= 0)
        return colors[index];

    return NULL;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  kopiert die Palette in einen Puffer (als RGBA)
 *
 *  @param[in,out] buffer Zielpuffer für die Palettendaten (muss 256*4-Byte groß sein)
 *
 *  @author FloSoft
 */
void libsiedler2::ArchivItem_Palette::copy(unsigned char* buffer) const
{
    // Puffer vorbereiten
    memset(buffer, 0xFF, 256 * 4);

    // Farben kopieren
    for(unsigned short i = 0; i < 256; ++i)
    {
        buffer[i * 4] = colors[i][0];
        buffer[i * 4 + 1] = colors[i][1];
        buffer[i * 4 + 2] = colors[i][2];
    }

    // Transparentes Element transparent machen
    buffer[TRANSPARENT_INDEX * 4 + 3] = 0x00;
}
