// Copyright (c) 2016 - 2020 Settlers Freaks (sf-team at siedler25.org)
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

#include "test/config.h"

#define BOOST_TEST_MODULE libSiedler2_Test
#include "cmpFiles.h"
#include <boost/filesystem/operations.hpp>
#include <boost/nowide/fstream.hpp>
#include <boost/test/unit_test.hpp>

//#include <vld.h>

struct TestSetup
{
    TestSetup() { boost::filesystem::create_directories(libsiedler2::test::outputPath); }
    ~TestSetup() { boost::filesystem::remove_all(libsiedler2::test::outputPath); }
};

BOOST_GLOBAL_FIXTURE(TestSetup);

BOOST_AUTO_TEST_CASE(TestCmpFiles)
{
    const auto testFile1 = libsiedler2::test::outputPath / "cmpFiles1.txt";
    const auto testFile2 = libsiedler2::test::outputPath / "cmpFiles2.txt";
    const auto testFile3 = libsiedler2::test::outputPath / "cmpFiles3.txt";
    const auto testFile4 = libsiedler2::test::outputPath / "cmpFiles4.txt";
    {
        boost::nowide::ofstream f1(testFile1);
        boost::nowide::ofstream f2(testFile2);
        boost::nowide::ofstream f3(testFile3);
        boost::nowide::ofstream f4(testFile4);
        f1 << "Hello";
        f2 << "Hello\n";
        f3 << "12345";
        f4 << "Hello";
    }
    BOOST_TEST(!testFilesEqual(testFile1, testFile2)); // Wrong size
    BOOST_TEST(!testFilesEqual(testFile1, testFile3)); // Wrong content
    BOOST_TEST(testFilesEqual(testFile1, testFile4));  // ok
}