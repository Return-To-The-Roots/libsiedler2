// Copyright (C) 2005 - 2026 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "libsiedler2/oem.h"
#include <boost/test/unit_test.hpp>
#include <string>

BOOST_AUTO_TEST_SUITE(OemConversion)

BOOST_AUTO_TEST_CASE(ConvertsAsciiUnchanged)
{
    const std::string input = "Hello World 123!";
    BOOST_TEST(libsiedler2::AnsiToOem(input) == input);
    BOOST_TEST(libsiedler2::OemToAnsi(input) == input);
}

BOOST_AUTO_TEST_CASE(ConvertsEmptyString)
{
    BOOST_TEST(libsiedler2::AnsiToOem("").empty());
    BOOST_TEST(libsiedler2::OemToAnsi("").empty());
}

BOOST_AUTO_TEST_CASE(ConvertsHighAnsiChars)
{
    std::string input;
    for(int c = 129; c < 256; ++c)
        input.push_back(static_cast<char>(c));

    const std::string oem = libsiedler2::AnsiToOem(input);
    BOOST_TEST_REQUIRE(oem.size() == input.size());

    const std::string back = libsiedler2::OemToAnsi(oem);
    BOOST_TEST_REQUIRE(back.size() == input.size());

    for(size_t i = 0; i < input.size(); ++i)
    {
        // chars mapped to <=128 don't round-trip - lossy table
        if(static_cast<uint8_t>(oem[i]) > 128)
            BOOST_TEST(back[i] == input[i]);
    }
}

BOOST_AUTO_TEST_CASE(ConvertsMixedString)
{
    const std::string input = "Test \xC4\xD6\xDC\xDF chars";
    const std::string oem = libsiedler2::AnsiToOem(input);
    BOOST_TEST_REQUIRE(oem.size() == input.size());

    const std::string back = libsiedler2::OemToAnsi(oem);
    BOOST_TEST(back == input);
}

BOOST_AUTO_TEST_CASE(ConvertsSpecificChars)
{
    BOOST_TEST(static_cast<uint8_t>(libsiedler2::AnsiToOem("\xC4")[0]) == 0x8E);
    BOOST_TEST(static_cast<uint8_t>(libsiedler2::AnsiToOem("\xD6")[0]) == 0x99);
    BOOST_TEST(static_cast<uint8_t>(libsiedler2::AnsiToOem("\xDC")[0]) == 0x9A);
    BOOST_TEST(static_cast<uint8_t>(libsiedler2::AnsiToOem("\xDF")[0]) == 0xE1);
}

BOOST_AUTO_TEST_SUITE_END()
