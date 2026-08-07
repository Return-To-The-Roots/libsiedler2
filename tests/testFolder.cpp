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
#include "s25util/tmpFile.h"
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

struct FolderFixture
{
    TmpFolder tmpFolder;
    bfs::path lstPath;

    FolderFixture() : tmpFolder(test::outputPath), lstPath(tmpFolder) {}

    void writeFile(const bfs::path& name, const std::string& content = "")
    {
        boost::nowide::ofstream f(lstPath / name);
        f << content;
    }

    void writeBitmap(const bfs::path& name)
    {
        ArchivItem_Bitmap_Raw bmp;
        PixelBufferBGRA buffer(2, 2);
        BOOST_TEST_REQUIRE(bmp.create(buffer) == 0);
        Archiv archiv;
        archiv.pushC(bmp);
        BOOST_TEST_REQUIRE(Write(lstPath / name, archiv) == 0);
    }
};

BOOST_FIXTURE_TEST_SUITE(Folder, FolderFixture)

BOOST_AUTO_TEST_CASE(FileEntryOperatorLess)
{
    FileEntry a(lstPath / "a", "", 0, BobType::Bitmap, 0, 0);
    FileEntry b(lstPath / "b", "", 1, BobType::Bitmap, 0, 0);
    BOOST_TEST(static_cast<bool>(a < b));
    BOOST_TEST(!static_cast<bool>(b < a));

    FileEntry c(lstPath / "c", "", -1, BobType::Bitmap, 0, 0);
    FileEntry d(lstPath / "d", "", -1, BobType::Palette, 0, 0);
    BOOST_TEST(static_cast<bool>(d < c));
    BOOST_TEST(!static_cast<bool>(c < d));
}

BOOST_AUTO_TEST_CASE(ReadFolderInfoParsesUnicodePrefix)
{
    writeFile("u+1234.test.txt");
    writeFile("0xABCD.test.txt");

    std::vector<FileEntry> entries = ReadFolderInfo(lstPath);
    BOOST_TEST_REQUIRE(entries.size() == 2u);
    std::sort(entries.begin(), entries.end());
    BOOST_TEST(entries[0].nr == 0x1234);
    BOOST_TEST(entries[1].nr == 0xABCD);
}

BOOST_AUTO_TEST_CASE(ReadFolderInfoReturnsCorrectFiles)
{
    const std::array infoExpected = {
      // clang-format off
      FileEntry{lstPath / "0.foo.bmp", "foo", 0, BobType::Bitmap, 0, 0},
      FileEntry{lstPath / "1.player.nx5.ny7.bmp", "", 1, BobType::BitmapPlayer, 5, 7},
      FileEntry{lstPath / "3.rle.nx1.ny9.bmp", "", 3, BobType::BitmapRLE, 1, 9},
      FileEntry{lstPath / "5.cooltext.txt", "cooltext", 5, BobType::Text, 0, 0},
      FileEntry{lstPath / "6.empty", "", 6, BobType::None, 0, 0},
      FileEntry{lstPath / "8.bbm", "", 8, BobType::Palette, 0, 0},
      FileEntry{lstPath / "9.bar.act", "bar", 9, BobType::Palette, 0, 0},
      FileEntry{lstPath / "12.txt", "", 12, BobType::Text, 0, 0},
      FileEntry{lstPath / "13.ger", "", 13, BobType::Text, 0, 0},
      FileEntry{lstPath / "14.eng", "", 14, BobType::Text, 0, 0},
      FileEntry{lstPath / "15.links", "", 15, BobType::Text, 0, 0},
      FileEntry{lstPath / "16.midi", "", 16, BobType::Sound, 0, 0},
      FileEntry{lstPath / "17.xmi", "", 17, BobType::Sound, 0, 0},
      FileEntry{lstPath / "18.wav", "", 18, BobType::Sound, 0, 0},
      FileEntry{lstPath / "a.bmp", "a", -1, BobType::Bitmap, 0, 0},
      FileEntry{lstPath / "f.bmp", "f", -1, BobType::Bitmap, 0, 0},
      // clang-format on
    };
    for(const auto& e : infoExpected)
        writeFile(e.filePath.filename());

    std::vector<FileEntry> entries = ReadFolderInfo(lstPath);
    BOOST_TEST_REQUIRE(entries.size() == infoExpected.size());
    std::sort(entries.begin(), entries.end());
    for(unsigned i = 0; i < infoExpected.size(); i++)
    {
        BOOST_TEST_INFO_SCOPE("Item " << i);
        const auto& entry = entries[i];
        BOOST_TEST(bfs::path(entry.filePath).make_preferred() == bfs::path(infoExpected[i].filePath).make_preferred());
        BOOST_TEST(entry.name == infoExpected[i].name);
        BOOST_TEST(entry.nr == infoExpected[i].nr);
        BOOST_TEST(entry.bobtype == infoExpected[i].bobtype);
        BOOST_TEST(entry.nx == infoExpected[i].nx);
        BOOST_TEST(entry.ny == infoExpected[i].ny);
    }
}

