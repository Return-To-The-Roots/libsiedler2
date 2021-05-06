// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

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
