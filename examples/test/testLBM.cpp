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
#include "libsiedler2/src/ArchivItem_BitmapBase.h"
#include <boost/filesystem.hpp>
#include <boost/test/unit_test.hpp>

BOOST_FIXTURE_TEST_SUITE(LbmFiles, LoadPalette)

BOOST_AUTO_TEST_CASE(LoadLbmFile)
{
    // Proprietary file. Copy S2 installation into the testFiles folder to test this
    std::string inPath = "testFiles/GFX/TEXTURES/TEX5.LBM";
    if(!bfs::exists(inPath))
        return;
    libsiedler2::ArchivInfo archiv;
    BOOST_REQUIRE_EQUAL(libsiedler2::Load(inPath, archiv, palette), 0);
    BOOST_REQUIRE_EQUAL(archiv.size(), 1u);
    const libsiedler2::ArchivItem_BitmapBase* bmp = dynamic_cast<libsiedler2::ArchivItem_BitmapBase*>(archiv[0]);
    BOOST_REQUIRE(bmp);
    BOOST_REQUIRE_EQUAL(bmp->getWidth(), 256u);
    BOOST_REQUIRE_EQUAL(bmp->getHeight(), 256u);
    BOOST_REQUIRE_EQUAL(bmp->getNx(), 0);
    BOOST_REQUIRE_EQUAL(bmp->getNy(), 0);
    // Hard coded expectation
    BOOST_REQUIRE_EQUAL(bmp->tex_getPixel(0, 0), 124u);
    BOOST_REQUIRE_EQUAL(bmp->tex_getPixel(123, 65), 77u);
    BOOST_REQUIRE_EQUAL(bmp->tex_getPixel(255, 255), 240u);
}

BOOST_AUTO_TEST_SUITE_END()