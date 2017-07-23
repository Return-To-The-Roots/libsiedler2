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

#include "config.h"
#include "cmpFiles.h"
#include "libsiedler2/src/ArchivInfo.h"
#include "libsiedler2/src/libsiedler2.h"
#include "libsiedler2/src/ArchivItem_Palette.h"
#include <boost/filesystem.hpp>
#include <boost/test/unit_test.hpp>
#include <fstream>

struct LoadPalette
{
    libsiedler2::ArchivInfo paletteArchiv;
    libsiedler2::ArchivItem_Palette* palette;
    LoadPalette()
    {
        BOOST_REQUIRE_EQUAL(libsiedler2::Load("pal5.act", paletteArchiv), 0);
        palette = dynamic_cast<libsiedler2::ArchivItem_Palette*>(paletteArchiv.get(0));
        BOOST_REQUIRE(palette);
    }
};

BOOST_FIXTURE_TEST_SUITE(Bitmaps, LoadPalette)

BOOST_AUTO_TEST_CASE(TestPlayerBitmap)
{
    std::string bmpPath = "testFiles/bmpPlayer.lst";
    std::string bmpOutPath = testOutputPath + "/bmp.lst";
    BOOST_REQUIRE(bfs::exists(bmpPath));
    libsiedler2::ArchivInfo bmp;
    BOOST_REQUIRE_EQUAL(libsiedler2::Load(bmpPath, bmp, palette), 0);
    BOOST_REQUIRE_EQUAL(libsiedler2::Write(bmpOutPath, bmp, palette), 0);
    BOOST_REQUIRE(testFilesEqual(bmpOutPath, bmpPath));
}

BOOST_AUTO_TEST_CASE(TestRawBitmap)
{
    std::string bmpPath = "testFiles/bmpRaw.lst";
    std::string bmpOutPath = testOutputPath + "/bmp.lst";
    BOOST_REQUIRE(bfs::exists(bmpPath));
    libsiedler2::ArchivInfo bmp;
    BOOST_REQUIRE_EQUAL(libsiedler2::Load(bmpPath, bmp, palette), 0);
    BOOST_REQUIRE_EQUAL(libsiedler2::Write(bmpOutPath, bmp, palette), 0);
    BOOST_REQUIRE(testFilesEqual(bmpOutPath, bmpPath));
}

BOOST_AUTO_TEST_CASE(TestShadowBitmap)
{
    std::string bmpPath = "testFiles/bmpShadow.lst";
    std::string bmpOutPath = testOutputPath + "/bmp.lst";
    BOOST_REQUIRE(bfs::exists(bmpPath));
    libsiedler2::ArchivInfo bmp;
    BOOST_REQUIRE_EQUAL(libsiedler2::Load(bmpPath, bmp, palette), 0);
    BOOST_REQUIRE_EQUAL(libsiedler2::Write(bmpOutPath, bmp, palette), 0);
    BOOST_REQUIRE(testFilesEqual(bmpOutPath, bmpPath));
}

BOOST_AUTO_TEST_CASE(TestRLEBitmap)
{
    std::string bmpPath = "testFiles/bmpRLE.lst";
    std::string bmpOutPath = testOutputPath + "/bmp.lst";
    BOOST_REQUIRE(bfs::exists(bmpPath));
    libsiedler2::ArchivInfo bmp;
    BOOST_REQUIRE_EQUAL(libsiedler2::Load(bmpPath, bmp, palette), 0);
    BOOST_REQUIRE_EQUAL(libsiedler2::Write(bmpOutPath, bmp, palette), 0);
    BOOST_REQUIRE(testFilesEqual(bmpOutPath, bmpPath));
}

BOOST_AUTO_TEST_SUITE_END()
