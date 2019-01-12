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

#include "cmpFiles.h"
#include "config.h"
#include "libsiedler2/Archiv.h"
#include "libsiedler2/libsiedler2.h"
#include <boost/filesystem.hpp>
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(Maps)

BOOST_AUTO_TEST_CASE(ReadWriteSWD)
{
    std::string mapPath = "testFiles/map.SWD";
    std::string mapOutPath = testOutputPath + "/outMap.SWD";
    BOOST_REQUIRE(bfs::exists(mapPath));
    libsiedler2::Archiv act;
    BOOST_REQUIRE_EQUAL(libsiedler2::Load(mapPath, act), 0);
    BOOST_REQUIRE_EQUAL(libsiedler2::Write(mapOutPath, act), 0);
    BOOST_REQUIRE(testFilesEqual(mapOutPath, mapPath));
}

BOOST_AUTO_TEST_CASE(ReadWriteWLD)
{
    std::string mapPath = "testFiles/map.wld";
    std::string mapOutPath = testOutputPath + "/outMap.wld";
    BOOST_REQUIRE(bfs::exists(mapPath));
    libsiedler2::Archiv act;
    BOOST_REQUIRE_EQUAL(libsiedler2::Load(mapPath, act), 0);
    BOOST_REQUIRE_EQUAL(libsiedler2::Write(mapOutPath, act), 0);
    BOOST_REQUIRE(testFilesEqual(mapOutPath, mapPath));
}

BOOST_AUTO_TEST_CASE(ReadFaultyWLD)
{
    // Copy of map.wld but has a 20B title with size afterwards and extra word
    std::string mapPath = "testFiles/faultyMap.wld";
    std::string mapOutPath = testOutputPath + "/faultyMap.wld";
    std::string okMapPath = "testFiles/map.wld";
    BOOST_REQUIRE(bfs::exists(mapPath));
    libsiedler2::Archiv act;
    BOOST_REQUIRE_EQUAL(libsiedler2::Load(mapPath, act), 0);
    BOOST_REQUIRE_EQUAL(libsiedler2::Write(mapOutPath, act), 0);
    BOOST_REQUIRE(testFilesEqual(mapOutPath, okMapPath));
}

BOOST_AUTO_TEST_SUITE_END()
