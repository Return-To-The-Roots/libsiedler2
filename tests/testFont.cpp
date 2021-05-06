// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "LoadPalette.h"
#include "cmpFiles.h"
#include "test/config.h"
#include "libsiedler2/Archiv.h"
#include "libsiedler2/ArchivItem_Font.h"
#include "libsiedler2/libsiedler2.h"
#include <boost/filesystem.hpp>
#include <boost/test/unit_test.hpp>

BOOST_FIXTURE_TEST_SUITE(Fonts, LoadPalette)

BOOST_AUTO_TEST_CASE(LoadWriteFont)
{
    const boost::filesystem::path inPath = libsiedler2::test::inputPath / "testFonts.LST";
    const boost::filesystem::path outPath = libsiedler2::test::outputPath / "outFonts.lst";
    libsiedler2::Archiv archiv;
    BOOST_REQUIRE_EQUAL(libsiedler2::Load(inPath, archiv, palette), 0);
    const auto* font = dynamic_cast<const libsiedler2::ArchivItem_Font*>(archiv[1]);
    BOOST_REQUIRE(font);
    BOOST_REQUIRE(!font->isUnicode);
    font = dynamic_cast<const libsiedler2::ArchivItem_Font*>(archiv[2]);
    BOOST_REQUIRE(font);
    BOOST_REQUIRE(font->isUnicode);
    BOOST_REQUIRE_EQUAL(libsiedler2::Write(outPath, archiv, palette), 0);
    BOOST_REQUIRE(testFilesEqual(outPath, inPath));
}

BOOST_AUTO_TEST_SUITE_END()
