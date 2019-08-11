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

#include "libsiedler2.h"
#include "Archiv.h"
#include "ArchivItem_Bitmap.h"
#include "ArchivItem_BitmapBase.h"
#include "ArchivItem_Bitmap_Player.h"
#include "ArchivItem_Font.h"
#include "ErrorCodes.h"
#include "PixelBufferBGRA.h"
#include "StandardAllocator.h"
#include "prototypen.h"
#include "libutil/StringConversion.h"
#include "libutil/Tokenizer.h"
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/filesystem.hpp>
#include <algorithm>
#include <iostream>
#include <memory>
#include <stdexcept>

namespace bfs = boost::filesystem;

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
static IAllocator* allocator = nullptr;

} // namespace libsiedler2

namespace {
struct Initializer
{
    Initializer()
    {
        assert(libsiedler2::allocator == nullptr);
        libsiedler2::setAllocator(new libsiedler2::StandardAllocator());
        libsiedler2::setGlobalTextureFormat(libsiedler2::FORMAT_ORIGINAL);
    }
    ~Initializer() { libsiedler2::setAllocator(nullptr); }
};
Initializer initializer__;
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
    boost::algorithm::to_lower(extension);

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
            if(ret == ErrorCode::WRONG_HEADER && extension == "dat")
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
        else if(extension == "txt")
        {
            bfs::path filename = filePath.stem();
            std::string ext2 = (filename.has_extension()) ? filename.extension().string().substr(1) : filename.string();
            boost::algorithm::to_lower(ext2);
            if(ext2 == "paletteanims")
                ret = loader::LoadPaletteAnim(file, items);
            else if(ext2 == "palette")
                ret = loader::LoadTxtPalette(file, items);
            else
                ret = loader::LoadTXT(file, items);
        } else
            std::cerr << "Unsupported extension: " << extension << std::endl;
    } catch(std::exception& error)
    {
        std::cerr << "Error while reading: " << error.what() << std::endl;
        // Mostly error on reading (e.g. unexpected end of file)
        return ErrorCode::CUSTOM;
    }

    return ret;
}

