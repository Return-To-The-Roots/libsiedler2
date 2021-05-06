// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

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

    PixelBufferRefBase(PixelType* buf, uint16_t width, uint16_t height) : pixels_(buf), width_(width), height_(height)
    {}
    uint16_t getWidth() const { return width_; }
    uint16_t getHeight() const { return height_; }
    uint32_t getSizeInBytes() const { return getNumPixels() * sizeof(PixelType); }
    uint32_t getNumPixels() const { return static_cast<uint32_t>(width_) * static_cast<uint32_t>(height_); }
    uint8_t* getPixelPtr() { return reinterpret_cast<uint8_t*>(pixels_); }
    const uint8_t* getPixelPtr() const { return reinterpret_cast<const uint8_t*>(pixels_); }
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
