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

#ifndef ColorRGB_h__
#define ColorRGB_h__

#pragma once

#include <boost/static_assert.hpp>
#include <stdint.h>

namespace libsiedler2
{
	struct ColorRGB
	{
	    uint8_t r, g, b;
	    ColorRGB(): r(0), g(0), b(0){}
	    ColorRGB(uint8_t r, uint8_t g, uint8_t b): r(r), g(g), b(b){}
	    static ColorRGB fromBGR(const uint8_t* ptr);
	
	    void toBGR(uint8_t* ptr) const;

        uint8_t getRed() const { return r; }
        uint8_t getGreen() const { return g; }
        uint8_t getBlue() const { return b; }

	    bool operator==(const ColorRGB& rhs) const
	    {
	        return (r == rhs.r) && (g == rhs.g) && (b == rhs.b);
	    }
	    bool operator!=(const ColorRGB& rhs) const
	    {
	        return !(*this == rhs);
	    }
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
	BOOST_STATIC_ASSERT_MSG(sizeof(ColorRGB) == 3u, "Invalid size of ColorRGB");
}

#endif // ColorRGB_h__
