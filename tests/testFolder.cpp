// Copyright (C) 2005 - 2026 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "ColorOutput.h"
#include "LoadPalette.h"
#include "test/config.h"
#include "libsiedler2/Archiv.h"
#include "libsiedler2/ArchivItem_Bitmap_Player.h"
#include "libsiedler2/ArchivItem_Bitmap_Raw.h"
#include "libsiedler2/ArchivItem_Bob.h"
#include "libsiedler2/ArchivItem_Font.h"
#include "libsiedler2/ArchivItem_PaletteAnimation.h"
#include "libsiedler2/ArchivItem_Text.h"
#include "libsiedler2/ColorRGB.h"
#include "libsiedler2/ErrorCodes.h"
#include "libsiedler2/PixelBufferBGRA.h"
#include "libsiedler2/libsiedler2.h"
#include "s25util/tmpFile.h"
#include <boost/filesystem.hpp>
#include <boost/nowide/fstream.hpp>
#include <boost/test/unit_test.hpp>
#include <algorithm>
#include <array>
#include <iomanip>
#include <random>
#include <sstream>

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
        Archiv archive;
        archive.pushC(bmp);
        BOOST_TEST_REQUIRE(Write(lstPath / name, archive) == 0);
    }
};

BOOST_AUTO_TEST_SUITE(LoadFiles)

BOOST_AUTO_TEST_CASE(FileEntryOperatorLess)
{
    struct Entry
    {
        const char* name;
        int nr;
        BobType bobtype;
    };
    const std::vector<Entry> expected = {
      // clang-format off
        // Sort by number first
        {"b",     1, BobType::Bitmap},
        // Palette comes first with equal nr
        {"c",     3, BobType::Palette},
        {"c",     3, BobType::Bitmap},
        // order by name, case-insensitive
        {"Alpha", 4, BobType::Bitmap},
        {"beta",  4, BobType::Bitmap},
        {"Carly", 4, BobType::Bitmap},
        // Shorter name first when same prefix
        {"a",     5, BobType::Bitmap},
        {"aa",    5, BobType::Bitmap},
        // Unnumbered last
        {"z",    -1, BobType::Palette},
        // Palette comes first
        {"a",    -1, BobType::Bitmap},
        // order by name
        {"zzz",  -1, BobType::Text},
      // clang-format on
    };

    bfs::path lstPath = "/tmp/foo.lst";
    std::vector<FileEntry> entries;
    for(const auto& e : expected)
        entries.emplace_back(lstPath / e.name, e.name, e.nr, e.bobtype, 0, 0);
    std::mt19937 rng(std::random_device{}());
    // Start with a random order
    std::shuffle(entries.begin(), entries.end(), rng);
    std::sort(entries.begin(), entries.end());

    for(unsigned i = 0; i < entries.size(); ++i)
    {
        BOOST_TEST_INFO_SCOPE("Item " << i);
        BOOST_TEST(entries[i].name == expected[i].name);
        BOOST_TEST(entries[i].nr == expected[i].nr);
        BOOST_TEST(entries[i].bobtype == expected[i].bobtype);
    }
}

