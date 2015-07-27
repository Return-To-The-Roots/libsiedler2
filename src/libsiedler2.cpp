// $Id: libsiedler2.cpp 9359 2014-04-25 15:37:22Z FloSoft $
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
#include "libsiedler2.h"

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#if defined _WIN32 && defined _DEBUG && defined _MSC_VER && defined _MSC_VER
#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////////
/** @mainpage libsiedler2
 *
 *  libsiedler2 ist eine Grafikbibliothek zum einfachen Zugriff auf
 *  die Dateiformate von Siedler II.
 *
 *  Sie kann natürlich auch verschiedene andere Formate lesen und schreiben,
 *  damit man die verschiedenen Formate hin und her konvertieren kann.
 *
 *  @author FloSoft
 *  @author OLiver
 *
 */

///////////////////////////////////////////////////////////////////////////////
/** @namespace libsiedler2
 *
 *  @brief Hauptnamensraum von @p libsiedler2
 *
 *  Enthält alle Klassen und exportierten Funktionen von @p libsiedler2.
 *
 *  @author FloSoft
 */

///////////////////////////////////////////////////////////////////////////////
/** @namespace libsiedler2::loader
 *
 *  Enthält alle Lade und Schreibfunktionen der einzelnen Dateitypen.
 *
 *  @author FloSoft
 */

///////////////////////////////////////////////////////////////////////////////
/**
 *  Das gewählte Texturformat.
 *
 *  @ingroup textureformat
 *
 *  @author FloSoft
 */
libsiedler2::TEXTURFORMAT texturformat;

///////////////////////////////////////////////////////////////////////////////
/**
 *  Setzt das verwendete Texturausgabeformat.
 *
 *  @param[in] format gewünschte Format
 *
 *  @return liefert das vorherige Texturausgabeformat zurück
 *
 *  @author FloSoft
 */
