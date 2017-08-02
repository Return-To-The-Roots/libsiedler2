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

#pragma once

#ifndef PixelBufferPaletted_h__
#define PixelBufferPaletted_h__

#include "PixelBuffer.h"
#include "ArchivItem_Palette.h"
#include <stdint.h>

namespace libsiedler2
{
    
    class PixelBufferPaletted: public PixelBuffer<uint8_t>
    {
    public:
        PixelBufferPaletted(){}
        PixelBufferPaletted(uint16_t width, uint16_t height): PixelBuffer<uint8_t>(width, height, TRANSPARENT_INDEX){}
        uint8_t get(uint16_t x, uint16_t y) const;
        uint8_t get(uint32_t idx) const;
        void set(uint16_t x, uint16_t y, uint8_t clrIdx);
        void set(uint32_t idx, uint8_t clrIdx);
    };

    inline uint8_t PixelBufferPaletted::get(uint16_t x, uint16_t y) const
    {
        return pixels_[calcIdx(x, y)];
    }

    inline void PixelBufferPaletted::set(uint16_t x, uint16_t y, uint8_t clrIdx)
    {
        pixels_[calcIdx(x, y)] = clrIdx;
    }

}

#endif // PixelBufferPaletted_h__
