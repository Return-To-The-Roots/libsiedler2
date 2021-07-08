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
#include "ErrorCodes.h"
#include "prototypen.h"
#include "libendian/EndianOStreamAdapter.h"
#include <iostream>

/**
 *  schreibt eine spezifizierten Bobtype aus einem ArchivItem in eine Datei.
 *
 *  @param[in]  bobtype Typ des Items
 *  @param[in]  filepath    Filehandle auf die auszulesende Datei
 *  @param[in]  palette Grundpalette
 *  @param[out] item    ArchivItem-Struktur, welche gefüllt wird
 *
 *  @return Null bei Erfolg, ein Wert ungleich Null bei Fehler
 */
int libsiedler2::loader::WriteType(BobType bobtype, std::ostream& lst, const ArchivItem& item,
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
                const auto& i = dynamic_cast<const ArchivItem_Sound&>(item);
                libendian::EndianOStreamAdapter<false, std::ostream&> fs(lst);
                const long sizePos = fs.getPosition();
                fs << uint32_t(0);
                if(int ec = i.write(lst))
                    return ec;
                const long curPos = fs.getPosition();
                fs.setPosition(sizePos);
                fs << uint32_t(curPos - sizePos - 4);
                fs.setPosition(curPos);
            }
            break;
            case BobType::BitmapRLE: // RLE komprimiertes Bitmap
            {
                const auto& i = dynamic_cast<const baseArchivItem_Bitmap_RLE&>(item);

                if(int ec = i.write(lst, palette))
                    return ec;
            }
            break;
            case BobType::Font: // Font
            {
                const auto& i = dynamic_cast<const ArchivItem_Font&>(item);

                if(int ec = i.write(lst, palette))
                    return ec;
            }
            break;
            case BobType::BitmapPlayer: // Bitmap mit spezifischer Spielerfarbe
            {
                const auto& i = dynamic_cast<const ArchivItem_Bitmap_Player&>(item);

                if(int ec = i.write(lst, palette))
                    return ec;
            }
            break;
            case BobType::Palette: // Palette
            {
                const auto& i = dynamic_cast<const ArchivItem_Palette&>(item);

                if(int ec = i.write(lst))
                    return ec;
            }
            break;
            case BobType::BitmapShadow: // Schatten
            {
                const auto& i = dynamic_cast<const baseArchivItem_Bitmap_Shadow&>(item);

                if(int ec = i.write(lst, palette))
                    return ec;
            }
            break;
            case BobType::Bob: // Bobfile
            {
                const auto& i = dynamic_cast<const ArchivItem_Bob&>(item);

                if(int ec = i.write(lst, palette))
                    return ec;
            }
            break;
            case BobType::Map: // Mapfile
            {
                const auto& i = dynamic_cast<const ArchivItem_Map&>(item);

                if(int ec = i.write(lst))
                    return ec;
            }
            break;
            case BobType::Bitmap: // unkomprimiertes Bitmap
            {
                const auto& i = dynamic_cast<const baseArchivItem_Bitmap_Raw&>(item);

                if(int ec = i.write(lst, palette))
                    return ec;
            }
            break;
            case BobType::PaletteAnim:
            {
                const auto& nitem = dynamic_cast<const ArchivItem_PaletteAnimation&>(item);
                if(int ec = nitem.write(lst)) //-V522
                    return ec;
            }
            break;
            default: return ErrorCode::NONE;
        }
    } catch(std::exception& e)
    {
        // Mostly error on writing (e.g. unexpected end of file)
        std::cerr << "Error while writing: " << e.what() << std::endl;
        return ErrorCode::CUSTOM;
    }
    return ErrorCode::NONE;
}
