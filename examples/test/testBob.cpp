// Copyright (c) 2017 - 2017 Settlers Freaks (sf-team at siedler25.org)
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

#include "config.h"
#include "LoadPalette.h"
#include "libsiedler2/src/ArchivInfo.h"
#include "libsiedler2/src/libsiedler2.h"
#include "libsiedler2/src/ArchivItem_Bob.h"
#include "libsiedler2/src/ArchivItem_Bitmap_Player.h"
#include <boost/filesystem.hpp>
#include <boost/test/unit_test.hpp>

BOOST_FIXTURE_TEST_SUITE(BobFiles, LoadPalette)

BOOST_AUTO_TEST_CASE(LoadBobFile)
{
    // Proprietary file. Copy S2 installation into the testFiles folder to test this
    std::string inPath = "testFiles/DATA/BOBS/CARRIER.BOB";
    if(!bfs::exists(inPath))
        return;
    libsiedler2::ArchivInfo archiv;
    BOOST_REQUIRE_EQUAL(libsiedler2::Load(inPath, archiv, palette), 0);
    BOOST_REQUIRE_EQUAL(archiv.size(), 1u);
    const libsiedler2::ArchivItem_Bob& bob = *dynamic_cast<const libsiedler2::ArchivItem_Bob*>(archiv[0]);
    BOOST_REQUIRE_EQUAL(bob.getGoodImgCount(), 602u);
    BOOST_REQUIRE_EQUAL(bob.getItemCount(), 3264u);
    BOOST_REQUIRE_EQUAL(bob.size(), 698u);
    for(unsigned i = 0; i < bob.size(); i++)
    {
        const libsiedler2::ArchivItem_Bitmap_Player* bmp = dynamic_cast<const libsiedler2::ArchivItem_Bitmap_Player*>(bob[i]);
        BOOST_REQUIRE(bmp);
        BOOST_REQUIRE_EQUAL(bmp->getNx(), 16);
        BOOST_REQUIRE_EQUAL(bmp->getWidth(), 32u);
        BOOST_REQUIRE_GT(bmp->getHeight(), 0u);
    }
    // Some fixed tests
    const libsiedler2::ArchivItem_Bitmap_Player* bmp = dynamic_cast<const libsiedler2::ArchivItem_Bitmap_Player*>(bob[0]);
    BOOST_REQUIRE_EQUAL(bmp->getNy(), 12);
    BOOST_REQUIRE_EQUAL(bmp->getHeight(), 13u);
    bmp = dynamic_cast<const libsiedler2::ArchivItem_Bitmap_Player*>(bob[1]);
    BOOST_REQUIRE_EQUAL(bmp->getNy(), 13);
    BOOST_REQUIRE_EQUAL(bmp->getHeight(), 14u);
    bmp = dynamic_cast<const libsiedler2::ArchivItem_Bitmap_Player*>(bob[697]);
    BOOST_REQUIRE_EQUAL(bmp->getNy(), 18);
    BOOST_REQUIRE_EQUAL(bmp->getHeight(), 12u);
}

BOOST_AUTO_TEST_SUITE_END()
