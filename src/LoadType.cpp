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
#include "ArchivItem_Bitmap_Player.h"
#include "ArchivItem_Bitmap_RLE.h"
#include "ArchivItem_Bitmap_Raw.h"
#include "ArchivItem_Bitmap_Shadow.h"
#include "ArchivItem_Bob.h"
#include "ArchivItem_Font.h"
#include "ArchivItem_Map.h"
#include "ArchivItem_Palette.h"
#include "ArchivItem_Sound.h"
#include "ArchivItem_Text.h"
#include "ErrorCodes.h"
#include "IAllocator.h"
#include "libsiedler2.h"
#include "prototypen.h"
#include "libendian/EndianIStreamAdapter.h"
#include <iostream>
#include <stdexcept>

/**
 *  lädt eine spezifizierten Bobtype aus einer Datei in ein ArchivItem.
 *
 *  @param[in]  bobtype Typ des Items
 *  @param[in]  file    Filehandle auf die auszulesende Datei
 *  @param[in]  palette Grundpalette
 *  @param[out] item    ArchivItem-Struktur, welche gefüllt wird
 *
 *  @return Null bei Erfolg, ein Wert ungleich Null bei Fehler
 */
int libsiedler2::loader::LoadType(BobType bobtype, std::istream& lst, ArchivItem*& item, const ArchivItem_Palette* palette)
{
    if(!lst)
        return ErrorCode::FILE_NOT_ACCESSIBLE;

    try
    {
        switch(bobtype)
        {
            case BOBTYPE_SOUND: // WAVs, MIDIs
            {
                libendian::EndianIStreamAdapter<false, std::istream&> fs(lst);
                uint32_t length;
                fs >> length;

                ArchivItem_Sound* nitem = ArchivItem_Sound::findSubType(lst);
                if(!nitem)
                    return ErrorCode::WRONG_HEADER;
                if(int ec = nitem->load(lst, length))
                {
                    delete nitem;
                    return ec;
                }
                item = nitem;
            }
            break;
            case BOBTYPE_BITMAP_RLE: // RLE komprimiertes Bitmap
            {
                baseArchivItem_Bitmap_RLE* nitem = dynamic_cast<baseArchivItem_Bitmap_RLE*>(getAllocator().create(BOBTYPE_BITMAP_RLE));
                if(int ec = nitem->load(lst, palette))
                {
                    delete nitem;
                    return ec;
                }
                item = nitem;
            }
            break;
            case BOBTYPE_FONT: // Font
            {
                ArchivItem_Font* nitem = dynamic_cast<ArchivItem_Font*>(getAllocator().create(BOBTYPE_FONT));
                if(int ec = nitem->load(lst, palette))
                {
                    delete nitem;
                    return ec;
                }
                item = nitem;
            }
            break;
            case BOBTYPE_BITMAP_PLAYER: // Bitmap mit spezifischer Spielerfarbe
            {
                ArchivItem_Bitmap_Player* nitem = dynamic_cast<ArchivItem_Bitmap_Player*>(getAllocator().create(BOBTYPE_BITMAP_PLAYER));
                if(int ec = nitem->load(lst, palette))
                {
                    delete nitem;
                    return ec;
                }
                item = nitem;
            }
            break;
            case BOBTYPE_PALETTE: // Palette
            {
                ArchivItem_Palette* nitem = dynamic_cast<ArchivItem_Palette*>(getAllocator().create(BOBTYPE_PALETTE));
                if(int ec = nitem->load(lst))
                {
                    delete nitem;
                    return ec;
                }
                item = nitem;
            }
            break;
            case BOBTYPE_BOB: // Bobfile
            {
                ArchivItem_Bob* nitem = dynamic_cast<ArchivItem_Bob*>(getAllocator().create(BOBTYPE_BOB));
                if(int ec = nitem->load(lst, palette))
                {
                    delete nitem;
                    return ec;
                }
                item = nitem;
            }
            break;
            case BOBTYPE_BITMAP_SHADOW: // Schatten
            {
                baseArchivItem_Bitmap_Shadow* nitem =
                  dynamic_cast<baseArchivItem_Bitmap_Shadow*>(getAllocator().create(BOBTYPE_BITMAP_SHADOW));
                if(int ec = nitem->load(lst, palette))
                {
                    delete nitem;
                    return ec;
                }
                item = nitem;
            }
            break;
            case BOBTYPE_MAP: // Mapfile
            {
                ArchivItem_Map* nitem = dynamic_cast<ArchivItem_Map*>(getAllocator().create(BOBTYPE_MAP));
                if(int ec = nitem->load(lst, false))
                {
                    delete nitem;
                    return ec;
                }
                item = nitem;
            }
            break;
            case BOBTYPE_TEXT: // Textfile
            {
                ArchivItem_Text* nitem = dynamic_cast<ArchivItem_Text*>(getAllocator().create(BOBTYPE_TEXT));
                if(int ec = nitem->load(lst))
                {
                    delete nitem;
                    return ec;
                }
                item = nitem;
            }
            break;
            case BOBTYPE_BITMAP_RAW: // unkomprimiertes Bitmap
            {
                baseArchivItem_Bitmap_Raw* nitem = dynamic_cast<baseArchivItem_Bitmap_Raw*>(getAllocator().create(BOBTYPE_BITMAP_RAW));
                if(int ec = nitem->load(lst, palette))
                {
                    delete nitem;
                    return ec;
                }
                item = nitem;
            }
            break;
            case BOBTYPE_NONE: item = NULL; break;
            default: item = NULL; return ErrorCode::WRONG_FORMAT;
        }
    } catch(std::exception& e)
    {
        // Mostly error on reading (e.g. unexpected end of file)
        std::cerr << "Error while reading: " << e.what() << std::endl;
        return ErrorCode::CUSTOM;
    }

    return ErrorCode::NONE;
}
