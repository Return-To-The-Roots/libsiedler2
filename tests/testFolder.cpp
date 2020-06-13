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

#include "LoadPalette.h"
#include "test/config.h"
#include "libsiedler2/Archiv.h"
#include "libsiedler2/ArchivItem_Bitmap_Raw.h"
#include "libsiedler2/ArchivItem_Text.h"
#include "libsiedler2/PixelBufferBGRA.h"
#include "libsiedler2/libsiedler2.h"
#include <boost/filesystem.hpp>
#include <boost/nowide/fstream.hpp>
#include <boost/test/unit_test.hpp>
#include <algorithm>
#include <array>

namespace bfs = boost::filesystem;

using namespace libsiedler2;

namespace libsiedler2 {
static std::ostream& boost_test_print_type(std::ostream& os, BobType bt)
{
    return os << static_cast<unsigned>(bt);
}
} // namespace libsiedler2

static void save(const ArchivItem_Bitmap_Raw& bmp, const boost::filesystem::path& path)
{
    libsiedler2::Archiv archive;
    archive.pushC(bmp);
    BOOST_TEST_REQUIRE(Write(path.string(), archive) == 0);
}

struct FolderFixture
{
    bfs::path lstPath;
    std::array<ArchivItem_Bitmap_Raw, 4> bmps;

    FolderFixture() : lstPath(bfs::unique_path(libsiedler2::test::outputPath + "/%%%%-%%%%.lst"))
    {
        bfs::create_directories(lstPath);
        PixelBufferBGRA buffer(5, 3);
        std::array<std::string, std::tuple_size<decltype(bmps)>::value> names = {"a", "1.player.nx5.ny7", "f", "3.rle.nx1.ny9"};

        for(unsigned i = 0; i < bmps.size(); i++)
        {
            BOOST_TEST(bmps[i].create(buffer) == 0);
            save(bmps[i], lstPath / (names[i] + ".bmp"));
        }
        {
            boost::nowide::ofstream f(lstPath / "5.cooltext.txt");
            f << "Hello World";
        }
    }
};

BOOST_FIXTURE_TEST_SUITE(Folder, FolderFixture)

BOOST_AUTO_TEST_CASE(ReadFolderInfoReturnsCorrectFiles)
{
    std::vector<FileEntry> info = ReadFolderInfo(lstPath.string());
    BOOST_TEST_REQUIRE(info.size() == 5u);
    std::sort(info.begin(), info.end());
    const std::vector<FileEntry> infoExpected = {
      FileEntry{(lstPath / "1.player.nx5.ny7.bmp").string(), "", 1, BobType::BitmapPlayer, 5, 7},
      FileEntry{(lstPath / "3.rle.nx1.ny9.bmp").string(), "", 3, BobType::BitmapRLE, 1, 9},
      FileEntry{(lstPath / "5.cooltext.txt").string(), "cooltext", 5, BobType::Text, 0, 0},
      FileEntry{(lstPath / "a.bmp").string(), "a", -1, BobType::Bitmap, 0, 0},
      FileEntry{(lstPath / "f.bmp").string(), "f", -1, BobType::Bitmap, 0, 0},
    };
    for(unsigned i = 0; i < infoExpected.size(); i++)
    {
        BOOST_TEST_INFO("Item " << i);
        BOOST_TEST(bfs::path(info[i].filePath).make_preferred() == bfs::path(infoExpected[i].filePath).make_preferred());
        BOOST_TEST(info[i].name == infoExpected[i].name);
        BOOST_TEST(info[i].nr == infoExpected[i].nr);
        BOOST_TEST(info[i].bobtype == infoExpected[i].bobtype);
        BOOST_TEST(info[i].nx == infoExpected[i].nx);
        BOOST_TEST(info[i].ny == infoExpected[i].ny);
    }
}

BOOST_AUTO_TEST_CASE(LoadFolderCreatesCorrectItems)
{
    LoadPalette loadPal;
    Archiv archive;
    BOOST_TEST_REQUIRE(LoadFolder(ReadFolderInfo(lstPath.string()), archive, loadPal.palette) == 0);
    BOOST_TEST_REQUIRE(archive.size() == 8u);
    BOOST_TEST_REQUIRE(!archive[0]);
    BOOST_TEST_REQUIRE(archive[1]);
    BOOST_TEST_REQUIRE(archive[1]->getBobType() == BobType::BitmapPlayer);
    BOOST_TEST_REQUIRE(archive[1]->getName() == "");
    BOOST_TEST_REQUIRE(!archive[2]);
    BOOST_TEST_REQUIRE(archive[3]);
    BOOST_TEST_REQUIRE(archive[3]->getBobType() == BobType::BitmapRLE);
    BOOST_TEST_REQUIRE(archive[3]->getName() == "");
    BOOST_TEST_REQUIRE(!archive[4]);
    BOOST_TEST_REQUIRE(archive[5]);
    BOOST_TEST_REQUIRE(archive[5]->getBobType() == BobType::Text);
    BOOST_TEST_REQUIRE(static_cast<ArchivItem_Text&>(*archive[5]).getText() == "Hello World");
    BOOST_TEST_REQUIRE(archive[5]->getName() == "cooltext");
    BOOST_TEST_REQUIRE(archive[6]);
    BOOST_TEST_REQUIRE(archive[6]->getBobType() == BobType::Bitmap);
    BOOST_TEST_REQUIRE(archive[6]->getName() == "a");
    BOOST_TEST_REQUIRE(archive[7]);
    BOOST_TEST_REQUIRE(archive[7]->getBobType() == BobType::Bitmap);
    BOOST_TEST_REQUIRE(archive[7]->getName() == "f");
}

BOOST_AUTO_TEST_SUITE_END()
