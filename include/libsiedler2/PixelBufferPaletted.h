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

#ifndef PixelBufferPaletted_h__
#define PixelBufferPaletted_h__

#include "ArchivItem_Palette.h"
#include "PixelBuffer.h"
#include <cstdint>

namespace libsiedler2 {

class PixelBufferPaletted : public PixelBuffer<uint8_t>
{
public:
    PixelBufferPaletted() = default;
    PixelBufferPaletted(uint16_t width, uint16_t height, uint8_t defValue = ArchivItem_Palette::DEFAULT_TRANSPARENT_IDX)
        : PixelBuffer<uint8_t>(width, height, defValue)
    {}
};

namespace traits {
    template<>
    struct GetFormat<PixelBufferPaletted>
    {
        static constexpr TextureFormat value = FORMAT_PALETTED;
    };
} // namespace traits

} // namespace libsiedler2

#endif // PixelBufferPaletted_h__
