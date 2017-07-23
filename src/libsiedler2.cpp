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

#include "libSiedler2Defines.h" // IWYU pragma: keep
#include "libsiedler2.h"
#include "StandardAllocator.h"
#include "prototypen.h"
#include "ArchivInfo.h"
#include <boost/filesystem.hpp>
#include <algorithm>
#include <stdexcept>
#include <iostream>

/** @mainpage libsiedler2
 *
 *  libsiedler2 ist eine Grafikbibliothek zum einfachen Zugriff auf
 *  die Dateiformate von Siedler II.
 *
 *  Sie kann natürlich auch verschiedene andere Formate lesen und schreiben,
 *  damit man die verschiedenen Formate hin und her konvertieren kann.
 *
 */

namespace libsiedler2{

    /**
     *  Das gewählte Texturformat.

     *
     *  @ingroup textureformat
     */
    static TEXTURFORMAT texturformat;

    /**
     *  Der gesetzte Item-Allokator.
     */
    static IAllocator* allocator = NULL;

}

namespace {
    struct Initializer{
        Initializer()
        {
            assert(libsiedler2::allocator == NULL);
            libsiedler2::setAllocator(new libsiedler2::StandardAllocator());
        }
        ~Initializer()
        {
            libsiedler2::setAllocator(NULL);
        }
    };
    static Initializer initializer__;
}

/** @namespace libsiedler2
 *
 *  @brief Hauptnamensraum von @p libsiedler2
 *
 *  Enthält alle Klassen und exportierten Funktionen von @p libsiedler2.
 */

namespace libsiedler2{

/**
 *  Setzt das verwendete Texturausgabeformat.
 *
 *  @param[in] format gewünschte Format
 *
 *  @return liefert das vorherige Texturausgabeformat zurück
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

/**
 *  liefert das verwendete Texturausgabeformat.
 *
 *  @return liefert das Texturausgabeformat zurück
 */
TEXTURFORMAT getTextureFormat()
{
    // Aktuelles zurückliefern
    return texturformat;
}


const IAllocator& getAllocator()
{
    return *allocator;
}

/**
 *  Setzt den Item-Allocator.
 *
 *  @param[in] new_allocator Der neue Item-Allokator
 */
void setAllocator(IAllocator* newAllocator)
{
    delete allocator;
    allocator = newAllocator;
}

/**
 *  Lädt die Datei im Format ihrer Endung.
 *
 *  @param[in]  file    Dateiname der Datei
 *  @param[out] items   ArchivInfo-Struktur, welche gefüllt wird
 *  @param[in]  palette Palette, welche benutzt werden soll
 *
 *  @return Null bei Erfolg, ein Wert ungleich Null bei Fehler
 */
int Load(const std::string& file, ArchivInfo& items, const ArchivItem_Palette* palette)
{
    if(file.empty())
        return 1;

    bfs::path filePath(file);
    if(!filePath.has_extension())
        return 2;
    std::string extension = filePath.extension().string().substr(1);
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
        else if(extension == "ogg")
            ret = loader::LoadSND(file, items);
        else
            std::cerr << "Unsupported extension: " << extension << std::endl;
    }catch(std::runtime_error&){
        // Mostly error on reading (e.g. unexpected end of file)
        return 999;
    }

    return ret;
}

/**
 *  Schreibt die Datei im Format ihrer Endung.
 *
 *  @param[in] file    Dateiname der Datei
 *  @param[in] items   ArchivInfo-Struktur, von welcher gelesen wird
 *  @param[in] palette Palette, welche benutzt werden soll
 *
 *  @return Null bei Erfolg, ein Wert ungleich Null bei Fehler
 */
int Write(const std::string& file, const ArchivInfo& items, const ArchivItem_Palette* palette)
{
    if(file.empty())
        return 1;

    bfs::path filePath(file);
    if(!filePath.has_extension())
        return 2;
    std::string extension = filePath.extension().string().substr(1);
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

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
        else if(extension == "swd" || extension == "wld")
            ret = loader::WriteMAP(file, items);
        else if(extension == "ger" || extension == "eng")
            ret = loader::WriteTXT(file, items, false);
        else if(extension == "ini")
            ret = loader::WriteINI(file, items);
        else
            std::cerr << "Unsupported extension: " << extension << std::endl;
    }catch(std::runtime_error&)
    {
        // Mostly error on write to file
        return 999;
    }

    return ret;
}

} // namespace libsiedler2
