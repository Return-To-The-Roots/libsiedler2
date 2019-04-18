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

#ifndef ColorARGB_h__
#define ColorARGB_h__

#pragma once

#include "ColorRGB.h"
#include <cstdint>

namespace libsiedler2 {
/// Stores color information as a word
/// If this is written to memory it will result in A being the first byte which is
/// in word order: ABGR on little endian and RGBA in big endian
struct ColorARGB
{
    uint32_t clrValue;
    ColorARGB() : clrValue(0) {}
    explicit ColorARGB(uint32_t clrValue) : clrValue(clrValue) {}
    /// Create a ARGB color.
    ColorARGB(uint8_t a, uint8_t r, uint8_t g, uint8_t b);
    ColorARGB(ColorRGB clrRGB, uint8_t a = 0xFF);

    operator ColorRGB() const;
    /// Create a color from a byte oriented buffer (A first, then R, G, B)
    static ColorARGB fromARGB(const uint8_t* ptr);
    /// Create a color from a byte oriented buffer (B first, then G, R, A)
    static ColorARGB fromBGRA(const uint8_t* ptr);
    /// Create a color from a byte oriented buffer (A first, then R, G, B)
    static ColorARGB fromARGB(const uint32_t* ptr);
    /// Create a color from a byte oriented buffer (B first, then G, R, A)
    static ColorARGB fromBGRA(const uint32_t* ptr);

    /// Write the color to a byte oriented buffer (A first, then R, G, B)
    void toARGB(uint8_t* ptr) const;
    /// Write the color to a byte oriented buffer (R first, then G, R, A)
    void toBGRA(uint8_t* ptr) const;
    /// Write the color to a byte oriented buffer (A first, then R, G, B)
    void toARGB(uint32_t* ptr) const;
    /// Write the color to a byte oriented buffer (R first, then G, R, A)
    void toBGRA(uint32_t* ptr) const;

    uint8_t getAlpha() const;
    void setAlpha(uint8_t val);
    uint8_t getRed() const;
    void setRed(uint8_t val);
    uint8_t getGreen() const;
    void setGreen(uint8_t val);
    uint8_t getBlue() const;
    void setBlue(uint8_t val);
};
//-V:ColorARGB:801

inline bool operator==(const ColorARGB& lhs, const ColorARGB& rhs)
{
    return (lhs.clrValue == rhs.clrValue);
}
inline bool operator!=(const ColorARGB& lhs, const ColorARGB& rhs)
{
    return !(lhs == rhs);
}

inline ColorARGB::ColorARGB(uint8_t a, uint8_t r, uint8_t g, uint8_t b)
{
    clrValue = a << 24 | r << 16 | g << 8 | b;
}

inline ColorARGB::ColorARGB(ColorRGB clrRGB, uint8_t a) : ColorARGB(a, clrRGB.r, clrRGB.g, clrRGB.b) {}

inline ColorARGB::operator ColorRGB() const
{
    return ColorRGB(getRed(), getGreen(), getBlue());
}

inline ColorARGB ColorARGB::fromARGB(const uint8_t* ptr)
{
    return ColorARGB(ptr[0], ptr[1], ptr[2], ptr[3]);
}

inline ColorARGB ColorARGB::fromARGB(const uint32_t* ptr)
{
    return fromARGB(reinterpret_cast<const uint8_t*>(ptr)); //-V206
}

#if defined(__x86_64__) || defined(_M_X64) || defined(__i386) || defined(_M_IX86)
// On x86 (which is little endian), unaligned loads are permitted
#define RTTR_USE_UNALIGNED_ACCESS 1
#endif

inline ColorARGB ColorARGB::fromBGRA(const uint8_t* ptr)
{
#if RTTR_USE_UNALIGNED_ACCESS
    return fromBGRA(reinterpret_cast<const uint32_t*>(ptr)); //-V206
#else
    return ColorARGB(ptr[3], ptr[2], ptr[1], ptr[0]);
#endif
}

inline ColorARGB ColorARGB::fromBGRA(const uint32_t* ptr)
{
#if RTTR_USE_UNALIGNED_ACCESS
    return ColorARGB(*ptr);
#else
    return fromBGRA(reinterpret_cast<const uint8_t*>(ptr));
#endif
}

inline void ColorARGB::toARGB(uint8_t* ptr) const
{
    ptr[0] = getAlpha();
    ptr[1] = getRed();
    ptr[2] = getGreen();
    ptr[3] = getBlue();
}

inline void ColorARGB::toARGB(uint32_t* ptr) const
{
    toARGB(reinterpret_cast<uint8_t*>(ptr)); //-V206
}

inline void ColorARGB::toBGRA(uint8_t* ptr) const
{
#if RTTR_USE_UNALIGNED_ACCESS
    toBGRA(reinterpret_cast<uint32_t*>(ptr)); //-V206
#else
    ptr[0] = getBlue();
    ptr[1] = getGreen();
    ptr[2] = getRed();
    ptr[3] = getAlpha();
#endif
}

inline void ColorARGB::toBGRA(uint32_t* ptr) const
{
#if RTTR_USE_UNALIGNED_ACCESS
    *ptr = clrValue;
#else
    toBGRA(reinterpret_cast<uint8_t*>(ptr));
#endif
}

#undef RTTR_USE_UNALIGNED_ACCESS

inline uint8_t ColorARGB::getAlpha() const
{
    return clrValue >> 24;
}
inline void ColorARGB::setAlpha(uint8_t val)
{
    clrValue = (clrValue & 0x00FFFFFF) | val << 24;
}
inline uint8_t ColorARGB::getRed() const
{
    return clrValue >> 16;
}
inline void ColorARGB::setRed(uint8_t val)
{
    clrValue = (clrValue & 0xFF00FFFF) | (val << 16);
}
inline uint8_t ColorARGB::getGreen() const
{
    return clrValue >> 8;
}
inline void ColorARGB::setGreen(uint8_t val)
{
    clrValue = (clrValue & 0xFFFF00FF) | (val << 8);
}
inline uint8_t ColorARGB::getBlue() const
{
    return clrValue;
}
inline void ColorARGB::setBlue(uint8_t val)
{
    clrValue = (clrValue & 0xFFFFFF00) | val;
}
} // namespace libsiedler2

#endif // ColorARGB_h__
