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
// along with Return To The Roots. If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include <cstdint>

namespace libsiedler2 {
/** Supported texture formats */
enum class TextureFormat
{
    Original,
    Paletted,
    BGRA
};

/** Supported bob types */
enum class BobType
{
    // Order matters
    None,
    Sound,
    BitmapRLE,
    Font,
    BitmapPlayer,
    Palette,
    Bob,
    BitmapShadow,
    Map,
    Text,
    Raw,
    MapHeader,
    // Extension (not in S2)
    Ini,
    Unset,
    Bitmap,
    PaletteAnim
};
static constexpr auto NUM_BOB_TYPES = static_cast<unsigned>(BobType::PaletteAnim) + 1u;

enum class SoundType
{
    // Order matters
    None,
    Wave,
    Midi,
    XMidi,
    // Extension (not in S2)
    MP3,
    OGG,
    Other = 99
};

/**
 * Enumeration of animals which can be stored in SWD format.
 * For each value there's a hex-value assigned for the animal
 * representation in SWD.
 * @see http://settlers2.net/documentation/world-map-file-format-wldswd/
 */
enum class Animal : uint8_t
{
    // Order matters
    None,
    Rabbit,
    Fox,
    Stag,
    Deer,
    Duck,
    Sheep,
    Deer2,
    Duck2,
    Donkey
};

/**
 * Enumeration of resource types which can be stored in SWD format.
 * For each value there's a hex-value assigned for the representation
 * of the resource in SWD.
 * @see http://settlers2.net/documentation/world-map-file-format-wldswd/
 */
enum Resource
{
    R_None = 0x00,
    R_Water = 0x21,
    R_Fish = 0x87,
    R_Coal = 0x40,
    R_Iron = 0x48,
    R_Gold = 0x50,
    R_Granite = 0x58
};

/**
 * Enumeration for object types stored in SWD. In combination with the
 * object information it makes a real object (therefore the "info"-comments).
 */
enum ObjectInfo
{
    OI_Empty = 0x00,
    OI_Stone_Begin = 0x01, // info: 0xCC / 0xCD
    OI_Stone_End = 0x06,
    OI_TreeOrPalm_Begin = 0x30, // info: 0xC4 / 0xC5
    OI_TreeOrPalm_End = 0x37,
    OI_Tree1_Begin = 0x70, // info: 0xC4
    OI_Tree1_End = 0x77,
    OI_Tree2_Begin = 0xB0, // info: 0xC4
    OI_Tree2_End = 0xB7,
    OI_Palm_Begin = 0xF0, // info: 0xC4
    OI_Palm_End = 0xF7
};

/**
 * Enumeration for object info stored in SWD.
 */
enum ObjectType
{
    OT_Empty = 0x00,
    OT_Stone1 = 0xCC,
    OT_Stone2 = 0xCD,
    OT_TreeOrPalm = 0xC4,
    OT_Palm = 0xC5,
    OT_HeadquarterMask = 0x80 // used in combination with object
                              // type 0x00-0x06 for player number
};

/**
 * Hex value to combine with texture values to enable harbor placement.
 */
const uint8_t HARBOR_MASK = 0x40;

} // namespace libsiedler2
