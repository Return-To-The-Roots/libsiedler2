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

#ifndef ImageBufferARGB_h__
#define ImageBufferARGB_h__

#include "ColorARGB.h"
#include "GetFormat.h"
#include "PixelBuffer.h"
#include "enumTypes.h"

namespace libsiedler2 {

class PixelBufferARGB : public PixelBuffer<uint32_t>
{
public:
    PixelBufferARGB() {}
    PixelBufferARGB(uint16_t width, uint16_t height, ColorARGB defValue = ColorARGB())
        : PixelBuffer<uint32_t>(width, height, defValue.clrValue)
    {}
    ColorARGB get(uint16_t x, uint16_t y) const;
    ColorARGB get(uint32_t idx) const;
    void set(uint16_t x, uint16_t y, ColorARGB clr);
    void set(uint32_t idx, ColorARGB clr);
};

inline ColorARGB PixelBufferARGB::get(uint16_t x, uint16_t y) const
{
    return get(calcIdx(x, y));
}

inline libsiedler2::ColorARGB PixelBufferARGB::get(uint32_t idx) const
{
    return ColorARGB::fromBGRA(&pixels_[idx]);
}

inline void PixelBufferARGB::set(uint16_t x, uint16_t y, ColorARGB clr)
{
    return set(calcIdx(x, y), clr);
}

inline void PixelBufferARGB::set(uint32_t idx, ColorARGB clr)
{
    return clr.toBGRA(&pixels_[idx]);
}

namespace traits {
    template<>
    struct GetFormat<PixelBufferARGB>
    {
        BOOST_STATIC_CONSTEXPR TextureFormat value = FORMAT_BGRA;
    };
} // namespace traits

} // namespace libsiedler2

#endif // ImageBufferARGB_h__
