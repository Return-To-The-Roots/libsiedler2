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
#include "cmpFiles.h"
#include "libsiedler2/src/ArchivInfo.h"
#include "libsiedler2/src/ArchivItem_Palette.h"
#include "libsiedler2/src/libsiedler2.h"
#include <boost/filesystem.hpp>
#include <boost/test/unit_test.hpp>

namespace libsiedler2{
    std::ostream& operator<<(std::ostream& os, const Color& clr)
    {
        return os << "Color(" << unsigned(clr.r) << ", " << unsigned(clr.g) << ", " << unsigned(clr.b) << ")";
    }
}

BOOST_AUTO_TEST_SUITE(Palette)

BOOST_AUTO_TEST_CASE(ReadWritePalette)
{
    std::string palPath = "pal5.act";
    std::string palOutPath = testOutputPath + "/" + palPath;
    BOOST_REQUIRE(bfs::exists(palPath));
    libsiedler2::ArchivInfo act;
    BOOST_REQUIRE_EQUAL(libsiedler2::Load(palPath, act), 0);
    BOOST_REQUIRE_EQUAL(libsiedler2::Write(palOutPath, act), 0);
    BOOST_REQUIRE(testFilesEqual(palOutPath, palPath));
}

void createBBM(const std::string& palPath, const std::string& palOutPath)
{
    BOOST_REQUIRE(bfs::exists(palPath));
    libsiedler2::ArchivInfo act;
    BOOST_REQUIRE_EQUAL(libsiedler2::Load(palPath, act), 0);
    libsiedler2::ArchivItem_Palette* pal = dynamic_cast<libsiedler2::ArchivItem_Palette*>(act[0]);
    act.pushC(*pal);
    pal->set(10, libsiedler2::Color(11, 22, 33));
    BOOST_REQUIRE_EQUAL(libsiedler2::Write(palOutPath, act), 0);
}

BOOST_AUTO_TEST_CASE(ReadWriteBBM)
{
    //createBBM("pal5.act", "testFiles/pal.bbm");
    std::string palPath = "testFiles/pal.bbm";
    std::string palOutPath = testOutputPath + "/palOut.bbm";
    BOOST_REQUIRE(bfs::exists(palPath));
    libsiedler2::ArchivInfo act, act2;
    BOOST_REQUIRE_EQUAL(libsiedler2::Load(palPath, act), 0);
    BOOST_REQUIRE_EQUAL(libsiedler2::Load("pal5.act", act2), 0);
    libsiedler2::ArchivItem_Palette* pal = dynamic_cast<libsiedler2::ArchivItem_Palette*>(act[0]);
    BOOST_REQUIRE(pal);
    BOOST_REQUIRE_EQUAL(pal->get(10), libsiedler2::Color(11, 22, 33));
    BOOST_REQUIRE_EQUAL(libsiedler2::Write(palOutPath, act), 0);
    BOOST_REQUIRE(testFilesEqual(palOutPath, palPath));
}

BOOST_AUTO_TEST_SUITE_END()
