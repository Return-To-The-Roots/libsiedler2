// Copyright (C) 2005 - 2026 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "LoadPalette.h"
#include "test/config.h"
#include "libsiedler2/Archiv.h"
#include "libsiedler2/ArchivItem_Bitmap_Raw.h"
#include "libsiedler2/ArchivItem_Text.h"
#include "libsiedler2/FileEntry.h"
#include "libsiedler2/PixelBufferBGRA.h"
#include "libsiedler2/libsiedler2.h"
#include <boost/filesystem.hpp>
#include <boost/nowide/fstream.hpp>
#include <boost/test/unit_test.hpp>
#include <array>
#include <tuple>
#include <vector>

namespace bfs = boost::filesystem;

BOOST_TEST_DONT_PRINT_LOG_VALUE(libsiedler2::BobType)

namespace {
struct LoadFolderFixture : LoadPalette
{
    bfs::path folderPath;

    LoadFolderFixture() : folderPath(libsiedler2::test::outputPath / "loadFolderTest")
    {
        bfs::create_directories(folderPath);
    }

    ~LoadFolderFixture() { bfs::remove_all(folderPath); }

    void writeFile(const std::string& name, const std::string& content = "")
    {
        boost::nowide::ofstream f(folderPath / name);
        f << content;
    }

    void writeBitmap(const std::string& name)
    {
        libsiedler2::ArchivItem_Bitmap_Raw bmp;
        libsiedler2::PixelBufferBGRA buffer(2, 2);
        bmp.create(buffer);
        libsiedler2::Archiv archiv;
        archiv.pushC(bmp);
        libsiedler2::Write(folderPath / name, archiv);
    }
};
} // namespace

BOOST_FIXTURE_TEST_SUITE(LoadFolder, LoadFolderFixture)

BOOST_AUTO_TEST_CASE(ReadFolderInfoParsesExtensions)
{
    using libsiedler2::BobType;
    const std::array<std::tuple<std::string, std::string, BobType>, 11> expectedTypes = {
      // clang-format off
      std::make_tuple("0.foo.bmp", "foo", BobType::Bitmap),
      std::make_tuple("1.bbm", "", BobType::Palette),
      std::make_tuple("2.bar.act", "bar", BobType::Palette),
      std::make_tuple("3.txt", "", BobType::Text),
      std::make_tuple("4.ger", "", BobType::Text),
      std::make_tuple("5.eng", "", BobType::Text),
      std::make_tuple("6.links", "", BobType::Text),
      std::make_tuple("7.empty", "", BobType::None),
      std::make_tuple("8.midi", "", BobType::Sound),
      std::make_tuple("9.xmi", "", BobType::Sound),
      std::make_tuple("a.wav", "a", BobType::Sound)
      // clang-format on
    };
    for(const auto& e : expectedTypes)
        writeFile(std::get<0>(e));

    std::vector<libsiedler2::FileEntry> entries = libsiedler2::ReadFolderInfo(folderPath);
    BOOST_TEST_REQUIRE(entries.size() == expectedTypes.size());

    for(unsigned i = 0; i < entries.size(); ++i)
    {
        const auto& entry = entries[i];
        const auto& [expFile, expName, expType] = expectedTypes[i];
        BOOST_TEST(entry.filePath.filename().string() == expFile);
        BOOST_TEST(entry.nr == ((expName == "a") ? -1 : i));
        BOOST_TEST(entry.bobtype == expType);
        BOOST_TEST(entry.name == expName);
    }
}

BOOST_AUTO_TEST_CASE(ReadFolderInfoParsesUnicodePrefix)
{
    writeFile("0xABCD.test.txt");
    writeFile("u+1234.test.txt");

    std::vector<libsiedler2::FileEntry> entries = libsiedler2::ReadFolderInfo(folderPath);
    BOOST_TEST_REQUIRE(entries.size() == 2u);
    BOOST_TEST(entries[0].nr == 0xABCD);
    BOOST_TEST(entries[1].nr == 0x1234);
}

