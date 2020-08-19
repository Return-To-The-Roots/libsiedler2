// Copyright (c) 2017 - 2020 Settlers Freaks (sf-team at siedler25.org)
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
#include "test/config.h"
#include "libsiedler2/Archiv.h"
#include "libsiedler2/libsiedler2.h"
#include <boost/filesystem.hpp>
#include <boost/test/unit_test.hpp>

namespace bfs = boost::filesystem;

BOOST_AUTO_TEST_SUITE(Maps)

BOOST_AUTO_TEST_CASE(ReadWriteSWD)
{
    const bfs::path mapPath = libsiedler2::test::inputPath / "map.SWD";
    const bfs::path mapOutPath = libsiedler2::test::outputPath / "outMap.SWD";
    BOOST_REQUIRE(bfs::exists(mapPath));
    libsiedler2::Archiv act;
    BOOST_REQUIRE_EQUAL(libsiedler2::Load(mapPath, act), 0);
    BOOST_REQUIRE_EQUAL(libsiedler2::Write(mapOutPath, act), 0);
    BOOST_REQUIRE(testFilesEqual(mapOutPath, mapPath));
}

BOOST_AUTO_TEST_CASE(ReadWriteWLD)
{
    const bfs::path mapPath = libsiedler2::test::inputPath / "map.wld";
    const bfs::path mapOutPath = libsiedler2::test::outputPath / "outMap.wld";
    BOOST_REQUIRE(bfs::exists(mapPath));
    libsiedler2::Archiv act;
    BOOST_REQUIRE_EQUAL(libsiedler2::Load(mapPath, act), 0);
    BOOST_REQUIRE_EQUAL(libsiedler2::Write(mapOutPath, act), 0);
    BOOST_REQUIRE(testFilesEqual(mapOutPath, mapPath));
}

BOOST_AUTO_TEST_CASE(ReadFaultyWLD)
{
    // Copy of map.wld but has a 20B title with size afterwards and extra word
    const bfs::path mapPath = libsiedler2::test::inputPath / "faultyMap.wld";
    const bfs::path mapOutPath = libsiedler2::test::outputPath / "faultyMap.wld";
    const bfs::path okMapPath = libsiedler2::test::inputPath / "map.wld";
    BOOST_REQUIRE(bfs::exists(mapPath));
    libsiedler2::Archiv act;
    BOOST_REQUIRE_EQUAL(libsiedler2::Load(mapPath, act), 0);
    BOOST_REQUIRE_EQUAL(libsiedler2::Write(mapOutPath, act), 0);
    BOOST_REQUIRE(testFilesEqual(mapOutPath, okMapPath));
}

BOOST_AUTO_TEST_SUITE_END()
