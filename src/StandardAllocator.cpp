// Copyright (c) 2005 - 2020 Settlers Freaks (sf-team at siedler25.org)
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
std::unique_ptr<ArchivItem> StandardAllocator::create(BobType type, SoundType subtype) const
{
    switch(type)
    {
        case BobType::Sound:
        {
            switch(subtype)
            {
                case SoundType::None: return nullptr;
                case SoundType::Midi: return std::make_unique<ArchivItem_Sound_Midi>();
                case SoundType::Wave: return std::make_unique<ArchivItem_Sound_Wave>();
                case SoundType::XMidi: return std::make_unique<ArchivItem_Sound_XMidi>();
                default: return std::make_unique<ArchivItem_Sound_Other>(subtype);
            }
            break;
        }
        case BobType::BitmapRLE: return std::make_unique<ArchivItem_Bitmap_RLE>();
        case BobType::Font: return std::make_unique<ArchivItem_Font>();
        case BobType::BitmapPlayer: return std::make_unique<ArchivItem_Bitmap_Player>();
        case BobType::Palette: return std::make_unique<ArchivItem_Palette>();
        case BobType::Bob: return std::make_unique<ArchivItem_Bob>();
        case BobType::BitmapShadow: return std::make_unique<ArchivItem_Bitmap_Shadow>();
        case BobType::Map: return std::make_unique<ArchivItem_Map>();
        case BobType::Text: return std::make_unique<ArchivItem_Text>();
        case BobType::Raw: return std::make_unique<ArchivItem_Raw>();
        case BobType::MapHeader: return std::make_unique<ArchivItem_Map_Header>();
        case BobType::Ini: return std::make_unique<ArchivItem_Ini>();
        case BobType::Bitmap: return std::make_unique<ArchivItem_Bitmap_Raw>();
        case BobType::PaletteAnim: return std::make_unique<ArchivItem_PaletteAnimation>();
        default: return nullptr;
    }
    return nullptr;
}

} // namespace libsiedler2
