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
#include "libsiedler2/src/libsiedler2.h"
#include <boost/filesystem.hpp>
#include <boost/test/unit_test.hpp>
#include <fstream>
#include <iostream>

BOOST_AUTO_TEST_SUITE(IniFiles)

BOOST_AUTO_TEST_CASE(ReadWriteIni)
{
    std::string inPath = "testFiles/test.ini";
    std::string outPath = testOutputPath + "/outTest.ini";
    BOOST_REQUIRE(bfs::exists(inPath));
    libsiedler2::ArchivInfo act;
    BOOST_REQUIRE_EQUAL(libsiedler2::Load(inPath, act), 0);
    BOOST_REQUIRE_EQUAL(libsiedler2::Write(outPath, act), 0);
    std::ifstream fs(outPath, std::ios::binary);
    fs.unsetf(std::ios::skipws);
    std::copy(std::istream_iterator<char>(fs), std::istream_iterator<char>(), std::ostream_iterator<char>(std::cout));
    BOOST_REQUIRE(testFilesEqual(outPath, inPath));
}

BOOST_AUTO_TEST_SUITE_END()
