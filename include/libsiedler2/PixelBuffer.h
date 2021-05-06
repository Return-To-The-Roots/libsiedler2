// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <cassert>
#include <cstdint>
#include <vector>

namespace libsiedler2 {
/// Describes a buffer of pixels
template<typename T_Pixel>
class PixelBuffer
{
public:
    using PixelType = T_Pixel;
    using Container = std::vector<T_Pixel>;
    // Compatibility with Boost < 1.67
    using iterator = typename Container::iterator;
    using const_iterator = typename Container::const_iterator;
    using value_type = typename Container::value_type;
    auto size() const { return pixels_.size(); }

    PixelBuffer() : width_(0), height_(0) {}
    PixelBuffer(uint16_t width, uint16_t height, T_Pixel defValue);
    uint16_t getWidth() const { return width_; }
    uint16_t getHeight() const { return height_; }
    uint32_t getSizeInBytes() const { return static_cast<uint32_t>(getNumPixels() * sizeof(T_Pixel)); }
    uint32_t getNumPixels() const { return static_cast<uint32_t>(pixels_.size()); }
    uint8_t* getPixelPtr() { return pixels_.empty() ? nullptr : reinterpret_cast<uint8_t*>(&pixels_[0]); }
    const uint8_t* getPixelPtr() const
    {
        return pixels_.empty() ? nullptr : reinterpret_cast<const uint8_t*>(&pixels_[0]);
    }
    std::vector<T_Pixel>& getPixels() { return pixels_; }
    const std::vector<T_Pixel>& getPixels() const { return pixels_; }

    PixelType get(uint16_t x, uint16_t y) const { return get(calcIdx(x, y)); }
    PixelType get(uint32_t idx) const { return pixels_[idx]; }
    void set(uint16_t x, uint16_t y, PixelType clr) { set(calcIdx(x, y), clr); }
    void set(uint32_t idx, PixelType clr) { pixels_[idx] = clr; }

    auto begin() { return pixels_.begin(); }
    auto begin() const { return pixels_.begin(); }
    auto end() { return pixels_.end(); }
    auto end() const { return pixels_.end(); }
    void clear();

    uint32_t calcIdx(uint32_t x, uint32_t y) const
    {
        assert(x < width_ && y < height_);
        return y * width_ + x;
    }

private:
    uint16_t width_, height_;

protected:
    Container pixels_;
};

template<typename T_Pixel>
inline PixelBuffer<T_Pixel>::PixelBuffer(uint16_t width, uint16_t height, T_Pixel defValue)
    : width_(width), height_(height), pixels_(static_cast<size_t>(width) * height, defValue)
{}

template<typename T_Pixel>
inline void libsiedler2::PixelBuffer<T_Pixel>::clear()
{
    pixels_.clear();
    width_ = height_ = 0;
}

} // namespace libsiedler2
