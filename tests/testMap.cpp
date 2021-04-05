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
#include "libsiedler2/ArchivItem_Map.h"
#include "libsiedler2/ArchivItem_Map_Header.h"
#include "libsiedler2/libsiedler2.h"
#include "s25util/tmpFile.h"
#include <boost/filesystem.hpp>
#include <boost/test/unit_test.hpp>

namespace bfs = boost::filesystem;

BOOST_AUTO_TEST_SUITE(Maps)

BOOST_AUTO_TEST_CASE(ReadWriteSWD)
{
    const bfs::path mapPath = libsiedler2::test::inputPath / "map.SWD";
    const bfs::path mapOutPath = libsiedler2::test::outputPath / "outMap.SWD";
    BOOST_TEST_REQUIRE(bfs::exists(mapPath));
    libsiedler2::Archiv act;
    BOOST_TEST_REQUIRE(libsiedler2::Load(mapPath, act) == 0);
    BOOST_TEST_REQUIRE(libsiedler2::Write(mapOutPath, act) == 0);
    BOOST_TEST(testFilesEqual(mapOutPath, mapPath));
}

BOOST_AUTO_TEST_CASE(ReadWriteWLD)
{
    const bfs::path mapPath = libsiedler2::test::inputPath / "map.wld";
    const bfs::path mapOutPath = libsiedler2::test::outputPath / "outMap.wld";
    BOOST_TEST_REQUIRE(bfs::exists(mapPath));
    libsiedler2::Archiv act;
    BOOST_TEST_REQUIRE(libsiedler2::Load(mapPath, act) == 0);
    BOOST_TEST_REQUIRE(libsiedler2::Write(mapOutPath, act) == 0);
    BOOST_TEST(testFilesEqual(mapOutPath, mapPath));
}

BOOST_AUTO_TEST_CASE(ReadFaultyWLD)
{
    // Copy of map.wld but has a 20B title with size afterwards and extra word
    const bfs::path mapPath = libsiedler2::test::inputPath / "faultyMap.wld";
    const bfs::path mapOutPath = libsiedler2::test::outputPath / "faultyMap.wld";
    const bfs::path okMapPath = libsiedler2::test::inputPath / "map.wld";
    BOOST_TEST_REQUIRE(bfs::exists(mapPath));
    libsiedler2::Archiv act;
    BOOST_TEST_REQUIRE(libsiedler2::Load(mapPath, act) == 0);
    BOOST_TEST_REQUIRE(libsiedler2::Write(mapOutPath, act) == 0);
    BOOST_TEST(testFilesEqual(mapOutPath, okMapPath));
}

BOOST_AUTO_TEST_CASE(CreateAndModifyMap)
{
    libsiedler2::ArchivItem_Map map;
    libsiedler2::ArchivItem_Map_Header header;
    header.setName("TestName");
    header.setAuthor("FooAuthor");
    header.setWidth(32);
    header.setHeight(42);
    header.setGfxSet(3);
    header.setNumPlayers(4);
    header.setPlayerHQ(0, 1, 2);
    header.setPlayerHQ(1, 6, 8);
    header.setPlayerHQ(2, 13, 17);
    header.setPlayerHQ(3, 23, 47);
    map.init(std::make_unique<libsiedler2::ArchivItem_Map_Header>(header));
    using libsiedler2::MapLayer;
    for(unsigned i = 0; i <= static_cast<unsigned>(MapLayer::Lakes); ++i)
        BOOST_TEST_REQUIRE(map.hasLayer(static_cast<MapLayer>(i)));
    map.setMapDataAt(MapLayer::Altitude, 10, 5);
    map.setMapDataAt(MapLayer::Altitude, 13, 17, 42);
    map.setMapDataAt(MapLayer::Resources, 11, 7);
    map.setMapDataAt(MapLayer::Resources, 19, 23, 24);

    TmpFile tmpMap(".wld");
    BOOST_TEST_REQUIRE(map.write(tmpMap.getStream()) == 0);
    tmpMap.close();
    libsiedler2::Archiv act;
    BOOST_TEST_REQUIRE(libsiedler2::Load(tmpMap.filePath, act) == 0);
    const auto* newMap = dynamic_cast<const libsiedler2::ArchivItem_Map*>(act[0]);
    BOOST_TEST_REQUIRE(newMap);
    BOOST_TEST(newMap->getMapDataAt(MapLayer::Altitude, 10) == 5);
    BOOST_TEST(newMap->getMapDataAt(MapLayer::Altitude, 13, 17) == 42);
    BOOST_TEST(newMap->getMapDataAt(MapLayer::Resources, 11) == 7);
    BOOST_TEST(newMap->getMapDataAt(MapLayer::Resources, 19, 23) == 24);

    // Also test cloning
    auto clonedMapItem = clone(act[0]);
    BOOST_TEST_REQUIRE(!!clonedMapItem);
    const auto* clonedMap = dynamic_cast<const libsiedler2::ArchivItem_Map*>(clonedMapItem.get());
    BOOST_TEST_REQUIRE(clonedMap);
    TmpFile tmpMap2(".wld");
    BOOST_TEST_REQUIRE(clonedMap->write(tmpMap2.getStream()) == 0);
    tmpMap2.close();
    BOOST_TEST(testFilesEqual(tmpMap.filePath, tmpMap2.filePath));
}

BOOST_AUTO_TEST_SUITE_END()
