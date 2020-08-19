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
#include "libsiedler2/ArchivItem_Bitmap.h"
#include "libsiedler2/ArchivItem_BitmapBase.h"
#include "libsiedler2/libsiedler2.h"
#include <boost/filesystem.hpp>
#include <boost/test/unit_test.hpp>

BOOST_FIXTURE_TEST_SUITE(LbmFiles, LoadPalette)

BOOST_AUTO_TEST_CASE(LoadLbmFile)
{
    if(!libsiedler2::test::hasS2Data)
        return;
    const boost::filesystem::path inPath = libsiedler2::test::s2Path / "GFX/TEXTURES/TEX5.LBM";
    libsiedler2::Archiv archiv;
    BOOST_REQUIRE_EQUAL(libsiedler2::Load(inPath, archiv, palette), 0);
    BOOST_REQUIRE_EQUAL(archiv.size(), 17u);
    const libsiedler2::ArchivItem_BitmapBase* bmp = dynamic_cast<libsiedler2::ArchivItem_BitmapBase*>(archiv[0]);
    BOOST_REQUIRE(bmp);
    BOOST_REQUIRE_EQUAL(bmp->getWidth(), 256u);
    BOOST_REQUIRE_EQUAL(bmp->getHeight(), 256u);
    BOOST_REQUIRE_EQUAL(bmp->getNx(), 0);
    BOOST_REQUIRE_EQUAL(bmp->getNy(), 0);
    // Hard coded expectation
    BOOST_REQUIRE_EQUAL(bmp->getPixelClrIdx(0, 0), 124u);
    BOOST_REQUIRE_EQUAL(bmp->getPixelClrIdx(123, 65), 77u);
    BOOST_REQUIRE_EQUAL(bmp->getPixelClrIdx(255, 255), 240u);
}

BOOST_AUTO_TEST_CASE(WriteReadLbmFile)
{
    const boost::filesystem::path inPathBmp = libsiedler2::test::inputPath / "pal.bmp";
    const boost::filesystem::path outPath = libsiedler2::test::outputPath / "out.lbm";
    // Load a paletted bmp and write it in lbm format
    libsiedler2::Archiv arBmp;
    BOOST_REQUIRE_EQUAL(libsiedler2::Load(inPathBmp, arBmp, palette), 0);
    BOOST_REQUIRE_EQUAL(libsiedler2::Write(outPath, arBmp, palette), 0);
    // Load lbm and check if bitmaps are equal
    libsiedler2::Archiv arLbm;
    BOOST_REQUIRE_EQUAL(libsiedler2::Load(outPath, arLbm, palette), 0);
    const auto* bmp = dynamic_cast<const libsiedler2::ArchivItem_Bitmap*>(arBmp[0]);
    const auto* bmpLbm = dynamic_cast<const libsiedler2::ArchivItem_Bitmap*>(arLbm[0]);
    BOOST_REQUIRE(bmp);
    BOOST_REQUIRE(bmpLbm);
    BOOST_REQUIRE_EQUAL(bmp->getWidth(), bmpLbm->getWidth());
    BOOST_REQUIRE_EQUAL(bmp->getHeight(), bmpLbm->getHeight());
    std::vector<libsiedler2::ColorRGB> bmpData(bmp->getPixelData().size()), bmpLbmData(bmp->getPixelData().size());
    std::transform(bmp->getPixelData().begin(), bmp->getPixelData().end(), bmpData.begin(),
                   [pal = *bmp->getPalette()](uint8_t i) { return pal[i]; });
    std::transform(bmpLbm->getPixelData().begin(), bmpLbm->getPixelData().end(), bmpLbmData.begin(),
                   [pal = *bmpLbm->getPalette()](uint8_t i) { return pal[i]; });
    BOOST_TEST_REQUIRE(bmpData == bmpLbmData, boost::test_tools::per_element());
}

BOOST_AUTO_TEST_SUITE_END()
