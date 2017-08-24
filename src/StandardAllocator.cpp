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
        case BOBTYPE_SOUND: // WAVs, MIDIs
        {
            switch(subtype)
            {
                case SOUNDTYPE_NONE: return NULL;
                case SOUNDTYPE_MIDI: // MIDI
                    return new ArchivItem_Sound_Midi();
                case SOUNDTYPE_WAVE: // WAV
                    return new ArchivItem_Sound_Wave();
                case SOUNDTYPE_XMIDI: // XMIDI
                    return new ArchivItem_Sound_XMidi();
                default: // Andere
                    return new ArchivItem_Sound_Other(subtype);
            }
            break;
        }
        case BOBTYPE_BITMAP_RLE: // RLE komprimiertes Bitmap
            return new ArchivItem_Bitmap_RLE();
        case BOBTYPE_FONT: // Font
            return new ArchivItem_Font();
        case BOBTYPE_BITMAP_PLAYER: // Bitmap mit spezifischer Spielerfarbe
            return new ArchivItem_Bitmap_Player();
        case BOBTYPE_PALETTE: // Palette
            return new ArchivItem_Palette();
        case BOBTYPE_BOB: // Bobfiles
            return new ArchivItem_Bob();
        case BOBTYPE_BITMAP_SHADOW: // Schatten
            return new ArchivItem_Bitmap_Shadow();
        case BOBTYPE_MAP: // Mapfiles
            return new ArchivItem_Map();
        case BOBTYPE_TEXT: // Text
            return new ArchivItem_Text();
        case BOBTYPE_RAW: // Raw-Item
            return new ArchivItem_Raw();
        case BOBTYPE_MAP_HEADER: // Mapheader-Item
            return new ArchivItem_Map_Header();
        case BOBTYPE_INI: // INI-Datei-Item
            return new ArchivItem_Ini();
        case BOBTYPE_BITMAP_RAW: // unkomprimiertes Bitmap
            return new ArchivItem_Bitmap_Raw();
        default: return NULL;
    }
    return NULL;
}

} // namespace libsiedler2