int LoadFolder(std::vector<FileEntry> folderInfos, Archiv& items, const ArchivItem_Palette* palette)
{
    std::sort(folderInfos.begin(), folderInfos.end());
    libsiedler2::PixelBufferBGRA buffer(1000, 1000);
    for(const FileEntry& entry : folderInfos)
    {
        // Ignore
        if(entry.bobtype == BOBTYPE_UNSET)
            continue;
        std::unique_ptr<ArchivItem> newItem;
        if(entry.bobtype == BOBTYPE_FONT)
        {
            auto font = std::make_unique<ArchivItem_Font>();
            font->isUnicode = boost::algorithm::to_lower_copy(bfs::path(entry.filePath).extension().string()) == ".fonx";
            font->setDx(static_cast<uint8_t>(entry.nx));
            font->setDy(static_cast<uint8_t>(entry.ny));
            int ec;
            if(bfs::is_directory(entry.filePath))
                ec = LoadFolder(ReadFolderInfo(entry.filePath), *font, palette);
            else
                ec = Load(entry.filePath, *font, palette);
            if(ec)
                return ec;

            newItem = std::move(font);
        } else if(entry.bobtype != BOBTYPE_NONE)
        {
            const ArchivItem_Palette* curPal = palette;
            if(entry.nr >= 0)
            {
                if(static_cast<unsigned>(entry.nr) < items.size() && items[entry.nr]->getBobType() == BOBTYPE_PALETTE)
                    curPal = static_cast<const ArchivItem_Palette*>(items[entry.nr]);
            } else if(!items.empty() && items[items.size() - 1u]->getBobType() == BOBTYPE_PALETTE)
                curPal = static_cast<const ArchivItem_Palette*>(items[items.size() - 1u]);
            Archiv tmpItems;
            if(int ec = Load(entry.filePath, tmpItems, curPal))
                return ec;
            if(entry.bobtype == BOBTYPE_BITMAP_PLAYER || entry.bobtype == BOBTYPE_BITMAP_RAW || entry.bobtype == BOBTYPE_BITMAP_RLE
               || entry.bobtype == BOBTYPE_BITMAP_SHADOW)
            {
                if(tmpItems.size() != 1)
                    return ErrorCode::UNSUPPORTED_FORMAT;

                if(entry.bobtype == tmpItems[0]->getBobType())
                {
                    // No conversion->Just take it
                    newItem = tmpItems.release(0);
                } else
                {
                    auto* bmp = dynamic_cast<ArchivItem_BitmapBase*>(tmpItems[0]);
                    if(!bmp)
                        return ErrorCode::UNSUPPORTED_FORMAT;
                    auto convertedBmp = getAllocator().create<ArchivItem_BitmapBase>(entry.bobtype);
                    std::fill(buffer.begin(), buffer.end(), ColorBGRA());
                    if(bmp->getBobType() == BOBTYPE_BITMAP_PLAYER)
                    {
                        auto* bmpPlayer = dynamic_cast<ArchivItem_Bitmap_Player*>(bmp);
                        assert(bmpPlayer);
                        bmpPlayer->print(buffer, curPal);
                    } else
                    {
                        auto* bmpBase = dynamic_cast<baseArchivItem_Bitmap*>(bmp);
                        assert(bmpBase);
                        bmpBase->print(buffer);
                    }

                    switch(entry.bobtype)
                    {
                        case BOBTYPE_BITMAP_RLE:
                        case BOBTYPE_BITMAP_SHADOW:
                        case BOBTYPE_BITMAP_RAW:
                        {
                            auto* bmpBase = dynamic_cast<baseArchivItem_Bitmap*>(convertedBmp.get());
                            assert(bmpBase);
                            bmpBase->create(bmp->getWidth(), bmp->getHeight(), buffer); //-V522
                            break;
                        }
                        case BOBTYPE_BITMAP_PLAYER:
                        {
                            auto* bmpPl = dynamic_cast<ArchivItem_Bitmap_Player*>(convertedBmp.get());
                            assert(bmpPl);
                            bmpPl->create(bmp->getWidth(), bmp->getHeight(), buffer, curPal); //-V522
                        }
                        break;
                        default: return ErrorCode::UNSUPPORTED_FORMAT;
                    }
                    newItem = std::move(convertedBmp);
                }
                auto* bmp = static_cast<ArchivItem_BitmapBase*>(newItem.get());
                bmp->setName(entry.name);
                bmp->setNx(entry.nx);
                bmp->setNy(entry.ny);
                bmp->setPaletteCopy(*curPal);
            } else if(entry.bobtype == BOBTYPE_PALETTE_ANIM)
            {
                for(unsigned i = 0; i < tmpItems.size(); i++)
                {
                    if(!tmpItems[i])
                        continue;
                    if(items[i])
                        return ErrorCode::UNSUPPORTED_FORMAT;
                    if(i >= items.size())
                        items.alloc_inc(i - items.size() + 1);
                    items.set(i, tmpItems.release(i));
                }
                continue;
            } else
            {
                // todo: andere typen als pal und bmp haben evtl mehr items!
                if(tmpItems.size() != 1)
                    return ErrorCode::UNSUPPORTED_FORMAT;

                newItem = tmpItems.release(0);
            }
        }
        // had the filename a number? then set it to the corresponding item.
        if(entry.nr >= 0)
        {
            if(static_cast<unsigned>(entry.nr) >= items.size())
                items.alloc_inc(entry.nr - items.size() + 1);
            items.set(entry.nr, std::move(newItem));
        } else
            items.push(std::move(newItem));
    }
    return ErrorCode::NONE;
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
    boost::algorithm::to_lower(extension);

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
        else if(extension == "txt")
            ret = loader::WritePaletteAnim(file, items);
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

namespace {
    unsigned hexToInt(const std::string& hexStr)
    {
        s25util::ClassicImbuedStream<std::istringstream> sIn(hexStr);
        unsigned tmp;
        sIn >> std::hex >> tmp;
        if(!sIn || !sIn.eof())
            throw std::runtime_error("Invalid hex number 0x" + hexStr);
        return tmp;
    }
} // namespace

std::vector<FileEntry> ReadFolderInfo(const std::string& folderPath)
{
    std::vector<FileEntry> entries;
    for(const auto& it : bfs::directory_iterator(folderPath))
    {
        if(!bfs::is_regular_file(it.status()) && !bfs::is_directory(it.status()))
            continue;

        bfs::path curPath = it.path();
        curPath.make_preferred();

        FileEntry file(curPath.string());

        std::string fileName = curPath.filename().string();
        boost::algorithm::to_lower(fileName);
        std::vector<std::string> wf = Tokenizer(fileName, ".").explode();

        if(wf.back() == "fon" || wf.back() == "fonx")
            file.bobtype = BOBTYPE_FONT;
        else if(wf.back() == "bmp")
            file.bobtype = BOBTYPE_BITMAP_RAW;
        else if(wf.back() == "bbm" || wf.back() == "act")
            file.bobtype = BOBTYPE_PALETTE;
        else if(wf.back() == "txt" || wf.back() == "ger" || wf.back() == "eng")
            file.bobtype = BOBTYPE_TEXT;
        else if(wf.back() == "empty")
            file.bobtype = BOBTYPE_NONE;
        if(file.bobtype != BOBTYPE_UNSET)
            wf.pop_back();

        std::string sNr = wf.empty() ? "" : wf.front();
        if(sNr.substr(0, 2) == "u+" || sNr.substr(0, 2) == "0x") // Allow unicode file names (e.g. U+1234)
        {
            file.nr = hexToInt(sNr.substr(2));
            wf.erase(wf.begin());
        } else if(s25util::tryFromStringClassic(sNr, file.nr))
            wf.erase(wf.begin());

        for(const std::string& part : wf)
        {
            if(part == "rle")
                file.bobtype = BOBTYPE_BITMAP_RLE;
            else if(part == "player")
                file.bobtype = BOBTYPE_BITMAP_PLAYER;
            else if(part == "shadow")
                file.bobtype = BOBTYPE_BITMAP_SHADOW;
            else if(part == "paletteanims")
                file.bobtype = BOBTYPE_PALETTE_ANIM;
            else if(part == "palette")
                file.bobtype = BOBTYPE_PALETTE;

            else if(part.substr(0, 2) == "nx" || part.substr(0, 2) == "dx")
                file.nx = s25util::fromStringClassic<unsigned>(part.substr(2));
            else if(part.substr(0, 2) == "ny" || part.substr(0, 2) == "dy")
                file.ny = s25util::fromStringClassic<unsigned>(part.substr(2));
            else
                file.name += (file.name.empty() ? "" : ".") + part;
        }

        entries.push_back(file);
    }
    return entries;
}

} // namespace libsiedler2
