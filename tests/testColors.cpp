// Copyright (c) 2017 - 2017 Settlers Freaks (sf-team at siedler25.org)
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

#include "ColorOutput.h"
#include "libsiedler2/ColorBGRA.h"
#include "libsiedler2/ColorRGB.h"
#include <boost/predef/other.h>
#include <boost/test/unit_test.hpp>
#include <algorithm>
#include <array>

using libsiedler2::ColorBGRA;
using libsiedler2::ColorRGB;

BOOST_AUTO_TEST_CASE(ColorRGBCtor)
{
    // Default should be all zeros
    ColorRGB clrDef;
    BOOST_TEST(clrDef.getRed() == 0u);
    BOOST_TEST(clrDef.getGreen() == 0u);
    BOOST_TEST(clrDef.getBlue() == 0u);
    // Check setting of elements
    ColorRGB clr(1, 2, 3);
    BOOST_TEST(clr.getRed() == 1u);
    BOOST_TEST(clr.getGreen() == 2u);
    BOOST_TEST(clr.getBlue() == 3u);
}

BOOST_AUTO_TEST_CASE(ColorBGRACtor)
{
    // Default should be all zeros
    ColorBGRA clrDef;
    BOOST_TEST(clrDef.getAlpha() == 0u);
    BOOST_TEST(clrDef.getRed() == 0u);
    BOOST_TEST(clrDef.getGreen() == 0u);
    BOOST_TEST(clrDef.getBlue() == 0u);
    // Check setting of elements
    ColorBGRA clr(1, 2, 3, 4);
    BOOST_TEST(clr.getAlpha() == 4u);
    BOOST_TEST(clr.getRed() == 3u);
    BOOST_TEST(clr.getGreen() == 2u);
    BOOST_TEST(clr.getBlue() == 1u);
    // Construct from RGB
    ColorBGRA clrRGB(ColorRGB(5, 6, 7));
    BOOST_TEST(clrRGB.getAlpha() == 0xFF);
    BOOST_TEST(clrRGB.getRed() == 5u);
    BOOST_TEST(clrRGB.getGreen() == 6u);
    BOOST_TEST(clrRGB.getBlue() == 7u);
    BOOST_TEST(ColorRGB(5, 6, 7) == ColorRGB(clrRGB));
    // Construct from uint32_t
    ColorBGRA clrHex(0x12345678);
    BOOST_TEST(clrHex.getAlpha() == 0x12);
    BOOST_TEST(clrHex.getRed() == 0x34);
    BOOST_TEST(clrHex.getGreen() == 0x56);
    BOOST_TEST(clrHex.getBlue() == 0x78);
    BOOST_TEST(clrHex.asValue() == 0x12345678u);
}

BOOST_AUTO_TEST_CASE(ColorBGRASetters)
{
    ColorBGRA clr;
    // Set components
    clr.setAlpha(10);
    BOOST_TEST(clr == ColorBGRA(0, 0, 0, 10));
    clr.setRed(20);
    BOOST_TEST(clr == ColorBGRA(0, 0, 20, 10));
    clr.setGreen(30);
    BOOST_TEST(clr == ColorBGRA(0, 30, 20, 10));
    clr.setBlue(40);
    BOOST_TEST(clr == ColorBGRA(40, 30, 20, 10));
}

BOOST_AUTO_TEST_CASE(ColorRGBBuffer)
{
    // Read from buffer
    const ColorRGB checkClr(1, 42, 24);
    const ColorRGB checkClr2(5, 6, 23);
    const std::array<uint8_t, 3> buf = {checkClr.getBlue(), checkClr.getGreen(), checkClr.getRed()};
    BOOST_TEST(ColorRGB::fromBGR(&buf[0]) == checkClr);
    // Write to buffer. Check by reading from it which is tested
    {
        std::array<uint8_t, 3> bufWrite;
        checkClr2.toBGR(&bufWrite[0]);
        BOOST_TEST(ColorRGB::fromBGR(&bufWrite[0]) == checkClr2);
    }

    const std::array<uint8_t, 6> buf2 = {checkClr.getRed(),  checkClr.getGreen(),  checkClr.getBlue(),
                                         checkClr2.getRed(), checkClr2.getGreen(), checkClr2.getBlue()};
    static_assert(sizeof(ColorRGB) == 3u, "Padding added to ColorRGB. Cannot read it as buffer");
    // Interpreting a buffer of RGBRGB values as ColorRGB should be possible
    const auto* clrs = reinterpret_cast<const ColorRGB*>(&buf2[0]);
    BOOST_TEST(clrs[0] == checkClr);
    BOOST_TEST(clrs[1] == checkClr2);
}

BOOST_AUTO_TEST_CASE(ColorBGRABuffer)
{
    // Read from buffer
    const ColorBGRA clr(1, 42, 24, 99);
    const std::array<uint8_t, 4> buf = {clr.getBlue(), clr.getGreen(), clr.getRed(), clr.getAlpha()};
    BOOST_TEST(ColorBGRA(buf.data()) == clr);
    BOOST_TEST(ColorBGRA::fromBGRA(buf.data()) == clr);
    // Write to buffer
    std::array<uint8_t, 4> buf2{};
    clr.toBGRA(buf2.data());
    BOOST_TEST(buf == buf2, boost::test_tools::per_element());

    const ColorBGRA clr2(5, 6, 23, 17);
    std::array<uint8_t, 8> bufBGRA = {clr.getBlue(),  clr.getGreen(),  clr.getRed(),  clr.getAlpha(),
                                      clr2.getBlue(), clr2.getGreen(), clr2.getRed(), clr2.getAlpha()};
    static_assert(sizeof(ColorBGRA) == 4u, "Padding added to ColorBGRA. Cannot read it as buffer");
    static_assert(alignof(ColorBGRA) == 1u, "Alignment of ColorBGRA is not 1. Cannot read it as buffer");
    // Interpreting a buffer of BGRABGRA values as ColorBGRA should be possible
    const auto* clrs = reinterpret_cast<const ColorBGRA*>(&bufBGRA[0]);
    BOOST_TEST(clrs[0] == clr);
    BOOST_TEST(clrs[1] == clr2);
}
