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
#include "libsiedler2/ArchivItem_Bitmap_Player.h"
#include "libsiedler2/ArchivItem_Bob.h"
#include "libsiedler2/libsiedler2.h"
#include "libsiedler2/loadMapping.h"
#include "s25util/StringConversion.h"
#include <boost/filesystem.hpp>
#include <boost/test/unit_test.hpp>

BOOST_FIXTURE_TEST_SUITE(BobFiles, LoadPalette)

BOOST_AUTO_TEST_CASE(LoadBobFile)
{
    if(!libsiedler2::test::hasS2Data)
        return;
    const boost::filesystem::path inPath = libsiedler2::test::s2Path / "DATA/BOBS/CARRIER.BOB";
    libsiedler2::Archiv archiv;
    BOOST_TEST_REQUIRE(libsiedler2::Load(inPath, archiv, palette) == 0);
    BOOST_TEST_REQUIRE(archiv.size() == 1u);
    BOOST_TEST_REQUIRE(dynamic_cast<const libsiedler2::ArchivItem_Bob*>(archiv[0]));
    libsiedler2::ArchivItem_Bob& bob = *static_cast<libsiedler2::ArchivItem_Bob*>(archiv[0]);
    BOOST_TEST(bob.getNumOverlayImgs() == 602u);
    BOOST_TEST(bob.getNumLinks() == 3264u);
    BOOST_TEST(bob.size() == 698u);
    for(unsigned i = 0; i < bob.size(); i++)
    {
        const auto* bmp = dynamic_cast<const libsiedler2::ArchivItem_Bitmap_Player*>(bob[i]);
        BOOST_TEST_REQUIRE(bmp);
        BOOST_TEST(bmp->getNx() == 16);
        BOOST_TEST(bmp->getWidth() == 32u);
        BOOST_TEST(bmp->getHeight() > 0u);
    }
    // Some fixed tests
    const auto* bmp = dynamic_cast<const libsiedler2::ArchivItem_Bitmap_Player*>(bob[0]);
    BOOST_TEST(bmp->getNy() == 12);
    BOOST_TEST(bmp->getHeight() == 13u);
    bmp = dynamic_cast<const libsiedler2::ArchivItem_Bitmap_Player*>(bob[1]);
    BOOST_TEST(bmp->getNy() == 13);
    BOOST_TEST(bmp->getHeight() == 14u);
    bmp = dynamic_cast<const libsiedler2::ArchivItem_Bitmap_Player*>(bob[697]);
    BOOST_TEST(bmp->getNy() == 18);
    BOOST_TEST(bmp->getHeight() == 12u);

    // Bodies are an array: array [fat][direction][animStep]: [2][6][8]
    using libsiedler2::ImgDir;
    BOOST_TEST(bob.getBody(true, ImgDir::W, 5));
    BOOST_TEST(bob.getBody(true, ImgDir::W, 5) == bob[1 * 48 + 3 * 8 + 5]);
    BOOST_TEST(bob.getBody(false, ImgDir::NW, 3));
    BOOST_TEST(bob.getBody(false, ImgDir::NW, 3) == bob[0 * 48 + 4 * 8 + 3]);
    // Overlay links are an array: [overlay][animStep][fat][direction]: [][8][2][6]
    BOOST_TEST(bob.getOverlay(21, true, ImgDir::W, 5));
    BOOST_TEST(bob.getOverlay(21, true, ImgDir::W, 5) == bob[bob.getOverlayIdx(21 * 96 + 5 * 12 + 3 + 1 * 6)]);
    BOOST_TEST(bob.getOverlay(13, false, ImgDir::NW, 7));
    BOOST_TEST(bob.getOverlay(13, false, ImgDir::NW, 7) == bob[bob.getOverlayIdx(13 * 96 + 7 * 12 + 4 + 0 * 6)]);
}

BOOST_AUTO_TEST_CASE(WriteReadLinks)
{
    if(!libsiedler2::test::hasS2Data)
        return;
    const boost::filesystem::path inPath = libsiedler2::test::s2Path / "DATA/BOBS/CARRIER.BOB";
    libsiedler2::Archiv archiv;
    BOOST_TEST_REQUIRE(libsiedler2::Load(inPath, archiv, palette) == 0);
    const auto* bob = dynamic_cast<const libsiedler2::ArchivItem_Bob*>(archiv[0]);
    BOOST_TEST_REQUIRE(bob);
    std::stringstream s;
    bob->writeLinks(s);
    s.seekg(0);
    const auto links = libsiedler2::ArchivItem_Bob::readLinks(s);
    for(unsigned i = 0; i < bob->getNumLinks(); i++)
    {
        const auto it = links.find(i);
        BOOST_TEST_REQUIRE((it != links.end()));
        BOOST_TEST_REQUIRE(it->second == bob->getOverlayIdx(i));
    }
}

BOOST_AUTO_TEST_SUITE_END()
