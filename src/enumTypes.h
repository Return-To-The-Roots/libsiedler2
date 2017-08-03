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

#ifndef enumTypes_h__
#define enumTypes_h__

#include <stdint.h>

namespace libsiedler2
{
    /** Supported texture formats */
    enum TextureFormat
    {
        FORMAT_PALETTED = 1,
        FORMAT_BGRA = 4
    };

     /** Supported bob types */
    enum BobType
    {
        BOBTYPE_NONE = 0,
        BOBTYPE_SOUND = 1,
        BOBTYPE_BITMAP_RLE = 2,
        BOBTYPE_FONT = 3,
        BOBTYPE_BITMAP_PLAYER = 4,
        BOBTYPE_PALETTE = 5,
        BOBTYPE_BOB = 6,
        BOBTYPE_BITMAP_SHADOW = 7,
        BOBTYPE_MAP = 8,
        BOBTYPE_TEXT = 9,
        BOBTYPE_RAW = 10,
        BOBTYPE_MAP_HEADER = 11,
        BOBTYPE_INI = 12,
        BOBTYPE_UNSET,
        BOBTYPE_BITMAP_RAW = 14
    };
    static const uint16_t NUM_BOB_TYPES = BOBTYPE_BITMAP_RAW + 1;

    enum SoundType
    {
        SOUNDTYPE_NONE = 0,
        SOUNDTYPE_WAVE = 1,
        SOUNDTYPE_MIDI = 2,
        SOUNDTYPE_XMIDI = 3,
        SOUNDTYPE_OTHER = 4
    };

    /**
     * Enumeration of animals which can be stored in SWD format. 
     * For each value there's a hex-value assigned for the animal 
     * representation in SWD.
     * @see http://settlers2.net/documentation/world-map-file-format-wldswd/
     */
    enum Animal
    {
        A_None    = 0x00,
        A_Rabbit  = 0x01,
        A_Fox     = 0x02,
        A_Stag    = 0x03,
        A_Deer    = 0x04,
        A_Duck    = 0x05,
        A_Sheep   = 0x06,
        A_Deer2   = 0x07,
        A_Duck2   = 0x08,
        A_Donkey  = 0x09
    };
    
    /**
     * Enumeration of resource types which can be stored in SWD format.
     * For each value there's a hex-value assigned for the representation
     * of the resource in SWD.
     * @see http://settlers2.net/documentation/world-map-file-format-wldswd/
     */
    enum Resource
    {
        R_None    = 0x00,
        R_Water   = 0x21,
        R_Fish    = 0x87,
        R_Coal    = 0x40,
        R_Iron    = 0x48,
        R_Gold    = 0x50,
        R_Granite = 0x58
    };
    
    /**
     * Enumeration for object types stored in SWD. In combination with the
     * object information it makes a real object (therefore the "info"-comments).
     */
    enum ObjectType
    {
        OT_Empty            = 0x00,
        OT_Stone_Begin      = 0x01, // info: 0xCC / 0xCD
        OT_Stone_End        = 0x06,
        OT_TreeOrPalm_Begin = 0x30, // info: 0xC4 / 0xC5
        OT_TreeOrPalm_End   = 0x37,
        OT_Tree1_Begin      = 0x70, // info: 0xC4
        OT_Tree1_End        = 0x77,
        OT_Tree2_Begin      = 0xB0, // info: 0xC4
        OT_Tree2_End        = 0xB7,
        OT_Palm_Begin       = 0xF0, // info: 0xC4
        OT_Palm_End         = 0xF7
    };
    
    /**
     * Enumeration for object info stored in SWD.
     */
    enum ObjectInfo
    {
        OI_Empty            = 0x00,
        OI_Stone1           = 0xCC,
        OI_Stone2           = 0xCD,
        OI_TreeOrPalm       = 0xC4,
        OI_Palm             = 0xC5,
        OI_HeadquarterMask  = 0x80  // used in combination with object
                                    // type 0x00-0x06 for player number
    };
    
    /**
     * Hex value to combine with texture values to enable harbor placement.
     */
    const uint8_t HARBOR_MASK = 0x40;
    
} // namspace libsiedler2

#endif // enumTypes_h__
