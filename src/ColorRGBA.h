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
#include <boost/endian/conversion.hpp>
#include <stdint.h>

namespace libsiedler2
{
    /// Stores color information as a word
    /// If this is written to memory it will result in A being the first byte which is
    /// in word order: ABGR on little endian and RGBA in big endian
    struct ColorRGBA
    {
        uint32_t clrValue;
        ColorRGBA(): clrValue(0){}
        explicit ColorRGBA(uint32_t clrValue): clrValue(clrValue){}
        /// Create a RGBA color.
        ColorRGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 0xFF);
        ColorRGBA(ColorRGB clrRGB);
        /// Create a color from a byte oriented buffer (A first, then B, G, R)
        static ColorRGBA fromABGR(const uint8_t* ptr);
        /// Create a color from a byte oriented buffer (R first, then G, B, A)
        static ColorRGBA fromRGBA(const uint8_t* ptr);
        /// Create a color from a byte oriented buffer (A first, then B, G, R)
        static ColorRGBA fromABGR(const uint32_t* ptr);
        /// Create a color from a byte oriented buffer (R first, then G, B, A)
        static ColorRGBA fromRGBA(const uint32_t* ptr);

        /// Write the color to a byte oriented buffer (A first, then B, G, R)
        void toABGR(uint8_t* ptr);
        /// Write the color to a byte oriented buffer (R first, then G, B, A)
        void toRGBA(uint8_t* ptr);
        /// Write the color to a byte oriented buffer (A first, then B, G, R)
        void toABGR(uint32_t* ptr);
        /// Write the color to a byte oriented buffer (R first, then G, B, A)
        void toRGBA(uint32_t* ptr);

        uint8_t getAlpha() const;
        void setAlpha(uint8_t val);
        uint8_t getRed() const;
        void setRed(uint8_t val);
        uint8_t getGreen() const;
        void setGreen(uint8_t val);
        uint8_t getBlue() const;
        void setBlue(uint8_t val);

        bool operator==(const ColorRGBA& rhs) const
        {
            return (clrValue == rhs.clrValue);
        }
        bool operator!=(const ColorRGBA& rhs) const
        {
            return !(*this == rhs);
        }
    };

    inline ColorRGBA::ColorRGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        clrValue = r << 24 | g << 16 | b << 8 | a;
    }

    inline ColorRGBA::ColorRGBA(ColorRGB clrRGB)
    {
        clrValue = ColorRGBA(clrRGB.r, clrRGB.g, clrRGB.b).clrValue;
    }

    inline ColorRGBA ColorRGBA::fromABGR(const uint8_t* ptr)
    {
        return fromABGR(reinterpret_cast<const uint32_t*>(ptr));
    }

    inline ColorRGBA ColorRGBA::fromABGR(const uint32_t* ptr)
    {
        // This is little endian ABGR word format
        return ColorRGBA(boost::endian::little_to_native(*ptr));
    }

    inline ColorRGBA ColorRGBA::fromRGBA(const uint8_t* ptr)
    {
        return fromRGBA(reinterpret_cast<const uint32_t*>(ptr));
    }

    inline ColorRGBA ColorRGBA::fromRGBA(const uint32_t* ptr)
    {
        // This is big endian RGBA word format
        return ColorRGBA(boost::endian::big_to_native(*ptr));
    }

    inline void ColorRGBA::toABGR(uint8_t* ptr)
    {
        toABGR(reinterpret_cast<uint32_t*>(ptr));
    }

    inline void ColorRGBA::toABGR(uint32_t* ptr)
    {
        *ptr = boost::endian::native_to_little(clrValue);
    }

    inline void ColorRGBA::toRGBA(uint8_t* ptr)
    {
        toRGBA(reinterpret_cast<uint32_t*>(ptr));
    }

    inline void ColorRGBA::toRGBA(uint32_t* ptr)
    {
        *ptr = boost::endian::native_to_big(clrValue);
    }

    inline uint8_t ColorRGBA::getRed() const { return clrValue >> 24; }
    inline void ColorRGBA::setRed(uint8_t val) { clrValue = (clrValue & 0x00FFFFFF) | (val << 24); }
    inline uint8_t ColorRGBA::getGreen() const { return clrValue >> 16; }
    inline void ColorRGBA::setGreen(uint8_t val) { clrValue = (clrValue & 0xFF00FFFF) | (val << 16); }
    inline uint8_t ColorRGBA::getBlue() const { return clrValue >> 8; }
    inline void ColorRGBA::setBlue(uint8_t val) { clrValue = (clrValue & 0xFFFF00FF) | (val << 8); }
    inline uint8_t ColorRGBA::getAlpha() const { return clrValue; }
    inline void ColorRGBA::setAlpha(uint8_t val) { clrValue = (clrValue & 0xFFFFFF00) | val; }
}

#endif // ColorARGB_h__
