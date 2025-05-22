// Copyright (C) 2005 - 2025 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

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
// LCOV_EXCL_START
static std::ostream& boost_test_print_type(std::ostream& os, BobType bt)
{
    return os << static_cast<unsigned>(bt);
}
// LCOV_EXCL_STOP
} // namespace libsiedler2

static void save(const ArchivItem_Bitmap_Raw& bmp, const boost::filesystem::path& path)
{
    libsiedler2::Archiv archive;
    archive.pushC(bmp);
    BOOST_TEST_REQUIRE(Write(path, archive) == 0);
}

struct FolderFixture
{
    bfs::path lstPath;
    std::array<ArchivItem_Bitmap_Raw, 4> bmps;

    FolderFixture() : lstPath(bfs::unique_path(libsiedler2::test::outputPath / "%%%%-%%%%.lst"))
    {
        bfs::create_directories(lstPath);
        PixelBufferBGRA buffer(5, 3);
        std::array<std::string, std::tuple_size_v<decltype(bmps)>> names = {"a", "1.player.nx5.ny7", "f",
                                                                            "3.rle.nx1.ny9"};

        for(unsigned i = 0; i < bmps.size(); i++)
        {
            BOOST_TEST(bmps[i].create(buffer) == 0);
            save(bmps[i], lstPath / (names[i] + ".bmp"));
        }
        {
            boost::nowide::ofstream f(lstPath / "5.cooltext.txt");
            f << "Hello World";
        }
        {
            boost::nowide::ofstream f(lstPath / "6.empty");
        }
    }
};

BOOST_FIXTURE_TEST_SUITE(Folder, FolderFixture)

BOOST_AUTO_TEST_CASE(ReadFolderInfoReturnsCorrectFiles)
{
    std::vector<FileEntry> info = ReadFolderInfo(lstPath);
    BOOST_TEST_REQUIRE(info.size() == 6u);
    std::sort(info.begin(), info.end());
    const std::vector<FileEntry> infoExpected = {
      FileEntry{(lstPath / "1.player.nx5.ny7.bmp"), "", 1, BobType::BitmapPlayer, 5, 7},
      FileEntry{(lstPath / "3.rle.nx1.ny9.bmp"), "", 3, BobType::BitmapRLE, 1, 9},
      FileEntry{(lstPath / "5.cooltext.txt"), "cooltext", 5, BobType::Text, 0, 0},
      FileEntry{(lstPath / "6.empty"), "", 6, BobType::None, 0, 0},
      FileEntry{(lstPath / "a.bmp"), "a", -1, BobType::Bitmap, 0, 0},
      FileEntry{(lstPath / "f.bmp"), "f", -1, BobType::Bitmap, 0, 0},
    };
    for(unsigned i = 0; i < infoExpected.size(); i++)
    {
        BOOST_TEST_INFO("Item " << i);
        BOOST_TEST(bfs::path(info[i].filePath).make_preferred()
                   == bfs::path(infoExpected[i].filePath).make_preferred());
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
    BOOST_TEST_REQUIRE(LoadFolder(ReadFolderInfo(lstPath), archive, loadPal.palette) == 0);
    BOOST_TEST_REQUIRE(archive.size() == 9u);
    BOOST_TEST(!archive[0]);
    BOOST_TEST_REQUIRE(archive[1]);
    BOOST_TEST_REQUIRE(archive[1]->getBobType() == BobType::BitmapPlayer);
    BOOST_TEST(archive[1]->getName() == "");
    const auto& bmp1 = static_cast<ArchivItem_BitmapBase&>(*archive[1]);
    BOOST_TEST(bmp1.getNx() == 5);
    BOOST_TEST(bmp1.getNy() == 7);
    BOOST_TEST(!archive[2]);
    BOOST_TEST_REQUIRE(archive[3]);
    BOOST_TEST_REQUIRE(archive[3]->getBobType() == BobType::BitmapRLE);
    BOOST_TEST(archive[3]->getName() == "");
    const auto& bmp3 = static_cast<ArchivItem_BitmapBase&>(*archive[3]);
    BOOST_TEST(bmp3.getNx() == 1);
    BOOST_TEST(bmp3.getNy() == 9);
    BOOST_TEST(!archive[4]);
    BOOST_TEST_REQUIRE(archive[5]);
    BOOST_TEST_REQUIRE(archive[5]->getBobType() == BobType::Text);
    BOOST_TEST(static_cast<ArchivItem_Text&>(*archive[5]).getText() == "Hello World");
    BOOST_TEST(archive[5]->getName() == "cooltext");
    BOOST_TEST(!archive[6]);
    BOOST_TEST_REQUIRE(archive[7]);
    BOOST_TEST(archive[7]->getBobType() == BobType::Bitmap);
    BOOST_TEST(archive[7]->getName() == "a");
    BOOST_TEST_REQUIRE(archive[8]);
    BOOST_TEST(archive[8]->getBobType() == BobType::Bitmap);
    BOOST_TEST(archive[8]->getName() == "f");
}

BOOST_AUTO_TEST_SUITE_END()