BOOST_AUTO_TEST_CASE(ReadFolderInfoParsesNxNy)
{
    writeFile("5.nx10.ny20.bmp");

    std::vector<libsiedler2::FileEntry> entries = libsiedler2::ReadFolderInfo(folderPath);
    BOOST_TEST_REQUIRE(entries.size() == 1u);
    BOOST_TEST(entries[0].nx == 10);
    BOOST_TEST(entries[0].ny == 20);
}

BOOST_AUTO_TEST_CASE(LoadFolderEmptyReturnsNone)
{
    libsiedler2::Archiv archiv;
    std::vector<libsiedler2::FileEntry> empty;
    BOOST_TEST(libsiedler2::LoadFolder(std::move(empty), archiv, palette) == 0);
    BOOST_TEST(archiv.empty());
}

BOOST_AUTO_TEST_CASE(LoadFolderBitmapEntry)
{
    writeBitmap("0.test.bmp");

    libsiedler2::Archiv archiv;
    BOOST_TEST_REQUIRE(libsiedler2::LoadFolder(libsiedler2::ReadFolderInfo(folderPath), archiv, palette) == 0);
    BOOST_TEST_REQUIRE(archiv.size() == 1u);
    BOOST_TEST_REQUIRE(archiv[0]);
    BOOST_TEST(static_cast<bool>(archiv[0]->getBobType() == libsiedler2::BobType::Bitmap));
}

BOOST_AUTO_TEST_CASE(LoadFolderTextEntryWithNr)
{
    writeFile("5.hello.txt", "Hello");

    libsiedler2::Archiv archiv;
    BOOST_TEST_REQUIRE(libsiedler2::LoadFolder(libsiedler2::ReadFolderInfo(folderPath), archiv, palette) == 0);
    BOOST_TEST_REQUIRE(archiv.size() >= 6u);
    BOOST_TEST_REQUIRE(archiv[5]);
    BOOST_TEST(static_cast<bool>(archiv[5]->getBobType() == libsiedler2::BobType::Text));
}

BOOST_AUTO_TEST_CASE(LoadFolderEntryWithoutNrIsPushed)
{
    writeFile("hello.txt", "World");

    libsiedler2::Archiv archiv;
    BOOST_TEST_REQUIRE(libsiedler2::LoadFolder(libsiedler2::ReadFolderInfo(folderPath), archiv, palette) == 0);
    BOOST_TEST_REQUIRE(archiv.size() == 1u);
    BOOST_TEST_REQUIRE(archiv[0]);
    BOOST_TEST(archiv[0]->getName() == "hello");
}

BOOST_AUTO_TEST_CASE(LoadFolderUnsetEntriesAreIgnored)
{
    std::vector<libsiedler2::FileEntry> entries;
    libsiedler2::FileEntry e1(folderPath / "dummy1");
    e1.bobtype = libsiedler2::BobType::Unset;
    entries.push_back(e1);

    libsiedler2::Archiv archiv;
    BOOST_TEST(libsiedler2::LoadFolder(std::move(entries), archiv, palette) == 0);
    BOOST_TEST(archiv.empty());
}

BOOST_AUTO_TEST_CASE(FileEntryOperatorLess)
{
    libsiedler2::FileEntry a(folderPath / "a", "", 0, libsiedler2::BobType::Bitmap, 0, 0);
    libsiedler2::FileEntry b(folderPath / "b", "", 1, libsiedler2::BobType::Bitmap, 0, 0);
    BOOST_TEST(static_cast<bool>(a < b));
    BOOST_TEST(!static_cast<bool>(b < a));

    libsiedler2::FileEntry c(folderPath / "c", "", -1, libsiedler2::BobType::Bitmap, 0, 0);
    libsiedler2::FileEntry d(folderPath / "d", "", -1, libsiedler2::BobType::Palette, 0, 0);
    BOOST_TEST(static_cast<bool>(d < c));
    BOOST_TEST(!static_cast<bool>(c < d));
}

BOOST_AUTO_TEST_SUITE_END()
