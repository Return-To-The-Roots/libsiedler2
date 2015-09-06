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
#include "types.h"
#include "archives.h"
#include "prototypen.h"
#include <algorithm>
#include <stdexcept>

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

namespace libsiedler2{

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
TEXTURFORMAT texturformat;

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
TEXTURFORMAT setTextureFormat(TEXTURFORMAT format)
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
TEXTURFORMAT getTextureFormat(void)
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
IAllocator* allocator = new StandardAllocator();
const IAllocator& getAllocator()
{
    return *allocator;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Setzt den Item-Allocator.
 *
 *  @param[in] new_allocator Der neue Item-Allokator
 *
 *  @author FloSoft
 */
void setAllocator(IAllocator* newAllocator)
{
    delete allocator;
    allocator = newAllocator;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Der Standard-Item-Allocator.
 *
 *  @param[in] type    Der Typ des Items
 *  @param[in] subtype Der Subtyp des Items
 *
 *  @author FloSoft
 */
ArchivItem* StandardAllocator::create(BOBTYPES type, SOUNDTYPES subtype) const
{
    switch(type)
    {
        case BOBTYPE_SOUND: // WAVs, MIDIs
        {
            switch(subtype)
            {
                case SOUNDTYPE_NONE:
                    return NULL;
                case SOUNDTYPE_MIDI: // MIDI
                    return new ArchivItem_Sound_Midi();
                case SOUNDTYPE_WAVE: // WAV
                    return new ArchivItem_Sound_Wave();
                case SOUNDTYPE_XMIDI: // XMIDI
                    return new ArchivItem_Sound_XMidi();
                case SOUNDTYPE_OTHER: // Andere
                    return new ArchivItem_Sound_Other();
            }
            break;
        }
        case BOBTYPE_BITMAP_RLE: // RLE komprimiertes Bitmap
            return new ArchivItem_Bitmap_RLE();
        case BOBTYPE_FONT: // Font
            return new ArchivItem_Font();
        case BOBTYPE_BITMAP_PLAYER: // Bitmap mit spezifischer Spielerfarbe
            return new ArchivItem_Bitmap_Player();
        case BOBTYPE_PALETTE: // Palette
            return new ArchivItem_Palette();
        case BOBTYPE_BOB: // Bobfiles
            return new ArchivItem_Bob();
        case BOBTYPE_BITMAP_SHADOW: // Schatten
            return new ArchivItem_Bitmap_Shadow();
        case BOBTYPE_MAP: // Mapfiles
            return new ArchivItem_Map();
        case BOBTYPE_TEXT: // Text
            return new ArchivItem_Text();
        case BOBTYPE_RAW: // Raw-Item
            return new ArchivItem_Raw();
        case BOBTYPE_MAP_HEADER: // Mapheader-Item
            return new ArchivItem_Map_Header();
        case BOBTYPE_INI: // INI-Datei-Item
            return new ArchivItem_Ini();
        case BOBTYPE_BITMAP_RAW: // unkomprimiertes Bitmap
            return new ArchivItem_Bitmap_Raw();
        default:
            return NULL;
    }
    return NULL;
}

/**
 *  Der Standard-Item-Allocator.
 *
 *  @param[in] item    Das zu kopierende Item
 *
 *  @author FloSoft
 */
ArchivItem* StandardAllocator::clone(const ArchivItem& item) const
{
    BOBTYPES type = static_cast<BOBTYPES>(item.getBobType());

    switch(type)
    {
        case BOBTYPE_SOUND: // WAVs, MIDIs
        {
            SOUNDTYPES subtype = static_cast<SOUNDTYPES>(dynamic_cast<const ArchivItem_Sound&>(item).getType());

            switch(subtype)
            {
                case SOUNDTYPE_NONE:
                    return NULL;
                case SOUNDTYPE_MIDI: // MIDI
                    return new ArchivItem_Sound_Midi( dynamic_cast<const ArchivItem_Sound_Midi&>(item) );
                case SOUNDTYPE_WAVE: // WAV
                    return new ArchivItem_Sound_Wave( dynamic_cast<const ArchivItem_Sound_Wave&>(item) );
                case SOUNDTYPE_XMIDI: // XMIDI
                    return new ArchivItem_Sound_XMidi( dynamic_cast<const ArchivItem_Sound_XMidi&>(item) );
                case SOUNDTYPE_OTHER: // Andere
                    return new ArchivItem_Sound_Other( dynamic_cast<const ArchivItem_Sound_Other&>(item) );
            }
            break;
        }
        case BOBTYPE_BITMAP_RLE: // RLE komprimiertes Bitmap
            return new ArchivItem_Bitmap_RLE( dynamic_cast<const ArchivItem_Bitmap_RLE&>(item) );
        case BOBTYPE_FONT: // Font
            return new ArchivItem_Font( dynamic_cast<const ArchivItem_Font&>(item) );
        case BOBTYPE_BITMAP_PLAYER: // Bitmap mit spezifischer Spielerfarbe
            return new ArchivItem_Bitmap_Player( dynamic_cast<const ArchivItem_Bitmap_Player&>(item) );
        case BOBTYPE_PALETTE: // Palette
            return new ArchivItem_Palette( dynamic_cast<const ArchivItem_Palette&>(item) );
        case BOBTYPE_BOB: // Bobfiles
            return new ArchivItem_Bob( dynamic_cast<const ArchivItem_Bob&>(item) );
        case BOBTYPE_BITMAP_SHADOW: // Schatten
            return new ArchivItem_Bitmap_Shadow( dynamic_cast<const ArchivItem_Bitmap_Shadow&>(item) );
        case BOBTYPE_MAP: // Mapfiles
            return new ArchivItem_Map( dynamic_cast<const ArchivItem_Map&>(item) );
        case BOBTYPE_TEXT: // Text
            return new ArchivItem_Text( dynamic_cast<const ArchivItem_Text&>(item) );
        case BOBTYPE_RAW: // Raw-Item
            return new ArchivItem_Raw( dynamic_cast<const ArchivItem_Raw&>(item) );
        case BOBTYPE_MAP_HEADER: // Mapheader-Item
            return new ArchivItem_Map_Header( dynamic_cast<const ArchivItem_Map_Header&>(item) );
        case BOBTYPE_INI: // INI-Datei-Item
            return new ArchivItem_Ini( dynamic_cast<const ArchivItem_Ini&>(item) );
        case BOBTYPE_BITMAP_RAW: // unkomprimiertes Bitmap
            return new ArchivItem_Bitmap_Raw( dynamic_cast<const ArchivItem_Bitmap_Raw&>(item) );
        default:
            return NULL;
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
int Load(const std::string& file, ArchivInfo& items, const ArchivItem_Palette* palette)
{
    if(file.empty())
        return 1;

    // Endung holen
    size_t extPos = file.find_last_of('.');
    if(extPos == std::string::npos)
        return 2;
    std::string extension = file.substr(extPos + 1);
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

    int ret = 254;

    try{
        // Datei laden
        if(extension == "act")
            ret = loader::LoadACT(file, items);
        else if(extension == "bbm")
            ret = loader::LoadBBM(file, items);
        else if(extension == "bmp")
        {
            items.clear();
            ArchivItem *bmp, *palette;
            ret = loader::LoadBMP(file, bmp, &palette);
            items.push(bmp);
            items.push(palette);
        }
        else if(extension == "bob")
            ret = loader::LoadBOB(file, palette, items);
        else if(extension == "dat" || extension == "idx")
            ret = loader::LoadDATIDX(file, palette, items);
        else if(extension == "lbm")
            ret = loader::LoadLBM(file, items);
        else if(extension == "lst")
            ret = loader::LoadLST(file, palette, items);
        else if(extension == "tlst")
            ret = loader::LoadTLST(file, items);
        else if(extension == "swd" || extension == "wld")
            ret = loader::LoadMAP(file, items);
        else if(extension == "ger" || extension == "eng")
            ret = loader::LoadTXT(file, items);
        else if(extension == "ini")
            ret = loader::LoadINI(file, items);
    }catch(std::runtime_error){
        // Mostly error on reading (e.g. unexpected end of file)
        return 999;
    }

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
int Write(const std::string& file, const ArchivInfo& items, const ArchivItem_Palette* palette)
{
    if(file.empty())
        return 1;

    // String kopieren und auf Lower-Case bringen
    std::string filen(file);
    std::transform(filen.begin(), filen.end(), filen.begin(), ::tolower);

    // Endung holen
    size_t extPos = filen.find_last_of('.');
    if(extPos == std::string::npos)
        return 2;
    std::string extension = filen.substr(extPos + 1);

    int ret = 0;

    try{
        // Datei schreiben
        if(extension == "act")
            ret = loader::WriteACT(file, items);
        else if(extension == "bbm")
            ret = loader::WriteBBM(file, items);
        else if(extension == "bmp")
            ret = loader::WriteBMP(file, palette, items);
        /*else if(strncmp(endung, "bob", 3)==0)
            ret = loader::WriteBOB(file, palette, items);*/
        /*else if(strncmp(endung, "dat", 3)==0 || strncmp(endung, "idx", 3)==0)
            ret = loader::WriteDATIDX(file, palette, items);*/
        /*else if(strncmp(endung, "lbm", 3)==0)
            ret = loader::WriteLBM(file, items);*/
        else if(extension == "lst")
            ret = loader::WriteLST(file, palette, items);
        /*else if(strncmp(endung, "swd", 3)==0 || strncmp(endung, "wld", 3)==0)
            ret = loader::WriteMAP(file, items);*/
        else if(extension == "ger" || extension == "eng")
            ret = loader::WriteTXT(file, items, false);
        else if(extension == "ini")
            ret = loader::WriteINI(file, items);
    }catch(std::runtime_error)
    {
        // Mostly error on write to file
        return 999;
    }

    return ret;
}

} // namespace libsiedler2
