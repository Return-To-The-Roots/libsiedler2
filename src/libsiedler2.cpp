// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
// Copyright (C) 2005 - 2021 Settlers Freaks <sf-team at siedler25.org>
//
// SPDX-License-Identifier: GPL-2.0-or-later

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
#include "s25util/StringConversion.h"
#include "s25util/Tokenizer.h"
#include "s25util/strAlgos.h"
#include <boost/filesystem.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <algorithm>
#include <iostream>
#include <memory>
#include <stdexcept>

namespace bfs = boost::filesystem;
using boost::bad_numeric_cast;
using boost::numeric_cast;

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
        libsiedler2::setGlobalTextureFormat(libsiedler2::TextureFormat::Original);
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
 *  @param[in]  filepath    Dateiname der Datei
 *  @param[out] items   Archiv-Struktur, welche gefüllt wird
 *  @param[in]  palette Palette, welche benutzt werden soll
 *
 *  @return Null bei Erfolg, ein Wert ungleich Null bei Fehler
 */
int Load(const boost::filesystem::path& filepath, Archiv& items, const ArchivItem_Palette* palette)
{
    if(filepath.empty())
        return ErrorCode::INVALID_BUFFER;

    if(!filepath.has_extension())
        return ErrorCode::UNSUPPORTED_FORMAT;
    const std::string extension = s25util::toLower(filepath.extension().string().substr(1));

    int ret = ErrorCode::UNSUPPORTED_FORMAT;

    try
    {
        // Datei laden
        if(extension == "act")
            ret = loader::LoadACT(filepath, items);
        else if(extension == "bbm")
            ret = loader::LoadBBM(filepath, items);
        else if(extension == "bmp")
            ret = loader::LoadBMP(filepath, items, palette);
        else if(extension == "bob")
            ret = loader::LoadBOB(filepath, items, palette);
        else if(extension == "dat" || extension == "idx")
        {
            ret = loader::LoadDATIDX(filepath, items, palette);
            if(ret == ErrorCode::WRONG_HEADER && extension == "dat")
                ret = loader::LoadSND(filepath, items);
        } else if(extension == "lbm")
            ret = loader::LoadLBM(filepath, items);
        else if(extension == "lst")
            ret = loader::LoadLST(filepath, items, palette);
        else if(extension == "swd" || extension == "wld")
            ret = loader::LoadMAP(filepath, items);
        else if(extension == "ger" || extension == "eng")
            ret = loader::LoadTXT(filepath, items, true);
        else if(extension == "ini")
            ret = loader::LoadINI(filepath, items);
        else if(extension == "ogg" || extension == "wav" || extension == "mid" || extension == "midi"
                || extension == "xmi")
            ret = loader::LoadSND(filepath, items);
        else if(extension == "links")
            ret = loader::LoadTXT(filepath, items, false);
        else if(extension == "txt")
        {
            const bfs::path filename = filepath.stem();
            const std::string ext2 =
              s25util::toLower(filename.has_extension() ? filename.extension().string().substr(1) : filename.string());
            if(ext2 == "paletteanims")
                ret = loader::LoadPaletteAnim(filepath, items);
            else if(ext2 == "palette")
                ret = loader::LoadTxtPalette(filepath, items);
            else
                ret = loader::LoadTXT(filepath, items, false);
        } else
            std::cerr << "Unsupported extension: " << extension << std::endl;
    } catch(std::exception& error)
    {
        std::cerr << "Error while reading: " << error.what() << std::endl;
        // Mostly error on reading (e.g. unexpected end of filepath)
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
        if(entry.bobtype == BobType::Unset)
            continue;
        std::unique_ptr<ArchivItem> newItem;
        if(entry.bobtype == BobType::Font)
        {
            auto font = getAllocator().create<ArchivItem_Font>(BobType::Font);
            font->isUnicode = s25util::toLower(entry.filePath.extension().string()) == ".fonx";
            try
            {
                font->setDx(numeric_cast<uint8_t>(entry.nx));
                font->setDy(numeric_cast<uint8_t>(entry.ny));
            } catch(const bad_numeric_cast&)
            {
                return ErrorCode::CUSTOM + 1;
            }
            int ec;
            if(bfs::is_directory(entry.filePath))
                ec = LoadFolder(ReadFolderInfo(entry.filePath), *font, palette);
            else
                ec = Load(entry.filePath, *font, palette);
            if(ec)
                return ec;

            newItem = std::move(font);
        } else if(entry.bobtype != BobType::None)
        {
            const ArchivItem_Palette* curPal = palette;
            if(entry.nr >= 0)
            {
                if(static_cast<unsigned>(entry.nr) < items.size() && items[entry.nr]->getBobType() == BobType::Palette)
                    curPal = static_cast<const ArchivItem_Palette*>(items[entry.nr]);
            } else if(!items.empty() && items[items.size() - 1u]
                      && items[items.size() - 1u]->getBobType() == BobType::Palette)
                curPal = static_cast<const ArchivItem_Palette*>(items[items.size() - 1u]);
            Archiv tmpItems;
            if(int ec = Load(entry.filePath, tmpItems, curPal))
                return ec;
            if(entry.bobtype == BobType::BitmapPlayer || entry.bobtype == BobType::Bitmap
               || entry.bobtype == BobType::BitmapRLE || entry.bobtype == BobType::BitmapShadow)
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
                    if(bmp->getBobType() == BobType::BitmapPlayer)
                    {
                        auto* bmpPlayer = dynamic_cast<ArchivItem_Bitmap_Player*>(bmp);
                        assert(bmpPlayer);
                        if(int ec = bmpPlayer->print(buffer, curPal))
                            return ec;
                    } else
                    {
                        auto* bmpBase = dynamic_cast<baseArchivItem_Bitmap*>(bmp);
                        assert(bmpBase);
                        if(int ec = bmpBase->print(buffer))
                            return ec;
                    }

                    switch(entry.bobtype)
                    {
                        case BobType::BitmapRLE:
                        case BobType::BitmapShadow:
                        case BobType::Bitmap:
                        {
                            auto* bmpBase = dynamic_cast<baseArchivItem_Bitmap*>(convertedBmp.get());
                            assert(bmpBase);
                            if(int ec = bmpBase->create(bmp->getWidth(), bmp->getHeight(), buffer)) //-V522
                                return ec;
                            break;
                        }
                        case BobType::BitmapPlayer:
                        {
                            auto* bmpPl = dynamic_cast<ArchivItem_Bitmap_Player*>(convertedBmp.get());
                            assert(bmpPl);
                            if(int ec = bmpPl->create(bmp->getWidth(), bmp->getHeight(), buffer, curPal)) //-V522
                                return ec;
                        }
                        break;
                        default: return ErrorCode::UNSUPPORTED_FORMAT;
                    }
                    newItem = std::move(convertedBmp);
                }
                auto* bmp = static_cast<ArchivItem_BitmapBase*>(newItem.get());
                try
                {
                    bmp->setNx(numeric_cast<int16_t>(entry.nx));
                    bmp->setNy(numeric_cast<int16_t>(entry.ny));
                } catch(const bad_numeric_cast&)
                {
                    return ErrorCode::CUSTOM + 1;
                }
                if(curPal && !bmp->getPalette())
                    bmp->setPaletteCopy(*curPal);
            } else if(entry.bobtype == BobType::PaletteAnim)
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
        if(newItem)
            newItem->setName(entry.name);
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
 *  @param[in] filepath    Dateiname der Datei
 *  @param[in] items   Archiv-Struktur, von welcher gelesen wird
 *  @param[in] palette Palette, welche benutzt werden soll
 *
 *  @return Null bei Erfolg, ein Wert ungleich Null bei Fehler
 */
int Write(const boost::filesystem::path& filepath, const Archiv& items, const ArchivItem_Palette* palette)
{
    if(filepath.empty())
        return ErrorCode::INVALID_BUFFER;

    if(!filepath.has_extension())
        return ErrorCode::UNSUPPORTED_FORMAT;
    const std::string extension = s25util::toLower(filepath.extension().string().substr(1));

    int ret = ErrorCode::UNSUPPORTED_FORMAT;

    try
    {
        // Datei schreiben
        if(extension == "act")
            ret = loader::WriteACT(filepath, items);
        else if(extension == "bbm")
            ret = loader::WriteBBM(filepath, items);
        else if(extension == "bmp")
            ret = loader::WriteBMP(filepath, items, palette);
        else if(extension == "lst")
            ret = loader::WriteLST(filepath, items, palette);
        else if(extension == "swd" || extension == "wld")
            ret = loader::WriteMAP(filepath, items);
        else if(extension == "ger" || extension == "eng")
            ret = loader::WriteTXT(filepath, items, true);
        else if(extension == "ini")
            ret = loader::WriteINI(filepath, items);
        else if(extension == "ogg" || extension == "wav" || extension == "mid" || extension == "midi"
                || extension == "xmi")
            ret = loader::WriteSND(filepath, items);
        else if(extension == "lbm")
            ret = loader::WriteLBM(filepath, items, palette);
        else if(extension == "txt")
            ret = loader::WritePaletteAnim(filepath, items);
        else
            std::cerr << "Unsupported extension: " << extension << std::endl;
    } catch(std::exception& error)
    {
        std::cerr << "Error while writing: " << error.what() << std::endl;
        // Mostly error on write to filepath
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

std::vector<FileEntry> ReadFolderInfo(const boost::filesystem::path& folderPath)
{
    std::vector<FileEntry> entries;
    for(const auto& it : bfs::directory_iterator(folderPath))
    {
        if(!bfs::is_regular_file(it.status()) && !bfs::is_directory(it.status()))
            continue;

        bfs::path curPath = it.path();
        curPath.make_preferred();

        FileEntry filepath(curPath);

        const std::string fileName = s25util::toLower(curPath.filename().string());
        std::vector<std::string> wf = Tokenizer(fileName, ".").explode();

        if(wf.back() == "fon" || wf.back() == "fonx")
            filepath.bobtype = BobType::Font;
        else if(wf.back() == "bmp")
            filepath.bobtype = BobType::Bitmap;
        else if(wf.back() == "bbm" || wf.back() == "act")
            filepath.bobtype = BobType::Palette;
        else if(wf.back() == "txt" || wf.back() == "ger" || wf.back() == "eng" || wf.back() == "links")
            filepath.bobtype = BobType::Text;
        else if(wf.back() == "empty")
            filepath.bobtype = BobType::None;
        else if(wf.back() == "midi" || wf.back() == "xmi" || wf.back() == "wav")
            filepath.bobtype = BobType::Sound;
        if(filepath.bobtype != BobType::Unset)
            wf.pop_back();

        std::string sNr = wf.empty() ? "" : wf.front();
        if(sNr.substr(0, 2) == "u+" || sNr.substr(0, 2) == "0x") // Allow Unicode filepath names (e.g. U+1234)
        {
            filepath.nr = hexToInt(sNr.substr(2));
            wf.erase(wf.begin());
        } else if(s25util::tryFromStringClassic(sNr, filepath.nr))
            wf.erase(wf.begin());
        else
            filepath.nr = -1;

        for(const std::string& part : wf)
        {
            if(part == "rle")
                filepath.bobtype = BobType::BitmapRLE;
            else if(part == "player")
                filepath.bobtype = BobType::BitmapPlayer;
            else if(part == "shadow")
                filepath.bobtype = BobType::BitmapShadow;
            else if(part == "paletteanims")
                filepath.bobtype = BobType::PaletteAnim;
            else if(part == "palette")
                filepath.bobtype = BobType::Palette;

            else if(part.substr(0, 2) == "nx" || part.substr(0, 2) == "dx")
                filepath.nx = s25util::fromStringClassic<int>(part.substr(2));
            else if(part.substr(0, 2) == "ny" || part.substr(0, 2) == "dy")
                filepath.ny = s25util::fromStringClassic<int>(part.substr(2));
            else
                filepath.name += (filepath.name.empty() ? "" : ".") + part;
        }

        entries.push_back(filepath);
    }
    return entries;
}

} // namespace libsiedler2
