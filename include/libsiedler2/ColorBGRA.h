// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "ColorRGB.h"
#include <array>
#include <cstring>

namespace libsiedler2 {
/// Color with 4 channels: Blue, Green, Red, Alpha
/// In memory it will be stored in this order
struct ColorBGRA
{
    std::array<uint8_t, 4> value = {};

    ColorBGRA() = default;
    /// Create a ARGB color.
    explicit ColorBGRA(uint8_t b, uint8_t g, uint8_t r, uint8_t a);
    ColorBGRA(ColorRGB clrRGB, uint8_t a = 0xFF);
    /// Construct from a BGRA buffer (bytes in this order)
    explicit ColorBGRA(const void* bgraBuffer);
    /// Convert from a 4 byte value. Highest byte is alpha
    explicit ColorBGRA(uint32_t clr);

    /// Return as a 4 byte value. Highest byte is alpha
    uint32_t asValue() const;
    operator ColorRGB() const;

    /// Create a color from a byte oriented buffer (B first, then G, R, A)
    static ColorBGRA fromBGRA(const void* bgraBuffer);
    /// Write the color to a byte oriented buffer (B first, then G, R, A)
    void toBGRA(void* bgraBuffer) const;

    uint8_t getAlpha() const { return value[3]; }
    void setAlpha(uint8_t val) { value[3] = val; }
    uint8_t getRed() const { return value[2]; }
    void setRed(uint8_t val) { value[2] = val; }
    uint8_t getGreen() const { return value[1]; }
    void setGreen(uint8_t val) { value[1] = val; }
    uint8_t getBlue() const { return value[0]; }
    void setBlue(uint8_t val) { value[0] = val; }
};
//-V:ColorBGRA:801

inline bool operator==(const ColorBGRA& lhs, const ColorBGRA& rhs)
{
    return (lhs.value == rhs.value);
}
inline bool operator!=(const ColorBGRA& lhs, const ColorBGRA& rhs)
{
    return !(lhs == rhs);
}

inline ColorBGRA::ColorBGRA(uint8_t b, uint8_t g, uint8_t r, uint8_t a) : value{b, g, r, a} {}
inline ColorBGRA::ColorBGRA(ColorRGB clrRGB, uint8_t a) : ColorBGRA(clrRGB.b, clrRGB.g, clrRGB.r, a) {}
inline ColorBGRA::ColorBGRA(const void* bgraBuffer)
{
    std::memcpy(value.data(), bgraBuffer, 4);
}
inline ColorBGRA::ColorBGRA(uint32_t clr)
{
    value[0] = clr;
    value[1] = clr >> 8;
    value[2] = clr >> 16;
    value[3] = clr >> 24;
}

inline uint32_t ColorBGRA::asValue() const
{
    return value[0] | (value[1] << 8) | (value[2] << 16) | (value[3] << 24);
}

inline ColorBGRA::operator ColorRGB() const
{
    return ColorRGB(getRed(), getGreen(), getBlue());
}

inline ColorBGRA ColorBGRA::fromBGRA(const void* bgraBuffer)
{
    return ColorBGRA{bgraBuffer};
}

inline void ColorBGRA::toBGRA(void* bgraBuffer) const
{
    std::memcpy(bgraBuffer, value.data(), 4);
}
} // namespace libsiedler2