BOOST_AUTO_TEST_CASE(LoadFolderEmptyReturnsNone)
{
    Archiv archiv;
    std::vector<FileEntry> empty;
    BOOST_TEST(LoadFolder(empty, archiv) == 0);
    BOOST_TEST(archiv.empty());
}

BOOST_AUTO_TEST_CASE(LoadFolderTextEntryWithNr)
{
    writeFile("5.hello.txt", "Hello");

    Archiv archiv;
    BOOST_TEST_REQUIRE(LoadFolder(ReadFolderInfo(lstPath), archiv) == 0);
    BOOST_TEST_REQUIRE(archiv.size() == 6u);
    for(int i = 0; i < 5; ++i)
        BOOST_TEST(!archiv[i]);
    BOOST_TEST_REQUIRE(archiv[5]);
    BOOST_TEST(archiv[5]->getBobType() == BobType::Text);
}

BOOST_AUTO_TEST_CASE(LoadFolderEntryWithoutNrIsPushed)
{
    writeFile("hello.txt", "Hello");
    writeFile("world.txt", "World");

    Archiv archiv;
    BOOST_TEST_REQUIRE(LoadFolder(ReadFolderInfo(lstPath), archiv) == 0);
    BOOST_TEST_REQUIRE(archiv.size() == 2u);
    BOOST_TEST_REQUIRE(archiv[0]);
    BOOST_TEST(archiv[0]->getName() == "hello");
    BOOST_TEST_REQUIRE(archiv[1]);
    BOOST_TEST(archiv[1]->getName() == "world");
}

BOOST_AUTO_TEST_CASE(LoadFolderUnsetEntriesAreIgnored)
{
    std::vector<FileEntry> entries;
    FileEntry e1(lstPath / "dummy1");
    e1.bobtype = BobType::Unset;
    entries.push_back(e1);

    Archiv archiv;
    BOOST_TEST(LoadFolder(std::move(entries), archiv) == 0);
    BOOST_TEST(archiv.empty());
}

BOOST_AUTO_TEST_CASE(LoadFolderCreatesCorrectItems)
{
    std::array names = {"a", "1.player.nx5.ny7", "f", "3.rle.nx1.ny9"};
    for(const auto& name : names)
        writeBitmap(std::string(name) + ".bmp");
    writeFile("5.cooltext.txt", "Hello World");
    writeFile("6.empty");

    LoadPalette loadPal;
    Archiv archive;
    BOOST_TEST_REQUIRE(LoadFolder(ReadFolderInfo(lstPath), archive, loadPal.palette) == 0);
    BOOST_TEST_REQUIRE(archive.size() == 9u);
    BOOST_TEST(!archive[0]);
    BOOST_TEST_REQUIRE(archive[1]);
    BOOST_TEST_REQUIRE(archive[1]->getBobType() == BobType::BitmapPlayer);
    BOOST_TEST(archive[1]->getName() == "");
    const auto& bmp1 = dynamic_cast<const ArchivItem_BitmapBase&>(*archive[1]);
    BOOST_TEST(bmp1.getNx() == 5);
    BOOST_TEST(bmp1.getNy() == 7);
    BOOST_TEST(!archive[2]);
    BOOST_TEST_REQUIRE(archive[3]);
    BOOST_TEST_REQUIRE(archive[3]->getBobType() == BobType::BitmapRLE);
    BOOST_TEST(archive[3]->getName() == "");
    const auto& bmp3 = dynamic_cast<const ArchivItem_BitmapBase&>(*archive[3]);
    BOOST_TEST(bmp3.getNx() == 1);
    BOOST_TEST(bmp3.getNy() == 9);
    BOOST_TEST(!archive[4]);
    BOOST_TEST_REQUIRE(archive[5]);
    BOOST_TEST_REQUIRE(archive[5]->getBobType() == BobType::Text);
    BOOST_TEST(dynamic_cast<const ArchivItem_Text&>(*archive[5]).getText() == "Hello World");
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
