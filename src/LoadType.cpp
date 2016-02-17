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
#include "prototypen.h"
#include "archives.h"
#include "types.h"
#include <fstream>
#include <EndianStream.h>
#include <stdexcept>

// Include last!
#include "DebugNew.h"

///////////////////////////////////////////////////////////////////////////////
/**
 *  lädt eine spezifizierten Bobtype aus einer Datei in ein ArchivItem.
 *
 *  @param[in]  bobtype Typ des Items
 *  @param[in]  file    Filehandle auf die auszulesende Datei
 *  @param[in]  palette Grundpalette
 *  @param[out] item    ArchivItem-Struktur, welche gefüllt wird
 *
 *  @return Null bei Erfolg, ein Wert ungleich Null bei Fehler
 *
 *  @author FloSoft
 *  @author OLiver
 */
int libsiedler2::loader::LoadType(BOBTYPES bobtype, std::istream& file, const ArchivItem_Palette* palette, ArchivItem*& item)
{
    if(!file)
        return 1;

    try{
        switch(bobtype)
        {
            case BOBTYPE_SOUND: // WAVs, MIDIs
            {
                libendian::LittleEndianIStreamRef fs(file);
                unsigned int length;
                fs >> length;

                baseArchivItem_Sound* nitem = baseArchivItem_Sound::findSubType(file);
                if(!nitem || nitem->load(file, length) != 0){
                    delete nitem;
                    return 5;
                }
                item = nitem;
            } break;
            case BOBTYPE_BITMAP_RLE: // RLE komprimiertes Bitmap
            {
                baseArchivItem_Bitmap_RLE* nitem = dynamic_cast<baseArchivItem_Bitmap_RLE*>(getAllocator().create(BOBTYPE_BITMAP_RLE));
                if(nitem->load(file, palette) != 0){
                    delete nitem;
                    return 6;
                }
                item = nitem;
            } break;
            case BOBTYPE_FONT: // Font
            {
                ArchivItem_Font* nitem = dynamic_cast<ArchivItem_Font*>(getAllocator().create(BOBTYPE_FONT));
                if(nitem->load(file, palette) != 0){
                    delete nitem;
                    return 7;
                }
                item = nitem;
            } break;
            case BOBTYPE_BITMAP_PLAYER: // Bitmap mit spezifischer Spielerfarbe
            {
                ArchivItem_Bitmap_Player* nitem = dynamic_cast<ArchivItem_Bitmap_Player*>(getAllocator().create(BOBTYPE_BITMAP_PLAYER));
                if(nitem->load(file, palette) != 0){
                    delete nitem;
                    return 8;
                }
                item = nitem;
            } break;
            case BOBTYPE_PALETTE: // Palette
            {
                ArchivItem_Palette* nitem =  dynamic_cast<ArchivItem_Palette*>(getAllocator().create(BOBTYPE_PALETTE));
                if(nitem->load(file) != 0){
                    delete nitem;
                    return 9;
                }
                item = nitem;
            } break;
            case BOBTYPE_BOB: // Bobfile
            {
                ArchivItem_Bob* nitem = dynamic_cast<ArchivItem_Bob*>(getAllocator().create(BOBTYPE_BOB));
                if(nitem->load(file, palette) != 0){
                    delete nitem;
                    return 10;
                }
                item = nitem;
            } break;
            case BOBTYPE_BITMAP_SHADOW: // Schatten
            {
                baseArchivItem_Bitmap_Shadow* nitem = dynamic_cast<baseArchivItem_Bitmap_Shadow*>(getAllocator().create(BOBTYPE_BITMAP_SHADOW));
                if(nitem->load(file, palette) != 0){
                    delete nitem;
                    return 11;
                }
                item = nitem;
            } break;
            case BOBTYPE_MAP: // Mapfile
            {
                ArchivItem_Map* nitem = dynamic_cast<ArchivItem_Map*>(getAllocator().create(BOBTYPE_MAP));
                if(nitem->load(file, false) != 0){
                    delete nitem;
                    return 12;
                }
                item = nitem;
            } break;
            case BOBTYPE_TEXT: // Textfile
            {
                ArchivItem_Text* nitem =  dynamic_cast<ArchivItem_Text*>(getAllocator().create(BOBTYPE_TEXT));
                if(nitem->load(file) != 0){
                    delete nitem;
                    return 13;
                }
                item = nitem;
            } break;
            case BOBTYPE_BITMAP_RAW: // unkomprimiertes Bitmap
            {
                baseArchivItem_Bitmap_Raw* nitem = dynamic_cast<baseArchivItem_Bitmap_Raw*>(getAllocator().create(BOBTYPE_BITMAP_RAW));
                if(nitem->load(file, palette) != 0){
                    delete nitem;
                    return 14;
                }
                item = nitem;
            } break;
            default:
                item = NULL;
                break;
        }

        if(item != NULL)
            item->setBobType(bobtype);
    }catch(std::runtime_error&){
        // Mostly error on reading (e.g. unexpected end of file)
        return 999;
    }

    return 0;
}
