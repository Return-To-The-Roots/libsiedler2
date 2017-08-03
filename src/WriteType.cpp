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
#include "prototypen.h"
#include "ArchivItem_Bitmap_Player.h"
#include "ArchivItem_Bitmap_RLE.h"
#include "ArchivItem_Bitmap_Raw.h"
#include "ArchivItem_Bitmap_Shadow.h"
#include "ArchivItem_Bob.h"
#include "ArchivItem_Font.h"
#include "ArchivItem_Map.h"
#include "ArchivItem_Palette.h"
#include "ArchivItem_Sound.h"
#include "ErrorCodes.h"
#include "libendian/src/EndianOStreamAdapter.h"
#include <stdexcept>
#include <iostream>

/**
 *  schreibt eine spezifizierten Bobtype aus einem ArchivItem in eine Datei.
 *
 *  @param[in]  bobtype Typ des Items
 *  @param[in]  file    Filehandle auf die auszulesende Datei
 *  @param[in]  palette Grundpalette
 *  @param[out] item    ArchivItem-Struktur, welche gefüllt wird
 *
 *  @return Null bei Erfolg, ein Wert ungleich Null bei Fehler
 */
int libsiedler2::loader::WriteType(BobType bobtype, std::ostream&lst, const ArchivItem& item, const ArchivItem_Palette* palette)
{
    if(!lst)
        return ErrorCode::FILE_NOT_ACCESSIBLE;

    try{
        switch(bobtype)
        {
            case BOBTYPE_SOUND: // WAVs, MIDIs
            {
                const baseArchivItem_Sound& i = dynamic_cast<const baseArchivItem_Sound&>(item);
                libendian::EndianOStreamAdapter<false, std::ostream&> fs(lst);
                const long sizePos = fs.getPosition();
                fs << uint32_t(0);
                if(int ec = i.write(lst))
                    return ec;
                const long curPos = fs.getPosition();
                fs.setPosition(sizePos);
                fs << uint32_t(curPos - sizePos);
                fs.setPosition(curPos);
            } break;
            case BOBTYPE_BITMAP_RLE: // RLE komprimiertes Bitmap
            {
                const baseArchivItem_Bitmap_RLE& i = dynamic_cast<const baseArchivItem_Bitmap_RLE&>(item);

                if(int ec = i.write(lst, palette))
                    return ec;
            } break;
            case BOBTYPE_FONT: // Font
            {
                const ArchivItem_Font& i = dynamic_cast<const ArchivItem_Font&>(item);

                if(int ec = i.write(lst, palette))
                    return ec;
            } break;
            case BOBTYPE_BITMAP_PLAYER: // Bitmap mit spezifischer Spielerfarbe
            {
                const ArchivItem_Bitmap_Player& i = dynamic_cast<const ArchivItem_Bitmap_Player&>(item);

                if(int ec = i.write(lst, palette))
                    return ec;
            } break;
            case BOBTYPE_PALETTE: // Palette
            {
                const ArchivItem_Palette& i = dynamic_cast<const ArchivItem_Palette&>(item);

                if(int ec = i.write(lst))
                    return ec;
            } break;
            case BOBTYPE_BITMAP_SHADOW: // Schatten
            {
                const baseArchivItem_Bitmap_Shadow& i = dynamic_cast<const baseArchivItem_Bitmap_Shadow&>(item);

                if(int ec = i.write(lst, palette))
                    return ec;
            } break;
            case BOBTYPE_BOB: // Bobfile
            {
                const ArchivItem_Bob& i = dynamic_cast<const ArchivItem_Bob&>(item);

                if(int ec = i.write(lst, palette))
                    return ec;
            } break;
            case BOBTYPE_MAP: // Mapfile
            {
                const ArchivItem_Map& i = dynamic_cast<const ArchivItem_Map&>(item);

                if(int ec = i.write(lst))
                    return ec;
            } break;
            case BOBTYPE_BITMAP_RAW: // unkomprimiertes Bitmap
            {
                const baseArchivItem_Bitmap_Raw& i = dynamic_cast<const baseArchivItem_Bitmap_Raw&>(item);

                if(int ec = i.write(lst, palette))
                    return ec;
            } break;
            default:
                return ErrorCode::NONE;
        }
    } catch(std::exception& e){
        // Mostly error on reading (e.g. unexpected end of file)
        std::cerr << "Error while reading: " << e.what() << std::endl;
        return ErrorCode::CUSTOM;
    }
    return ErrorCode::NONE;
}
