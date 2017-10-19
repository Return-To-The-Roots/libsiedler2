// Copyright (c) 2005 - 2017 Settlers Freaks (sf-team at siedler25.org)
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
#include "Archiv.h"
#include "ErrorCodes.h"
#include "StandardAllocator.h"
#include "prototypen.h"
#include <boost/filesystem.hpp>
#include <algorithm>
#include <iostream>
#include <stdexcept>

/** @mainpage libsiedler2
 *
 *  libsiedler2 ist eine Grafikbibliothek zum einfachen Zugriff auf
 *  die Dateiformate von Siedler II.
 *
 *  Sie kann natürlich auch verschiedene andere Formate lesen und schreiben,
 *  damit man die verschiedenen Formate hin und her konvertieren kann.
 *
 */

namespace libsiedler2 {

/**
 *  Das gewählte Texturformat.

 *
 *  @ingroup textureformat
 */
static TextureFormat texturformat;

/**
 *  Der gesetzte Item-Allokator.
 */
static IAllocator* allocator = NULL;

} // namespace libsiedler2

namespace {
struct Initializer
{
    Initializer()
    {
        assert(libsiedler2::allocator == NULL);
        libsiedler2::setAllocator(new libsiedler2::StandardAllocator());
        libsiedler2::setGlobalTextureFormat(libsiedler2::FORMAT_BGRA);
    }
    ~Initializer() { libsiedler2::setAllocator(NULL); }
};
static Initializer initializer__;
} // namespace

/** @namespace libsiedler2
 *
 *  @brief Hauptnamensraum von @p libsiedler2
 *
 *  Enthält alle Klassen und exportierten Funktionen von @p libsiedler2.
 */

namespace libsiedler2 {

/**
 *  Setzt das verwendete Texturausgabeformat.
 *
 *  @param[in] format gewünschte Format
 *
 *  @return liefert das vorherige Texturausgabeformat zurück
 */
TextureFormat setGlobalTextureFormat(TextureFormat format)
{
    // altes Texturformat sichern
    TextureFormat old = texturformat;

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
TextureFormat getGlobalTextureFormat()
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
 *  @param[out] items   Archiv-Struktur, welche gefüllt wird
 *  @param[in]  palette Palette, welche benutzt werden soll
 *
 *  @return Null bei Erfolg, ein Wert ungleich Null bei Fehler
 */
int Load(const std::string& file, Archiv& items, const ArchivItem_Palette* palette)
{
    if(file.empty())
        return ErrorCode::INVALID_BUFFER;

    bfs::path filePath(file);
    if(!filePath.has_extension())
        return ErrorCode::UNSUPPORTED_FORMAT;
    std::string extension = filePath.extension().string().substr(1);
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

    int ret = ErrorCode::UNSUPPORTED_FORMAT;

    try
    {
        // Datei laden
        if(extension == "act")
            ret = loader::LoadACT(file, items);
        else if(extension == "bbm")
            ret = loader::LoadBBM(file, items);
        else if(extension == "bmp")
            ret = loader::LoadBMP(file, items, palette);
        else if(extension == "bob")
            ret = loader::LoadBOB(file, items, palette);
        else if(extension == "dat" || extension == "idx")
        {
            ret = loader::LoadDATIDX(file, items, palette);
            if(ret && extension == "dat")
                ret = loader::LoadSND(file, items);
        } else if(extension == "lbm")
            ret = loader::LoadLBM(file, items);
        else if(extension == "lst")
            ret = loader::LoadLST(file, items, palette);
        else if(extension == "swd" || extension == "wld")
            ret = loader::LoadMAP(file, items);
        else if(extension == "ger" || extension == "eng")
            ret = loader::LoadTXT(file, items);
        else if(extension == "ini")
            ret = loader::LoadINI(file, items);
        else if(extension == "ogg" || extension == "wav" || extension == "mid" || extension == "midi" || extension == "xmi")
            ret = loader::LoadSND(file, items);
        else
            std::cerr << "Unsupported extension: " << extension << std::endl;
    } catch(std::exception& error)
    {
        std::cerr << "Error while reading: " << error.what() << std::endl;
        // Mostly error on reading (e.g. unexpected end of file)
        return ErrorCode::CUSTOM;
    }

    return ret;
}

/**
 *  Schreibt die Datei im Format ihrer Endung.
 *
 *  @param[in] file    Dateiname der Datei
 *  @param[in] items   Archiv-Struktur, von welcher gelesen wird
 *  @param[in] palette Palette, welche benutzt werden soll
 *
 *  @return Null bei Erfolg, ein Wert ungleich Null bei Fehler
 */
int Write(const std::string& file, const Archiv& items, const ArchivItem_Palette* palette)
{
    if(file.empty())
        return ErrorCode::INVALID_BUFFER;

    bfs::path filePath(file);
    if(!filePath.has_extension())
        return ErrorCode::UNSUPPORTED_FORMAT;
    std::string extension = filePath.extension().string().substr(1);
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

    int ret = ErrorCode::UNSUPPORTED_FORMAT;

    try
    {
        // Datei schreiben
        if(extension == "act")
            ret = loader::WriteACT(file, items);
        else if(extension == "bbm")
            ret = loader::WriteBBM(file, items);
        else if(extension == "bmp")
            ret = loader::WriteBMP(file, items, palette);
        else if(extension == "lst")
            ret = loader::WriteLST(file, items, palette);
        else if(extension == "swd" || extension == "wld")
            ret = loader::WriteMAP(file, items);
        else if(extension == "ger" || extension == "eng")
            ret = loader::WriteTXT(file, items, true);
        else if(extension == "ini")
            ret = loader::WriteINI(file, items);
        else if(extension == "ogg" || extension == "wav" || extension == "mid" || extension == "midi" || extension == "xmi")
            ret = loader::WriteSND(file, items);
        else if(extension == "lbm")
            ret = loader::WriteLBM(file, items, palette);
        else
            std::cerr << "Unsupported extension: " << extension << std::endl;
    } catch(std::exception& error)
    {
        std::cerr << "Error while writing: " << error.what() << std::endl;
        // Mostly error on write to file
        return ErrorCode::CUSTOM;
    }

    return ret;
}

} // namespace libsiedler2
