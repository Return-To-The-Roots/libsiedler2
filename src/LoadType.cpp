// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "ArchivItem_Bitmap_Player.h"
#include "ArchivItem_Bitmap_RLE.h"
#include "ArchivItem_Bitmap_Raw.h"
#include "ArchivItem_Bitmap_Shadow.h"
#include "ArchivItem_Bob.h"
#include "ArchivItem_Font.h"
#include "ArchivItem_Map.h"
#include "ArchivItem_Palette.h"
#include "ArchivItem_PaletteAnimation.h"
#include "ArchivItem_Sound.h"
#include "ArchivItem_Text.h"
#include "ErrorCodes.h"
#include "IAllocator.h"
#include "libsiedler2.h"
#include "prototypen.h"
#include "libendian/EndianIStreamAdapter.h"
#include <iostream>

/**
 *  lädt eine spezifizierten Bobtype aus einer Datei in ein ArchivItem.
 *
 *  @param[in]  bobtype Typ des Items
 *  @param[in]  filepath    Filehandle auf die auszulesende Datei
 *  @param[in]  palette Grundpalette
 *  @param[out] item    ArchivItem-Struktur, welche gefüllt wird
 *
 *  @return Null bei Erfolg, ein Wert ungleich Null bei Fehler
 */
int libsiedler2::loader::LoadType(BobType bobtype, std::istream& lst, std::unique_ptr<ArchivItem>& item,
                                  const ArchivItem_Palette* palette)
{
    if(!lst)
        return ErrorCode::FILE_NOT_ACCESSIBLE;

    try
    {
        switch(bobtype)
        {
            case BobType::Sound: // WAVs, MIDIs
            {
                libendian::EndianIStreamAdapter<false, std::istream&> fs(lst);
                uint32_t length;
                fs >> length;

                auto nitem = ArchivItem_Sound::findSubType(lst);
                if(!nitem)
                    return ErrorCode::WRONG_HEADER;
                if(int ec = nitem->load(lst, length))
                    return ec;
                item = std::move(nitem);
            }
            break;
            case BobType::BitmapRLE: // RLE komprimiertes Bitmap
            {
                auto nitem = getAllocator().create<baseArchivItem_Bitmap_RLE>(BobType::BitmapRLE);
                if(int ec = nitem->load(lst, palette)) //-V522
                    return ec;
                item = std::move(nitem);
            }
            break;
            case BobType::Font: // Font
            {
                auto nitem = getAllocator().create<ArchivItem_Font>(BobType::Font);
                if(int ec = nitem->load(lst, palette)) //-V522
                    return ec;
                item = std::move(nitem);
            }
            break;
            case BobType::BitmapPlayer: // Bitmap mit spezifischer Spielerfarbe
            {
                auto nitem = getAllocator().create<ArchivItem_Bitmap_Player>(BobType::BitmapPlayer);
                if(int ec = nitem->load(lst, palette)) //-V522
                    return ec;
                item = std::move(nitem);
            }
            break;
            case BobType::Palette: // Palette
            {
                auto nitem = getAllocator().create<ArchivItem_Palette>(BobType::Palette);
                if(int ec = nitem->load(lst)) //-V522
                    return ec;
                item = std::move(nitem);
            }
            break;
            case BobType::Bob: // Bobfile
            {
                auto nitem = getAllocator().create<ArchivItem_Bob>(BobType::Bob);
                if(int ec = nitem->load(lst, palette)) //-V522
                    return ec;
                item = std::move(nitem);
            }
            break;
            case BobType::BitmapShadow: // Schatten
            {
                auto nitem = getAllocator().create<baseArchivItem_Bitmap_Shadow>(BobType::BitmapShadow);
                if(int ec = nitem->load(lst, palette)) //-V522
                    return ec;
                item = std::move(nitem);
            }
            break;
            case BobType::Map: // Mapfile
            {
                auto nitem = getAllocator().create<ArchivItem_Map>(BobType::Map);
                if(int ec = nitem->load(lst, false)) //-V522
                    return ec;
                item = std::move(nitem);
            }
            break;
            case BobType::Text: // Textfile
            {
                auto nitem = getAllocator().create<ArchivItem_Text>(BobType::Text);
                if(int ec = nitem->load(lst)) //-V522
                    return ec;
                item = std::move(nitem);
            }
            break;
            case BobType::Bitmap: // unkomprimiertes Bitmap
            {
                auto nitem = getAllocator().create<baseArchivItem_Bitmap_Raw>(BobType::Bitmap);
                if(int ec = nitem->load(lst, palette)) //-V522
                    return ec;
                item = std::move(nitem);
            }
            break;
            case BobType::PaletteAnim:
            {
                auto nitem = getAllocator().create<ArchivItem_PaletteAnimation>(BobType::PaletteAnim);
                if(int ec = nitem->load(lst)) //-V522
                    return ec;
                item = std::move(nitem);
            }
            break;
            case BobType::None: item = nullptr; break;
            default: item = nullptr; return ErrorCode::WRONG_FORMAT;
        }
    } catch(std::exception& e)
    {
        // Mostly error on reading (e.g. unexpected end of file)
        std::cerr << "Error while reading: " << e.what() << std::endl;
        return ErrorCode::CUSTOM;
    }

    return ErrorCode::NONE;
}
