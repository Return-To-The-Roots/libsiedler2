// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "cmpFiles.h"
#include "test/config.h"
#include "libsiedler2/Archiv.h"
#include "libsiedler2/ArchivItem_Ini.h"
#include "libsiedler2/libsiedler2.h"
#include <boost/filesystem.hpp>
#include <boost/nowide/fstream.hpp>
#include <boost/test/unit_test.hpp>
#include <stdexcept>

BOOST_AUTO_TEST_SUITE(IniFiles)

BOOST_AUTO_TEST_CASE(ReadWriteIni)
{
    const boost::filesystem::path inPath = libsiedler2::test::inputPath / "test.ini";
    const boost::filesystem::path outPath = libsiedler2::test::outputPath / "outTest.ini";
    libsiedler2::Archiv act;
    BOOST_TEST_REQUIRE(libsiedler2::Load(inPath, act) == 0);
    BOOST_TEST_REQUIRE(libsiedler2::Write(outPath, act) == 0);
    BOOST_TEST(testFilesEqual(outPath, inPath));

    {
        boost::nowide::ofstream f(outPath);
        f << "[Section]\n";
        f << "duplicate=1\n";
        f << "duplicate=2\n";
        f << "other=3\n";
    }
    BOOST_TEST_REQUIRE(libsiedler2::Load(outPath, act) == 0);
    const auto* ini = dynamic_cast<const libsiedler2::ArchivItem_Ini*>(act.find("Section"));
    BOOST_TEST_REQUIRE(ini);
    BOOST_TEST(ini->getIntValue("duplicate") == 2);
    BOOST_TEST(ini->getIntValue("other") == 3);
}

BOOST_AUTO_TEST_CASE(GetSetValues)
{
    libsiedler2::ArchivItem_Ini ini;
    BOOST_CHECK_THROW(ini.getValue("string"), std::runtime_error);
    BOOST_CHECK_THROW(ini.getBoolValue("bool"), std::runtime_error);
    BOOST_CHECK_THROW(ini.getIntValue("int"), std::runtime_error);
    ini.setValue("string", "value");
    BOOST_TEST(ini.getValue("string") == "value");
    ini.setValue("string", "value2");
    BOOST_TEST(ini.getValue("string") == "value2");
    ini.setValue("bool", true);
    BOOST_TEST(ini.getBoolValue("bool"));
    ini.setValue("bool", false);
    BOOST_TEST(!ini.getBoolValue("bool"));
    ini.setValue("int", "42");
    BOOST_TEST(ini.getIntValue("int") == 42);
    ini.setValue("int", "1337");
    BOOST_TEST(ini.getIntValue("int") == 1337);
}

BOOST_AUTO_TEST_CASE(DefaultValues)
{
    const boost::filesystem::path inPath = libsiedler2::test::inputPath / "test.ini";
    const boost::filesystem::path outPath = libsiedler2::test::outputPath / "outTest.ini";
    libsiedler2::Archiv act;
    BOOST_TEST_REQUIRE(libsiedler2::Load(inPath, act) == 0);
    auto* ini = dynamic_cast<libsiedler2::ArchivItem_Ini*>(act.find("Section0"));
    BOOST_TEST_REQUIRE(ini);
    BOOST_TEST(ini->getValue("NonExisting", "Default") == "Default");
    BOOST_TEST(ini->getValue("key0", "Default") == "Value1");
    BOOST_TEST(ini->getValue("emptyValue", "Default") == "");
    BOOST_TEST(ini->getValue("NonExisting", 42) == 42);
    BOOST_TEST(ini->getValue("number2", 42) == 1337);
    BOOST_TEST(ini->getValue("NonExisting", true));
    BOOST_TEST(!ini->getValue("NonExisting", false));
    BOOST_TEST(ini->getValue("number2", false));

    ini->setValue("bool", true);
    BOOST_TEST(ini->getValue("bool", false));
    ini->setValue("bool", false);
    BOOST_TEST(!ini->getValue("bool", true));
}

BOOST_AUTO_TEST_SUITE_END()
