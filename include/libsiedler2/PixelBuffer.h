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

#ifndef PixelBuffer_h__
#define PixelBuffer_h__

#include <cassert>
#include <stdint.h>
#include <vector>

namespace libsiedler2 {
/// Describes a buffer of pixels
template<typename T_Pixel>
class PixelBuffer
{
public:
    typedef T_Pixel PixelType;

    PixelBuffer() : width_(0), height_(0) {}
    PixelBuffer(uint16_t width, uint16_t height, T_Pixel defValue);
    uint16_t getWidth() const { return width_; }
    uint16_t getHeight() const { return height_; }
    uint32_t getSize() const { return static_cast<uint32_t>(pixels_.size() * sizeof(T_Pixel)); }
    uint8_t* getPixelPtr() { return reinterpret_cast<uint8_t*>(&pixels_[0]); }
    const uint8_t* getPixelPtr() const { return reinterpret_cast<const uint8_t*>(&pixels_[0]); }
    std::vector<T_Pixel>& getPixels() { return pixels_; }
    const std::vector<T_Pixel>& getPixels() const { return pixels_; }
    void clear();

protected:
    uint32_t calcIdx(uint32_t x, uint32_t y) const
    {
        assert(x < width_ && y < height_);
        return y * width_ + x;
    }

private:
    uint16_t width_, height_;

protected:
    std::vector<T_Pixel> pixels_;
};

template<typename T_Pixel>
inline PixelBuffer<T_Pixel>::PixelBuffer(uint16_t width, uint16_t height, T_Pixel defValue)
    : width_(width), height_(height), pixels_(static_cast<uint32_t>(width) * height, defValue)
{
}

template<typename T_Pixel>
inline void libsiedler2::PixelBuffer<T_Pixel>::clear()
{
    pixels_.clear();
    width_ = height_ = 0;
}

} // namespace libsiedler2

#endif // PixelBuffer_h__