BOOST_AUTO_TEST_CASE(InvalidNames)
{
    Archiv archive;
    BOOST_TEST(Load("", archive) == ErrorCode::INVALID_BUFFER);
    BOOST_TEST(archive.empty());
    BOOST_TEST(Write("", archive) == ErrorCode::INVALID_BUFFER);

    BOOST_TEST(Load("noext", archive) == ErrorCode::UNSUPPORTED_FORMAT);
    BOOST_TEST(archive.empty());
    BOOST_TEST(Write("noext", archive) == ErrorCode::UNSUPPORTED_FORMAT);

    // Unsupported extension
    BOOST_TEST(Load("file.weird", archive) == ErrorCode::UNSUPPORTED_FORMAT);
    BOOST_TEST(archive.empty());
    BOOST_TEST(Write("file.weird", archive) == ErrorCode::UNSUPPORTED_FORMAT);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE(Folder, FolderFixture)

BOOST_AUTO_TEST_CASE(ReadFolderInfoParsesUnicodePrefix)
{
    writeFile("u+1234.test.txt");
    writeFile("U+2211.test.txt");
    writeFile("0xABCD.test.txt");

    std::vector<FileEntry> entries = ReadFolderInfo(lstPath);
    BOOST_TEST_REQUIRE(entries.size() == 3u);
    std::sort(entries.begin(), entries.end());
    BOOST_TEST(entries[0].nr == 0x1234);
    BOOST_TEST(entries[1].nr == 0x2211);
    BOOST_TEST(entries[2].nr == 0xABCD);

    // Invalid hex
    writeFile("0xZZ.bad.bmp");
    BOOST_CHECK_THROW(ReadFolderInfo(lstPath), std::runtime_error);
    bfs::remove(lstPath / "0xZZ.bad.bmp");
    writeFile("u+GHI.txt");
    BOOST_CHECK_THROW(ReadFolderInfo(lstPath), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(ReadFolderInfoReturnsCorrectFiles)
{
    const std::array infoExpected = {
      // clang-format off
      FileEntry{lstPath / "0.foo.bmp", "foo", 0, BobType::Bitmap, 0, 0},
      FileEntry{lstPath / "1.player.nx5.ny7.bmp", "", 1, BobType::BitmapPlayer, 5, 7},
      FileEntry{lstPath / "3.rle.nx1.ny9.bmp", "", 3, BobType::BitmapRLE, 1, 9},
      FileEntry{lstPath / "4.shadow.bmp", "", 4, BobType::BitmapShadow, 0, 0},
      FileEntry{lstPath / "5.cooltext.txt", "cooltext", 5, BobType::Text, 0, 0},
      FileEntry{lstPath / "6.empty", "", 6, BobType::None, 0, 0},
      FileEntry{lstPath / "7.player.nx1.ny2.lst", "", 7, BobType::BitmapPlayer, 1, 2},
      FileEntry{lstPath / "8.bbm", "", 8, BobType::Palette, 0, 0},
      FileEntry{lstPath / "9.bar.act", "bar", 9, BobType::Palette, 0, 0},
      FileEntry{lstPath / "12.txt", "", 12, BobType::Text, 0, 0},
      FileEntry{lstPath / "13.ger", "", 13, BobType::Text, 0, 0},
      FileEntry{lstPath / "14.eng", "", 14, BobType::Text, 0, 0},
      FileEntry{lstPath / "15.links", "", 15, BobType::Text, 0, 0},
      FileEntry{lstPath / "16.midi", "", 16, BobType::Sound, 0, 0},
      FileEntry{lstPath / "17.xmi", "", 17, BobType::Sound, 0, 0},
      FileEntry{lstPath / "18.wav", "", 18, BobType::Sound, 0, 0},
      FileEntry{lstPath / "20.weird", "weird", 20, BobType::Unset, 0, 0},
      FileEntry{lstPath / "shadow.nx5.ny7.lst", "", -1, BobType::BitmapShadow, 5, 7},
      FileEntry{lstPath / "a.bmp", "a", -1, BobType::Bitmap, 0, 0},
      FileEntry{lstPath / "f.bmp", "f", -1, BobType::Bitmap, 0, 0},
      FileEntry{lstPath / "z.player.nx5.ny7.lst", "z", -1, BobType::BitmapPlayer, 5, 7},
      // clang-format on
    };
    for(const auto& e : infoExpected)
        writeFile(e.filePath.filename());

    std::vector<FileEntry> entries = ReadFolderInfo(lstPath);
    BOOST_TEST_REQUIRE(entries.size() == infoExpected.size());
    std::sort(entries.begin(), entries.end());
    for(unsigned i = 0; i < infoExpected.size(); i++)
    {
        const auto& entry = entries[i];
        const auto& expected = infoExpected[i];
        BOOST_TEST_INFO_SCOPE("Item " << i << ": " << entry.filePath);
        BOOST_TEST(bfs::path(entry.filePath).make_preferred() == bfs::path(expected.filePath).make_preferred());
        BOOST_TEST(entry.name == expected.name);
        BOOST_TEST(entry.nr == expected.nr);
        BOOST_TEST(entry.bobtype == expected.bobtype);
        BOOST_TEST(entry.nx == expected.nx);
        BOOST_TEST(entry.ny == expected.ny);
    }
}

BOOST_AUTO_TEST_CASE(LoadPlayerBitmapFromFolder)
{
    LoadPalette loadPal;

    // --- Case 1: Regular approach (requested player, source is a plain BMP) ---
    // A 2x2 BMP whose first two pixels match the palette's player colors
    // LoadFolder must convert it and reconstruct the player layer.
    {
        PixelBufferBGRA buffer(2, 2);
        buffer.set(0, 0, ColorBGRA(loadPal.palette->get(128)));
        buffer.set(1, 0, ColorBGRA(loadPal.palette->get(129)));
        ArchivItem_Bitmap_Raw raw;
        BOOST_TEST_REQUIRE(raw.create(buffer) == 0);
        Archiv rawArchiv;
        rawArchiv.pushC(raw);
        BOOST_TEST_REQUIRE(Write(lstPath / "3.player.nx1.ny2.bmp", rawArchiv) == 0);

        Archiv archive;
        BOOST_TEST_REQUIRE(LoadFolder(ReadFolderInfo(lstPath), archive, loadPal.palette) == 0);
        BOOST_TEST_REQUIRE(archive[3]);
        BOOST_TEST(archive[3]->getBobType() == BobType::BitmapPlayer);
        {
            const auto& playerBmp = dynamic_cast<const ArchivItem_Bitmap_Player&>(*archive[3]);
            BOOST_TEST(playerBmp.getPlayerColorIdx(0, 0) == 0);
            BOOST_TEST(playerBmp.getPlayerColorIdx(1, 0) == 1);
        }
    }

    // --- Case 2: Source is already a player bitmap ---
    // BMPs are always loaded as raw bitmaps so this is unlikely but possible.
    // For this by using a LST inside the folder typed via the rle/player/shadow keywords.
    // The LST must contain exactly 1 item or we'd get UNSUPPORTED_FORMAT.
    {
        PixelBufferPaletted palBuffer(2, 2, loadPal.palette->getTransparentIdx());
        palBuffer.set(0, 0, 129);
        palBuffer.set(1, 0, 131);
        ArchivItem_Bitmap_Player playerLst;
        BOOST_TEST_REQUIRE(playerLst.create(palBuffer, loadPal.palette) == 0);
        Archiv lstArchiv;
        lstArchiv.pushC(playerLst);
        // Contrast the 2 similar cases: Player bitmap (-> taken as-is) and RLE (-> conversion)
        BOOST_TEST_REQUIRE(Write(lstPath / "7.player.nx1.ny2.lst", lstArchiv, loadPal.palette) == 0);
        BOOST_TEST_REQUIRE(Write(lstPath / "8.rle.nx2.ny3.lst", lstArchiv, loadPal.palette) == 0);

        Archiv archiveLst;
        BOOST_TEST_REQUIRE(LoadFolder(ReadFolderInfo(lstPath), archiveLst, loadPal.palette) == 0);
        BOOST_TEST_REQUIRE(archiveLst[7]);
        BOOST_TEST(archiveLst[7]->getBobType() == BobType::BitmapPlayer);
        BOOST_TEST(archiveLst[7]->getName() == "");
        {
            const auto& playerBmp = dynamic_cast<const ArchivItem_Bitmap_Player&>(*archiveLst[7]);
            BOOST_TEST(playerBmp.getPlayerColorIdx(0, 0) == 1);
            BOOST_TEST(playerBmp.getPlayerColorIdx(1, 0) == 3);
        }
        BOOST_TEST_REQUIRE(archiveLst[8]);
        BOOST_TEST(archiveLst[8]->getBobType() == BobType::BitmapRLE);
        BOOST_TEST(archiveLst[8]->getName() == "");
    }

    // --- Case 3: Requested player bitmap but no palette given -> PALETTE_MISSING ---
    {
        Archiv archivErr;
        BOOST_TEST(LoadFolder(ReadFolderInfo(lstPath), archivErr, nullptr) == ErrorCode::PALETTE_MISSING);
    }
}

BOOST_AUTO_TEST_CASE(LoadTextEntryWithNr)
{
    writeFile("5.hello.txt", "Hello");

    Archiv archive;
    BOOST_TEST_REQUIRE(LoadFolder(ReadFolderInfo(lstPath), archive) == 0);
    BOOST_TEST_REQUIRE(archive.size() == 6u);
    for(int i = 0; i < 5; ++i)
        BOOST_TEST(!archive[i]);
    BOOST_TEST_REQUIRE(archive[5]);
    BOOST_TEST(archive[5]->getBobType() == BobType::Text);
}

BOOST_AUTO_TEST_CASE(EntryWithoutNrIsPushed)
{
    writeFile("hello.txt", "Hello");
    writeFile("world.txt", "World");

    Archiv archive;
    BOOST_TEST_REQUIRE(LoadFolder(ReadFolderInfo(lstPath), archive) == 0);
    BOOST_TEST_REQUIRE(archive.size() == 2u);
    BOOST_TEST_REQUIRE(archive[0]);
    BOOST_TEST(archive[0]->getName() == "hello");
    BOOST_TEST_REQUIRE(archive[1]);
    BOOST_TEST(archive[1]->getName() == "world");
}

BOOST_AUTO_TEST_CASE(UnsetEntriesAreIgnored)
{
    std::vector<FileEntry> entries;
    FileEntry e1(lstPath / "dummy1");
    e1.bobtype = BobType::Unset;
    entries.push_back(e1);

    Archiv archive;
    BOOST_TEST(LoadFolder(std::move(entries), archive) == 0);
    BOOST_TEST(archive.empty());
}

BOOST_AUTO_TEST_CASE(LoadFolderCreatesCorrectItems)
{
    {
        // Empty folder -> empty archive
        Archiv archive;
        std::vector<FileEntry> empty;
        BOOST_TEST(LoadFolder(empty, archive) == 0);
        BOOST_TEST(archive.empty());
        BOOST_TEST_REQUIRE(LoadFolder(ReadFolderInfo(lstPath), archive) == 0);
        BOOST_TEST(archive.empty());
    }

    std::array names = {"a", "1.player.nx5.ny7", "f", "3.rle.nx1.ny9", "4.shadow"};
    for(const auto& name : names)
        writeBitmap(std::string(name) + ".bmp");
    writeFile("6.cooltext.txt", "Hello World");
    writeFile("5.weird"); // Unsupported extensions are ignored
    writeFile("7.empty");

    LoadPalette loadPal;
    Archiv archive;
    BOOST_TEST_REQUIRE(LoadFolder(ReadFolderInfo(lstPath), archive, loadPal.palette) == 0);
    BOOST_TEST_REQUIRE(archive.size() == 10u);
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
    BOOST_TEST_REQUIRE(archive[4]);
    BOOST_TEST(archive[4]->getBobType() == BobType::BitmapShadow);
    BOOST_TEST(!archive[5]);
    BOOST_TEST_REQUIRE(archive[6]);
    BOOST_TEST_REQUIRE(archive[6]->getBobType() == BobType::Text);
    BOOST_TEST(archive[6]->getName() == "cooltext");
    BOOST_TEST(dynamic_cast<const ArchivItem_Text&>(*archive[6]).getText() == "Hello World");
    BOOST_TEST(!archive[7]);
    BOOST_TEST_REQUIRE(archive[8]);
    BOOST_TEST(archive[8]->getBobType() == BobType::Bitmap);
    BOOST_TEST(archive[8]->getName() == "a");
    BOOST_TEST_REQUIRE(archive[9]);
    BOOST_TEST(archive[9]->getBobType() == BobType::Bitmap);
    BOOST_TEST(archive[9]->getName() == "f");
}

BOOST_AUTO_TEST_CASE(LinksFileCanBeReadAsMapping)
{
    writeFile("7.links", "# links\n1\t3\n7\t5\n");
    Archiv archive;
    BOOST_TEST_REQUIRE(LoadFolder(ReadFolderInfo(lstPath), archive) == 0);
    BOOST_TEST_REQUIRE(archive[7]);
    BOOST_TEST(archive[7]->getBobType() == BobType::Text);

    // .links files are read as-is (no conversion) and then parsed like a bob's link list
    std::stringstream ss(dynamic_cast<const ArchivItem_Text&>(*archive[7]).getText());
    const auto links = ArchivItem_Bob::readLinks(ss);
    BOOST_TEST_REQUIRE(links.size() == 2u);
    BOOST_TEST(links.at(1) == 3);
    BOOST_TEST(links.at(7) == 5);
}

BOOST_AUTO_TEST_CASE(LoadTxtPalette)
{
    std::ostringstream content;
    content << "Bitmap palette V1\nTransparency: 1 Color: 123\n";
    for(unsigned i = 0; i < 256; ++i)
    {
        const ColorRGB clr(i, i - 1, i - 3);
        content << std::dec << i << "\t0x" << std::uppercase << std::hex << std::setfill('0') << std::setw(6)
                << clr.asValue() << "\n";
    }
    writeFile("2.palette.txt", content.str());

    Archiv archive;
    BOOST_TEST_REQUIRE(LoadFolder(ReadFolderInfo(lstPath), archive) == 0);
    BOOST_TEST_REQUIRE(archive[2]);
    BOOST_TEST(archive[2]->getBobType() == BobType::Palette);
    const auto& pal = dynamic_cast<const ArchivItem_Palette&>(*archive[2]);
    BOOST_TEST(pal.getTransparentIdx() == 123);
    BOOST_TEST(pal.get(4) == ColorRGB(4, 3, 1));
    BOOST_TEST(pal.get(255) == ColorRGB(255, 254, 252));
}

BOOST_AUTO_TEST_CASE(LoadFontDirectories)
{
    // Fonts are folders of glyph bitmaps; dx/dy encode the letter spacing
    bfs::create_directories(lstPath / "1.fon");
    writeBitmap(bfs::path("1.fon") / "65.bmp");
    writeBitmap(bfs::path("1.fon") / "66.bmp");
    // .fonx marks a unicode font; glyph file names may use hex code points
    bfs::create_directories(lstPath / "2.dx7.dy9.fonx");
    writeBitmap(bfs::path("2.dx7.dy9.fonx") / "U+E001.bmp");

    Archiv archive;
    BOOST_TEST_REQUIRE(LoadFolder(ReadFolderInfo(lstPath), archive) == 0);
    BOOST_TEST_REQUIRE(archive.size() == 3u);

    const auto& font = dynamic_cast<const ArchivItem_Font&>(*archive[1]);
    BOOST_TEST(!font.isUnicode);
    BOOST_TEST_REQUIRE(font.size() == 67u);
    BOOST_TEST_REQUIRE(font[65]);
    BOOST_TEST(font[65]->getBobType() == BobType::Bitmap);
    BOOST_TEST_REQUIRE(font[66]);

    const auto& fontx = dynamic_cast<const ArchivItem_Font&>(*archive[2]);
    BOOST_TEST(fontx.isUnicode);
    BOOST_TEST(fontx.getDx() == 7);
    BOOST_TEST(fontx.getDy() == 9);
    BOOST_TEST_REQUIRE(fontx.size() >= 0xE002u);
    BOOST_TEST_REQUIRE(fontx[0xE001]);
    BOOST_TEST(fontx[0xE001]->getBobType() == BobType::Bitmap);

    // A plain .fon file (not a folder) is not loadable
    bfs::remove_all(lstPath);
    bfs::create_directory(lstPath);
    writeFile("3.fon", "not a font");
    Archiv archive2;
    BOOST_TEST(LoadFolder(ReadFolderInfo(lstPath), archive2) == ErrorCode::UNSUPPORTED_FORMAT);
    BOOST_TEST(archive2.empty());
}

BOOST_AUTO_TEST_CASE(LoadPaletteAnims)
{
    // Animations are merged at their internal indices; the entry's own nr is not used
    writeFile("3.paletteanims.txt",
              R"(Palette animations V1
        0   10  active      forwards    1   3
        2   20  inactive    backwards   4   6
    )");

    Archiv archive;
    BOOST_TEST_REQUIRE(LoadFolder(ReadFolderInfo(lstPath), archive) == 0);
    BOOST_TEST_REQUIRE(archive.size() == 3u);
    BOOST_TEST(!archive[1]);

    const auto& anim0 = dynamic_cast<const ArchivItem_PaletteAnimation&>(*archive[0]);
    BOOST_TEST(anim0.rate == 10);
    BOOST_TEST(anim0.isActive);
    BOOST_TEST(anim0.moveUp);
    BOOST_TEST(anim0.firstClr == 1);
    BOOST_TEST(anim0.lastClr == 3);

    const auto& anim2 = dynamic_cast<const ArchivItem_PaletteAnimation&>(*archive[2]);
    BOOST_TEST(anim2.rate == 20);
    BOOST_TEST(!anim2.isActive);
    BOOST_TEST(!anim2.moveUp);
    BOOST_TEST(anim2.firstClr == 4);
    BOOST_TEST(anim2.lastClr == 6);

    writeBitmap("0.a.bmp");
    writeFile("3.paletteanims.txt",
              R"(Palette animations V1
        0   10  active  forwards    1   3
    )");
    // An animation index colliding with an already loaded item is an error
    Archiv archive2;
    BOOST_TEST(LoadFolder(ReadFolderInfo(lstPath), archive2) == ErrorCode::UNSUPPORTED_FORMAT);
}

BOOST_AUTO_TEST_CASE(NonBitmapSourceReturnsError)
{
    // "rle" keyword on a text file: the loaded text item is not a bitmap
    writeFile("5.rle.txt", "Hello");
    Archiv archive;
    BOOST_TEST(LoadFolder(ReadFolderInfo(lstPath), archive) == ErrorCode::UNSUPPORTED_FORMAT);
}

BOOST_AUTO_TEST_SUITE_END()
