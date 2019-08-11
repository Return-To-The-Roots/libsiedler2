// Copyright (c) 2005 - 2019 Settlers Freaks (sf-team at siedler25.org)
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

#ifndef PixelBufferRef_h__
#define PixelBufferRef_h__

#include <cassert>
#include <cstdint>
#include <type_traits>

namespace libsiedler2 {
class ArchivItem_Palette;

template<typename T_PixelType>
class PixelBufferRefBase
{
public:
    using PixelType = T_PixelType;

    PixelBufferRefBase(PixelType* buf, uint16_t width, uint16_t height) : pixels_(buf), width_(width), height_(height) {}
    uint16_t getWidth() const { return width_; }
    uint16_t getHeight() const { return height_; }
    PixelType* getPixelPtr(uint32_t x, uint32_t y) { return &pixels_[calcIdx(x, y)]; }
    const PixelType* getPixelPtr(uint32_t x, uint32_t y) const { return &pixels_[calcIdx(x, y)]; }

    uint32_t calcIdx(uint32_t x, uint32_t y) const
    {
        assert(x < width_ && y < height_);
        return y * width_ + x;
    }

private:
    PixelType* pixels_;
    uint16_t width_, height_;
};

using PixelBufferBGRARef = PixelBufferRefBase<uint32_t>;

class PixelBufferPalettedRef : public PixelBufferRefBase<uint8_t>
{
    const ArchivItem_Palette& pal_;

public:
    PixelBufferPalettedRef(PixelType* buf, uint16_t width, uint16_t height, const ArchivItem_Palette& pal)
        : PixelBufferRefBase(buf, width, height), pal_(pal)
    {}
    const ArchivItem_Palette& getPalette() const { return pal_; }
};
} // namespace libsiedler2

#endif // PixelBufferRef_h__
