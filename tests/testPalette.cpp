// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "ColorOutput.h"
#include "cmpFiles.h"
#include "test/config.h"
#include "libsiedler2/Archiv.h"
#include "libsiedler2/ArchivItem_Palette.h"
#include "libsiedler2/ArchivItem_PaletteAnimation.h"
#include "libsiedler2/ColorBGRA.h"
#include "libsiedler2/ColorRGB.h"
#include "libsiedler2/libsiedler2.h"
#include <boost/filesystem.hpp>
#include <boost/test/unit_test.hpp>

namespace bfs = boost::filesystem;

BOOST_AUTO_TEST_SUITE(Palette)

BOOST_AUTO_TEST_CASE(ReadWritePalette)
{
    const bfs::path palPath = libsiedler2::test::inputPath / "pal5.act";
    const bfs::path palOutPath = libsiedler2::test::outputPath / "pal5.act";
    BOOST_REQUIRE(bfs::exists(palPath));
    libsiedler2::Archiv act;
    BOOST_REQUIRE_EQUAL(libsiedler2::Load(palPath, act), 0);
    BOOST_REQUIRE_EQUAL(libsiedler2::Write(palOutPath, act), 0);
    BOOST_REQUIRE(testFilesEqual(palOutPath, palPath));
}

// LCOV_EXCL_START
static void createBBM(const bfs::path& palPath, const bfs::path& palOutPath)
{
    BOOST_REQUIRE(bfs::exists(palPath));
    libsiedler2::Archiv act;
    BOOST_REQUIRE_EQUAL(libsiedler2::Load(palPath, act), 0);
    auto* pal = dynamic_cast<libsiedler2::ArchivItem_Palette*>(act[0]);
    act.pushC(*pal);
    pal->set(10, libsiedler2::ColorRGB(11, 22, 33));
    BOOST_REQUIRE_EQUAL(libsiedler2::Write(palOutPath, act), 0);
}
// LCOV_EXCL_STOP

BOOST_AUTO_TEST_CASE(ReadWriteBBM)
{
    if((false))
        createBBM("pal5.act", libsiedler2::test::inputPath / "pal.bbm");
    const bfs::path palPath = libsiedler2::test::inputPath / "pal.bbm";
    const bfs::path palOutPath = libsiedler2::test::outputPath / "palOut.bbm";
    BOOST_REQUIRE(bfs::exists(palPath));
    libsiedler2::Archiv act, act2;
    BOOST_REQUIRE_EQUAL(libsiedler2::Load(palPath, act), 0);
    BOOST_REQUIRE_EQUAL(libsiedler2::Load(libsiedler2::test::inputPath / "pal5.act", act2), 0);
    auto* pal = dynamic_cast<libsiedler2::ArchivItem_Palette*>(act[0]);
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
    pal.copyToBGRA(&clrBuf[0], clrBuf.size());
    for(unsigned i = 0; i < 256; i++)
    {
        // BGRA buffer:
        BOOST_REQUIRE_EQUAL(libsiedler2::ColorBGRA::fromBGRA(&clrBuf[i * 4]), pal[i]);
    }
}

BOOST_AUTO_TEST_CASE(ReadWritePalAnim)
{
    const bfs::path outPath = libsiedler2::test::outputPath / "paletteAnims.txt";

    libsiedler2::Archiv archiv;
    libsiedler2::ArchivItem_PaletteAnimation anim;
    anim.isActive = true;
    anim.moveUp = true;
    anim.rate = 1337;
    anim.firstClr = 2;
    anim.lastClr = 5;
    archiv.pushC(anim);
    archiv.push(nullptr);
    anim.isActive = false;
    anim.rate = 42;
    anim.firstClr = 3;
    anim.lastClr = 6;
    archiv.pushC(anim);
    anim.moveUp = false;
    archiv.pushC(anim);

    BOOST_REQUIRE_EQUAL(libsiedler2::Write(outPath, archiv), 0);
    libsiedler2::Archiv archiv2;
    BOOST_REQUIRE_EQUAL(libsiedler2::Load(outPath, archiv2), 0);
    BOOST_REQUIRE_EQUAL(archiv.size(), archiv2.size());
    BOOST_REQUIRE(!archiv2[1]);
    const libsiedler2::ArchivItem_PaletteAnimation* anim2 =
      dynamic_cast<libsiedler2::ArchivItem_PaletteAnimation*>(archiv2[0]);
    BOOST_REQUIRE(anim2);
    BOOST_REQUIRE_EQUAL(anim2->isActive, true);
    BOOST_REQUIRE_EQUAL(anim2->moveUp, true);
    BOOST_REQUIRE_EQUAL(anim2->rate, 1337);
    BOOST_REQUIRE_EQUAL(anim2->firstClr, 2);
    BOOST_REQUIRE_EQUAL(anim2->lastClr, 5);
    anim2 = dynamic_cast<libsiedler2::ArchivItem_PaletteAnimation*>(archiv2[2]);
    BOOST_REQUIRE(anim2);
    BOOST_REQUIRE_EQUAL(anim2->isActive, false);
    BOOST_REQUIRE_EQUAL(anim2->moveUp, true);
    BOOST_REQUIRE_EQUAL(anim2->rate, 42);
    BOOST_REQUIRE_EQUAL(anim2->firstClr, 3);
    BOOST_REQUIRE_EQUAL(anim2->lastClr, 6);
    anim2 = dynamic_cast<libsiedler2::ArchivItem_PaletteAnimation*>(archiv2[3]);
    BOOST_REQUIRE(anim2);
    BOOST_REQUIRE_EQUAL(anim2->isActive, false);
    BOOST_REQUIRE_EQUAL(anim2->moveUp, false);
    BOOST_REQUIRE_EQUAL(anim2->rate, 42);
    BOOST_REQUIRE_EQUAL(anim2->firstClr, 3);
    BOOST_REQUIRE_EQUAL(anim2->lastClr, 6);
}

BOOST_AUTO_TEST_SUITE_END()