libsiedler2::TEXTURFORMAT libsiedler2::setTextureFormat(TEXTURFORMAT format)
{
    // altes Texturformat sichern
    TEXTURFORMAT old = texturformat;

    // Neues setzen
    texturformat = format;

    // und Altes zurückliefern
    return old;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  liefert das verwendete Texturausgabeformat.
 *
 *  @return liefert das Texturausgabeformat zurück
 *
 *  @author FloSoft
 */
libsiedler2::TEXTURFORMAT libsiedler2::getTextureFormat(void)
{
    // Aktuelles zurückliefern
    return texturformat;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Der gesetzte Item-Allokator.
 *
 *  @author FloSoft
 */
libsiedler2::allocatorType allocator = libsiedler2::StandardAllocator;

///////////////////////////////////////////////////////////////////////////////
/**
 *  Setzt den Item-Allocator.
 *
 *  @param[in] new_allocator Der neue Item-Allokator
 *
 *  @author FloSoft
 */
void libsiedler2::setAllocator(allocatorType new_allocator)
{
    allocator = new_allocator;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Der Standard-Item-Allocator.
 *
 *  @param[in] type    Der Typ des Items
 *  @param[in] subtype Der Subtyp des Items
 *  @param[in] item    Das zu kopierende Item
 *
 *  @author FloSoft
 */
libsiedler2::ArchivItem* libsiedler2::StandardAllocator(unsigned short type, unsigned short subtype, const libsiedler2::ArchivItem* item)
{
    if(item)
        type = item->getBobType();

    switch(type)
    {
        case BOBTYPE_SOUND: // WAVs, MIDIs
        {
            const ArchivItem_Sound* i = dynamic_cast<const ArchivItem_Sound*>(item);
            if(item)
                subtype = i->getType();

            switch(subtype)
            {
                case SOUNDTYPE_NONE:
                {
                    return NULL;
                } break;
                case SOUNDTYPE_MIDI: // MIDI
                {
                    if(!item)
                        return new ArchivItem_Sound_Midi();
                    else
                        return new ArchivItem_Sound_Midi( dynamic_cast<const ArchivItem_Sound_Midi*>(item) );
                } break;
                case SOUNDTYPE_WAVE: // WAV
                {
                    if(!item)
                        return new ArchivItem_Sound_Wave();
                    else
                        return new ArchivItem_Sound_Wave( dynamic_cast<const ArchivItem_Sound_Wave*>(item) );
                } break;
                case SOUNDTYPE_XMIDI: // XMIDI
                {
                    if(!item)
                        return new ArchivItem_Sound_XMidi();
                    else
                        return new ArchivItem_Sound_XMidi( dynamic_cast<const ArchivItem_Sound_XMidi*>(item) );
                } break;
                case SOUNDTYPE_OTHER: // Andere
                {
                    if(!item)
                        return new ArchivItem_Sound_Other();
                    else
                        return new ArchivItem_Sound_Other( dynamic_cast<const ArchivItem_Sound_Other*>(item) );
                } break;
            }
        } break;
        case BOBTYPE_BITMAP_RLE: // RLE komprimiertes Bitmap
        {
            if(!item)
                return new ArchivItem_Bitmap_RLE();
            else
                return new ArchivItem_Bitmap_RLE( dynamic_cast<const ArchivItem_Bitmap_RLE*>(item) );
        } break;
        case BOBTYPE_FONT: // Font
        {
            if(!item)
                return new ArchivItem_Font();
            else
                return new ArchivItem_Font( dynamic_cast<const ArchivItem_Font*>(item) );
        } break;
        case BOBTYPE_BITMAP_PLAYER: // Bitmap mit spezifischer Spielerfarbe
        {
            if(!item)
                return new ArchivItem_Bitmap_Player();
            else
                return new ArchivItem_Bitmap_Player( dynamic_cast<const ArchivItem_Bitmap_Player*>(item) );
        } break;
        case BOBTYPE_PALETTE: // Palette
        {
            if(!item)
                return new ArchivItem_Palette();
            else
                return new ArchivItem_Palette( dynamic_cast<const ArchivItem_Palette*>(item) );
        } break;
        case BOBTYPE_BOB: // Bobfiles
        {
            if(!item)
                return new ArchivItem_Bob();
            else
                return new ArchivItem_Bob( dynamic_cast<const ArchivItem_Bob*>(item) );
        } break;
        case BOBTYPE_BITMAP_SHADOW: // Schatten
        {
            if(!item)
                return new ArchivItem_Bitmap_Shadow();
            else
                return new ArchivItem_Bitmap_Shadow( dynamic_cast<const ArchivItem_Bitmap_Shadow*>(item) );
        } break;
        case BOBTYPE_MAP: // Mapfiles
        {
            if(!item)
                return new ArchivItem_Map();
            else
                return new ArchivItem_Map( dynamic_cast<const ArchivItem_Map*>(item) );
        } break;
        case BOBTYPE_TEXT: // Text
        {
            if(!item)
                return new ArchivItem_Text();
            else
                return new ArchivItem_Text( dynamic_cast<const ArchivItem_Text*>(item) );
        } break;
        case BOBTYPE_RAW: // Raw-Item
        {
            if(!item)
                return new ArchivItem_Raw();
            else
                return new ArchivItem_Raw( dynamic_cast<const ArchivItem_Raw*>(item) );
        } break;
        case BOBTYPE_MAP_HEADER: // Mapheader-Item
        {
            if(!item)
                return new ArchivItem_Map_Header();
            else
                return new ArchivItem_Map_Header( dynamic_cast<const ArchivItem_Map_Header*>(item) );
        } break;
        case BOBTYPE_INI: // INI-Datei-Item
        {
            if(!item)
                return new ArchivItem_Ini();
            else
                return new ArchivItem_Ini( dynamic_cast<const ArchivItem_Ini*>(item) );
        } break;
        case BOBTYPE_BITMAP_RAW: // unkomprimiertes Bitmap
        {
            if(!item)
                return new ArchivItem_Bitmap_Raw();
            else
                return new ArchivItem_Bitmap_Raw( dynamic_cast<const ArchivItem_Bitmap_Raw*>(item) );
        } break;
    }
    return NULL;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Lädt die Datei im Format ihrer Endung.
 *
 *  @param[in]  file    Dateiname der Datei
 *  @param[out] items   ArchivInfo-Struktur, welche gefüllt wird
 *  @param[in]  palette Palette, welche benutzt werden soll
 *
 *  @return Null bei Erfolg, ein Wert ungleich Null bei Fehler
 *
 *  @author FloSoft
 */
int libsiedler2::Load(const char* file, libsiedler2::ArchivInfo* items, const libsiedler2::ArchivItem_Palette* palette)
{
    if(!file)
        return 1;

    // String kopieren und auf Lower-Case bringen
    char* filen = strlwr(strdup(file));

    // Endung holen
    const char* endung = strrchr(filen, '.');
    if(!endung)
        return 2;
    ++endung;

    int ret = 254;

    // Datei laden
    if(strncmp(endung, "act", 3) == 0)
        ret = loader::LoadACT(file, items);
    else if(strncmp(endung, "bbm", 3) == 0)
        ret = loader::LoadBBM(file, items);
    else if(strncmp(endung, "bmp", 3) == 0)
    {
        items->alloc(2);
        ret = loader::LoadBMP(file, items->getP(0), items->getP(1));
    }
    else if(strncmp(endung, "bob", 3) == 0)
        ret = loader::LoadBOB(file, palette, items);
    else if(strncmp(endung, "dat", 3) == 0 || strncmp(endung, "idx", 3) == 0)
        ret = loader::LoadDATIDX(file, palette, items);
    else if(strncmp(endung, "lbm", 3) == 0)
        ret = loader::LoadLBM(file, items);
    else if(strncmp(endung, "lst", 3) == 0)
        ret = loader::LoadLST(file, palette, items);
    else if(strncmp(endung, "tlst", 4) == 0)
        ret = loader::LoadTLST(file, items);
    else if(strncmp(endung, "swd", 3) == 0 || strncmp(endung, "wld", 3) == 0)
        ret = loader::LoadMAP(file, items);
    else if(strncmp(endung, "ger", 3) == 0 || strncmp(endung, "eng", 3) == 0)
        ret = loader::LoadTXT(file, items);
    else if(strncmp(endung, "ini", 4) == 0)
        ret = loader::LoadINI(file, items);

    free(filen);

    return ret;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Schreibt die Datei im Format ihrer Endung.
 *
 *  @param[in] file    Dateiname der Datei
 *  @param[in] items   ArchivInfo-Struktur, von welcher gelesen wird
 *  @param[in] palette Palette, welche benutzt werden soll
 *
 *  @return Null bei Erfolg, ein Wert ungleich Null bei Fehler
 *
 *  @author FloSoft
 */
int libsiedler2::Write(const char* file, const libsiedler2::ArchivInfo* items, const libsiedler2::ArchivItem_Palette* palette)
{
    if(!file)
        return 1;

    // String kopieren und auf Lower-Case bringen
    char* filen = strlwr(strdup(file));

    // Endung holen
    const char* endung = strrchr(filen, '.');
    if(!endung)
        return 2;
    ++endung;

    int ret = 0;

    // Datei schreiben
    if(strncmp(endung, "act", 3) == 0)
        ret = loader::WriteACT(file, items);
    else if(strncmp(endung, "bbm", 3) == 0)
        ret = loader::WriteBBM(file, items);
    else if(strncmp(endung, "bmp", 3) == 0)
        ret = loader::WriteBMP(file, palette, items);
    /*else if(strncmp(endung, "bob", 3)==0)
        ret = loader::WriteBOB(file, palette, items);*/
    /*else if(strncmp(endung, "dat", 3)==0 || strncmp(endung, "idx", 3)==0)
        ret = loader::WriteDATIDX(file, palette, items);*/
    /*else if(strncmp(endung, "lbm", 3)==0)
        ret = loader::WriteLBM(file, items);*/
    else if(strncmp(endung, "lst", 3) == 0)
        ret = loader::WriteLST(file, palette, items);
    /*else if(strncmp(endung, "swd", 3)==0 || strncmp(endung, "wld", 3)==0)
        ret = loader::WriteMAP(file, items);*/
    else if(strncmp(endung, "ger", 3) == 0 || strncmp(endung, "eng", 3) == 0)
        ret = loader::WriteTXT(file, items, false);
    else if(strncmp(endung, "ini", 3) == 0)
        ret = loader::WriteINI(file, items);

    free(filen);

    return ret;
}
