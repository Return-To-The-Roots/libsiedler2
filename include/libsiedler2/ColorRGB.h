// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <cstdint>

namespace libsiedler2 {
struct ColorRGB
{
    uint8_t r, g, b;
    constexpr ColorRGB() : r(0), g(0), b(0) {}
    constexpr ColorRGB(uint8_t r, uint8_t g, uint8_t b) : r(r), g(g), b(b) {}
    static ColorRGB fromBGR(const uint8_t* ptr);

    void toBGR(uint8_t* ptr) const;

    constexpr uint8_t getRed() const { return r; }
    constexpr uint8_t getGreen() const { return g; }
    constexpr uint8_t getBlue() const { return b; }

    constexpr bool operator==(const ColorRGB& rhs) const { return (r == rhs.r) && (g == rhs.g) && (b == rhs.b); }
    constexpr bool operator!=(const ColorRGB& rhs) const { return !(*this == rhs); }
};

inline ColorRGB ColorRGB::fromBGR(const uint8_t* ptr)
{
    return ColorRGB(ptr[2], ptr[1], ptr[0]);
}

inline void ColorRGB::toBGR(uint8_t* ptr) const
{
    ptr[0] = b;
    ptr[1] = g;
    ptr[2] = r;
}

// If this fails, then loading an array of RGB value (byte oriented: RGBRGB...) will fail
static_assert(sizeof(ColorRGB) == 3u, "Invalid size of ColorRGB");
} // namespace libsiedler2
