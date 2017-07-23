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
#include "libsiedler2/src/ArchivInfo.h"
#include "libsiedler2/src/libsiedler2.h"
#include <boost/filesystem.hpp>
#include <boost/test/unit_test.hpp>
#include <fstream>

BOOST_AUTO_TEST_SUITE(Palette)

BOOST_AUTO_TEST_CASE(ReadWritePalette)
{
    std::string palPath = "pal5.act";
    std::string palOutPath = testOutputPath + "/" + palPath;
    BOOST_REQUIRE(bfs::exists(palPath));
    libsiedler2::ArchivInfo act;
    BOOST_REQUIRE_EQUAL(libsiedler2::Load(palPath, act), 0);
    BOOST_REQUIRE_EQUAL(libsiedler2::Write(palOutPath, act), 0);
    BOOST_REQUIRE_EQUAL(bfs::file_size(palPath), bfs::file_size(palOutPath));
    
    std::ifstream ifs1(palPath);
    std::ifstream ifs2(palOutPath);

    std::istream_iterator<char> b1(ifs1), e1;
    std::istream_iterator<char> b2(ifs2), e2;

    BOOST_CHECK_EQUAL_COLLECTIONS(b1, e1, b2, e2);
}

BOOST_AUTO_TEST_SUITE_END()
