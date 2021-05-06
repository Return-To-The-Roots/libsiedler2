// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "cmpFiles.h"
#include "test/config.h"
#include "libsiedler2/Archiv.h"
#include "libsiedler2/libsiedler2.h"
#include <boost/filesystem.hpp>
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(IniFiles)

BOOST_AUTO_TEST_CASE(ReadWriteIni)
{
    const boost::filesystem::path inPath = libsiedler2::test::inputPath / "test.ini";
    const boost::filesystem::path outPath = libsiedler2::test::outputPath / "outTest.ini";
    libsiedler2::Archiv act;
    BOOST_REQUIRE_EQUAL(libsiedler2::Load(inPath, act), 0);
    BOOST_REQUIRE_EQUAL(libsiedler2::Write(outPath, act), 0);
    BOOST_REQUIRE(testFilesEqual(outPath, inPath));
}

BOOST_AUTO_TEST_SUITE_END()
