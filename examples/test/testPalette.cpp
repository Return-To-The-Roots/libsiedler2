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
#include "cmpFiles.h"
#include "config.h"
#include "libsiedler2/Archiv.h"
#include "libsiedler2/ArchivItem_Palette.h"
#include "libsiedler2/ColorARGB.h"
#include "libsiedler2/ColorRGB.h"
#include "libsiedler2/libsiedler2.h"
#include <boost/filesystem.hpp>
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(Palette)

BOOST_AUTO_TEST_CASE(ReadWritePalette)
{
    std::string palPath = "pal5.act";
    std::string palOutPath = testOutputPath + "/" + palPath;
    BOOST_REQUIRE(bfs::exists(palPath));
    libsiedler2::Archiv act;
    BOOST_REQUIRE_EQUAL(libsiedler2::Load(palPath, act), 0);
    BOOST_REQUIRE_EQUAL(libsiedler2::Write(palOutPath, act), 0);
    BOOST_REQUIRE(testFilesEqual(palOutPath, palPath));
}

void createBBM(const std::string& palPath, const std::string& palOutPath)
{
    BOOST_REQUIRE(bfs::exists(palPath));
    libsiedler2::Archiv act;
    BOOST_REQUIRE_EQUAL(libsiedler2::Load(palPath, act), 0);
    libsiedler2::ArchivItem_Palette* pal = dynamic_cast<libsiedler2::ArchivItem_Palette*>(act[0]);
    act.pushC(*pal);
    pal->set(10, libsiedler2::ColorRGB(11, 22, 33));
    BOOST_REQUIRE_EQUAL(libsiedler2::Write(palOutPath, act), 0);
}

BOOST_AUTO_TEST_CASE(ReadWriteBBM)
{
    // createBBM("pal5.act", "testFiles/pal.bbm");
    std::string palPath = "testFiles/pal.bbm";
    std::string palOutPath = testOutputPath + "/palOut.bbm";
    BOOST_REQUIRE(bfs::exists(palPath));
    libsiedler2::Archiv act, act2;
    BOOST_REQUIRE_EQUAL(libsiedler2::Load(palPath, act), 0);
    BOOST_REQUIRE_EQUAL(libsiedler2::Load("pal5.act", act2), 0);
    libsiedler2::ArchivItem_Palette* pal = dynamic_cast<libsiedler2::ArchivItem_Palette*>(act[0]);
    BOOST_REQUIRE(pal);
    BOOST_REQUIRE_EQUAL(pal->get(10), libsiedler2::ColorRGB(11, 22, 33));
    BOOST_REQUIRE_EQUAL(libsiedler2::Write(palOutPath, act), 0);
    BOOST_REQUIRE(testFilesEqual(palOutPath, palPath));
}

BOOST_AUTO_TEST_CASE(GetColor)
{
    libsiedler2::ArchivItem_Palette pal;
    // Default all black
    for(unsigned i = 0; i < 256; i++)
        BOOST_REQUIRE_EQUAL(pal[i], libsiedler2::ColorRGB(0, 0, 0));
    for(unsigned i = 0; i < 256; i++)
        pal.set(i, libsiedler2::ColorRGB(i, i + 1, i + 2));
    for(unsigned i = 0; i < 256; i++)
    {
        BOOST_REQUIRE_EQUAL(pal.get(i), libsiedler2::ColorRGB(i, i + 1, i + 2));
        BOOST_REQUIRE_EQUAL(pal[i], libsiedler2::ColorRGB(i, i + 1, i + 2));
    }
    // Out of bounds throws
    BOOST_REQUIRE_THROW(pal[256], std::exception);
    // Reverse: Lookup
    for(unsigned i = 0; i < 256; i++)
    {
        BOOST_REQUIRE_EQUAL(pal.lookup(libsiedler2::ColorRGB(i, i + 1, i + 2)), i);
        BOOST_REQUIRE_EQUAL(pal.lookupOrDef(libsiedler2::ColorRGB(i, i + 1, i + 2)), i);
    }
    // Non existent color throws
    BOOST_REQUIRE_THROW(pal.lookup(libsiedler2::ColorRGB(10, 20, 30)), std::exception);
    // Return default
    BOOST_REQUIRE_EQUAL(pal.lookupOrDef(libsiedler2::ColorRGB(10, 20, 30)), 0u);
    BOOST_REQUIRE_EQUAL(pal.lookupOrDef(libsiedler2::ColorRGB(10, 20, 30), 1), 1u);
    BOOST_REQUIRE_EQUAL(pal.lookupOrDef(libsiedler2::ColorRGB(10, 20, 30), 2), 2u);
    // Existent and different default value
    BOOST_REQUIRE_EQUAL(pal.lookupOrDef(pal[5], 2), 5u);

    std::vector<uint8_t> clrBuf(256 * 4);
    pal.copy(&clrBuf[0], clrBuf.size(), false);
    for(unsigned i = 0; i < 256; i++)
    {
        libsiedler2::ColorARGB clr = pal[i];
        if(i == libsiedler2::TRANSPARENT_INDEX)
            clr = libsiedler2::ColorARGB(0, 0, 0, 0);

        // BGRA buffer:
        BOOST_REQUIRE_EQUAL(libsiedler2::ColorARGB::fromBGRA(&clrBuf[i * 4]), clr);
    }
    pal.copy(&clrBuf[0], clrBuf.size(), true);
    // BGRA buffer:
    BOOST_REQUIRE_EQUAL(libsiedler2::ColorARGB::fromBGRA(&clrBuf[libsiedler2::TRANSPARENT_INDEX * 4]), libsiedler2::TRANSPARENT_COLOR);
}

BOOST_AUTO_TEST_SUITE_END()
