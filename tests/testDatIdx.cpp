// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "LoadPalette.h"
#include "test/config.h"
#include "libsiedler2/Archiv.h"
#include "libsiedler2/ArchivItem_BitmapBase.h"
#include "libsiedler2/ArchivItem_Font.h"
#include "libsiedler2/ArchivItem_Palette.h"
#include "libsiedler2/libsiedler2.h"
#include <boost/filesystem.hpp>
#include <boost/test/unit_test.hpp>

BOOST_FIXTURE_TEST_SUITE(DatIdxFiles, LoadPalette)

BOOST_AUTO_TEST_CASE(LoadDatIdxFile)
{
    if(!libsiedler2::test::hasS2Data)
        return;
    // Both should work
    for(const auto* const file : {"EDITRES.IDX", "EDITRES.DAT"})
    {
        const boost::filesystem::path inPath = libsiedler2::test::s2Path / "DATA" / file;
        libsiedler2::Archiv archiv;
        BOOST_TEST_REQUIRE(libsiedler2::Load(inPath, archiv, palette) == 0);
        BOOST_TEST_REQUIRE(archiv.size() == 57u);
        BOOST_TEST_REQUIRE(dynamic_cast<libsiedler2::ArchivItem_Font*>(archiv[0]));
        BOOST_TEST_REQUIRE(dynamic_cast<libsiedler2::ArchivItem_Palette*>(archiv[1]));
        BOOST_TEST_REQUIRE(dynamic_cast<libsiedler2::ArchivItem_Font*>(archiv[2]));
        BOOST_TEST_REQUIRE(dynamic_cast<libsiedler2::ArchivItem_Font*>(archiv[3]));
        for(unsigned i = 4; i < archiv.size(); i++)
        {
            const auto* bmp = dynamic_cast<const libsiedler2::ArchivItem_BitmapBase*>(archiv[i]);
            BOOST_TEST_REQUIRE(bmp);
            BOOST_TEST(bmp->getWidth() > 0u);
            BOOST_TEST(bmp->getHeight() > 0u);
        }
    }
}

BOOST_AUTO_TEST_SUITE_END()
