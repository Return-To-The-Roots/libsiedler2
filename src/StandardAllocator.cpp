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
// along with Return To The Roots. If not, see <http://www.gnu.org/licenses/>

#include "libSiedler2Defines.h" // IWYU pragma: keep
#include "StandardAllocator.h"
#include "ArchivItem_PaletteAnimation.h"
#include "archives.h"

namespace libsiedler2 {

/**
 *  Der Standard-Item-Allocator.
 *
 *  @param[in] type    Der Typ des Items
 *  @param[in] subtype Der Subtyp des Items
 *
 */
ArchivItem* StandardAllocator::create(BobType type, SoundType subtype) const
{
    switch(type)
    {
        case BOBTYPE_SOUND:
        {
            switch(subtype)
            {
                case SOUNDTYPE_NONE: return nullptr;
                case SOUNDTYPE_MIDI: return new ArchivItem_Sound_Midi();
                case SOUNDTYPE_WAVE: return new ArchivItem_Sound_Wave();
                case SOUNDTYPE_XMIDI: return new ArchivItem_Sound_XMidi();
                default: return new ArchivItem_Sound_Other(subtype);
            }
            break;
        }
        case BOBTYPE_BITMAP_RLE: return new ArchivItem_Bitmap_RLE();
        case BOBTYPE_FONT: return new ArchivItem_Font();
        case BOBTYPE_BITMAP_PLAYER: return new ArchivItem_Bitmap_Player();
        case BOBTYPE_PALETTE: return new ArchivItem_Palette();
        case BOBTYPE_BOB: return new ArchivItem_Bob();
        case BOBTYPE_BITMAP_SHADOW: return new ArchivItem_Bitmap_Shadow();
        case BOBTYPE_MAP: return new ArchivItem_Map();
        case BOBTYPE_TEXT: return new ArchivItem_Text();
        case BOBTYPE_RAW: return new ArchivItem_Raw();
        case BOBTYPE_MAP_HEADER: return new ArchivItem_Map_Header();
        case BOBTYPE_INI: return new ArchivItem_Ini();
        case BOBTYPE_BITMAP_RAW: return new ArchivItem_Bitmap_Raw();
        case BOBTYPE_PALETTE_ANIM: return new ArchivItem_PaletteAnimation();
        default: return nullptr;
    }
    return nullptr;
}

} // namespace libsiedler2
