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
// along with Return To The Roots. If not, see <http://www.gnu.org/licenses/>

#include "main.h" // IWYU pragma: keep
#include "StandardAllocator.h"
#include "ArchivItem_Bitmap_Player.h"
#include "ArchivItem_Bitmap_RLE.h"
#include "ArchivItem_Bitmap_Raw.h"
#include "ArchivItem_Bitmap_Shadow.h"
#include "ArchivItem_Bob.h"
#include "ArchivItem_Font.h"
#include "ArchivItem_Ini.h"
#include "ArchivItem_Map.h"
#include "ArchivItem_Map_Header.h"
#include "ArchivItem_Palette.h"
#include "ArchivItem_Raw.h"
#include "ArchivItem_Sound.h"
#include "ArchivItem_Sound_Midi.h"
#include "ArchivItem_Sound_Other.h"
#include "ArchivItem_Sound_Wave.h"
#include "ArchivItem_Sound_XMidi.h"
#include "ArchivItem_Text.h"

// Include last!
#include "DebugNew.h" // IWYU pragma: keep

namespace libsiedler2
{

    /**
     *  Der Standard-Item-Allocator.
     *
     *  @param[in] type    Der Typ des Items
     *  @param[in] subtype Der Subtyp des Items
     *
     */
    ArchivItem* StandardAllocator::create(BOBTYPES type, SOUNDTYPES subtype) const
    {
        switch(type)
        {
            case BOBTYPE_SOUND: // WAVs, MIDIs
            {
                switch(subtype)
                {
                    case SOUNDTYPE_NONE:
                        return NULL;
                    case SOUNDTYPE_MIDI: // MIDI
                        return new ArchivItem_Sound_Midi();
                    case SOUNDTYPE_WAVE: // WAV
                        return new ArchivItem_Sound_Wave();
                    case SOUNDTYPE_XMIDI: // XMIDI
                        return new ArchivItem_Sound_XMidi();
                    case SOUNDTYPE_OTHER: // Andere
                        return new ArchivItem_Sound_Other();
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
            default:
                return NULL;
        }
        return NULL;
    }

    /**
     *  Der Standard-Item-Allocator.
     *
     *  @param[in] item    Das zu kopierende Item
     *
     */
    ArchivItem* StandardAllocator::clone(const ArchivItem& item) const
    {
        BOBTYPES type = static_cast<BOBTYPES>(item.getBobType());

        switch(type)
        {
            case BOBTYPE_SOUND: // WAVs, MIDIs
            {
                SOUNDTYPES subtype = static_cast<SOUNDTYPES>(dynamic_cast<const ArchivItem_Sound&>(item).getType());

                switch(subtype)
                {
                    case SOUNDTYPE_NONE:
                        return NULL;
                    case SOUNDTYPE_MIDI: // MIDI
                        return new ArchivItem_Sound_Midi( dynamic_cast<const ArchivItem_Sound_Midi&>(item) );
                    case SOUNDTYPE_WAVE: // WAV
                        return new ArchivItem_Sound_Wave( dynamic_cast<const ArchivItem_Sound_Wave&>(item) );
                    case SOUNDTYPE_XMIDI: // XMIDI
                        return new ArchivItem_Sound_XMidi( dynamic_cast<const ArchivItem_Sound_XMidi&>(item) );
                    case SOUNDTYPE_OTHER: // Andere
                        return new ArchivItem_Sound_Other( dynamic_cast<const ArchivItem_Sound_Other&>(item) );
                }
                break;
            }
            case BOBTYPE_BITMAP_RLE: // RLE komprimiertes Bitmap
                return new ArchivItem_Bitmap_RLE( dynamic_cast<const ArchivItem_Bitmap_RLE&>(item) );
            case BOBTYPE_FONT: // Font
                return new ArchivItem_Font( dynamic_cast<const ArchivItem_Font&>(item) );
            case BOBTYPE_BITMAP_PLAYER: // Bitmap mit spezifischer Spielerfarbe
                return new ArchivItem_Bitmap_Player( dynamic_cast<const ArchivItem_Bitmap_Player&>(item) );
            case BOBTYPE_PALETTE: // Palette
                return new ArchivItem_Palette( dynamic_cast<const ArchivItem_Palette&>(item) );
            case BOBTYPE_BOB: // Bobfiles
                return new ArchivItem_Bob( dynamic_cast<const ArchivItem_Bob&>(item) );
            case BOBTYPE_BITMAP_SHADOW: // Schatten
                return new ArchivItem_Bitmap_Shadow( dynamic_cast<const ArchivItem_Bitmap_Shadow&>(item) );
            case BOBTYPE_MAP: // Mapfiles
                return new ArchivItem_Map( dynamic_cast<const ArchivItem_Map&>(item) );
            case BOBTYPE_TEXT: // Text
                return new ArchivItem_Text( dynamic_cast<const ArchivItem_Text&>(item) );
            case BOBTYPE_RAW: // Raw-Item
                return new ArchivItem_Raw( dynamic_cast<const ArchivItem_Raw&>(item) );
            case BOBTYPE_MAP_HEADER: // Mapheader-Item
                return new ArchivItem_Map_Header( dynamic_cast<const ArchivItem_Map_Header&>(item) );
            case BOBTYPE_INI: // INI-Datei-Item
                return new ArchivItem_Ini( dynamic_cast<const ArchivItem_Ini&>(item) );
            case BOBTYPE_BITMAP_RAW: // unkomprimiertes Bitmap
                return new ArchivItem_Bitmap_Raw( dynamic_cast<const ArchivItem_Bitmap_Raw&>(item) );
            default:
                return NULL;
        }
        return NULL;
    }

} // namespace libsiedler2
