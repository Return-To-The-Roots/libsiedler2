// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "test/config.h"
#include "libsiedler2/Archiv.h"
#include "libsiedler2/ArchivItem_Text.h"
#include "libsiedler2/libsiedler2.h"
#include <boost/filesystem.hpp>
#include <boost/test/unit_test.hpp>
#include <cstdlib>
#include <random>

namespace bfs = boost::filesystem;

BOOST_AUTO_TEST_SUITE(TextFiles)

static void createTxt(libsiedler2::Archiv& archiv, const bfs::path& outFile, unsigned numEntries)
{
    std::mt19937 rng{std::random_device{}()};
    std::uniform_int_distribution<> getRand(numEntries > 1 ? 0 : 3, 3); // Ignore randomness for single entries
    for(unsigned i = 0; i < numEntries; i++)
    {
        const auto what = getRand(rng);
        if(what == 0)
        {
            BOOST_TEST_MESSAGE("Adding empty entry " << i);
            archiv.push(nullptr);
        } else
        {
            libsiedler2::ArchivItem_Text txt;
            if(what == 1)
            {
                BOOST_TEST_MESSAGE("Adding duplicate entry " << i);
                txt.setText("Duplicate entry");
            } else if(what == 2)
                BOOST_TEST_MESSAGE("Adding empty string " << i);
            else // what == 3
            {
                BOOST_TEST_MESSAGE("Adding normal string " << i);
                std::stringstream ss;
                ss << "Example data " << i << " with\r\n some \r\n line breaks and special chars";
                std::string myText = ss.str();
                // Add an umlaut
                myText[myText.size() - 7] = '\xF6';
                txt.setText(myText);
            }
            archiv.pushC(txt);
        }
    }
    BOOST_TEST_REQUIRE(libsiedler2::Write(outFile, archiv) == 0);
}

BOOST_AUTO_TEST_CASE(ReadWriteENG)
{
    const bfs::path outFilepath = libsiedler2::test::outputPath / "outText.ENG";
    libsiedler2::Archiv archiv, archivIn;
    createTxt(archiv, outFilepath, 11);
    BOOST_TEST_REQUIRE(libsiedler2::Load(outFilepath, archivIn) == 0);
    BOOST_TEST_REQUIRE(archiv.size() == archivIn.size());
    for(unsigned i = 0; i < archiv.size(); i++)
    {
        const auto* txtOut = dynamic_cast<const libsiedler2::ArchivItem_Text*>(archiv[i]);
        const auto* txtIn = dynamic_cast<const libsiedler2::ArchivItem_Text*>(archivIn[i]);
        if(txtOut)
            BOOST_TEST(txtIn);
        else
            BOOST_TEST(!txtIn);
        if(txtIn && txtOut)
            BOOST_TEST(txtIn->getText() == txtOut->getText());
    }
}

BOOST_AUTO_TEST_CASE(ReadWriteGER)
{
    const bfs::path outFilepath = libsiedler2::test::outputPath / "outText.GER";
    libsiedler2::Archiv archiv, archivIn;
    createTxt(archiv, outFilepath, 13);
    BOOST_TEST_REQUIRE(libsiedler2::Load(outFilepath, archivIn) == 0);
    BOOST_TEST_REQUIRE(archiv.size() == archivIn.size());
    for(unsigned i = 0; i < archiv.size(); i++)
    {
        const auto* txtOut = dynamic_cast<const libsiedler2::ArchivItem_Text*>(archiv[i]);
        const auto* txtIn = dynamic_cast<const libsiedler2::ArchivItem_Text*>(archivIn[i]);
        if(txtOut)
            BOOST_TEST(txtIn);
        else
            BOOST_TEST(!txtIn);
        if(txtIn && txtOut)
            BOOST_TEST(txtIn->getText() == txtOut->getText());
    }
}

BOOST_AUTO_TEST_CASE(ReadWritePlain)
{
    const bfs::path outFilepath = libsiedler2::test::outputPath / "outTextPlain.GER";
    libsiedler2::Archiv archiv, archivIn;
    createTxt(archiv, outFilepath, 1);
    BOOST_TEST_REQUIRE(libsiedler2::Load(outFilepath, archivIn) == 0);
    BOOST_TEST_REQUIRE(archivIn.size() == 1u);
    const auto* txtOut = dynamic_cast<const libsiedler2::ArchivItem_Text*>(archiv[0]);
    const auto* txtIn = dynamic_cast<const libsiedler2::ArchivItem_Text*>(archivIn[0]);
    BOOST_TEST_REQUIRE((txtOut && txtIn));
    BOOST_TEST(txtIn->getText() == txtOut->getText());
}

BOOST_AUTO_TEST_CASE(ReadWriteSingleEmptyEntry)
{
    const bfs::path outFilepath = libsiedler2::test::outputPath / "outTextEmpty.ENG";
    libsiedler2::Archiv archiv, archivIn;
    archiv.push(nullptr);
    BOOST_TEST_REQUIRE(libsiedler2::Write(outFilepath, archiv) == 0);
    BOOST_TEST_REQUIRE(libsiedler2::Load(outFilepath, archivIn) == 0);
    BOOST_TEST_REQUIRE(archivIn.size() == 1u);
    BOOST_TEST_REQUIRE(!archivIn[0]);
}

BOOST_AUTO_TEST_CASE(ReadTxtAsLst)
{
    const bfs::path inFilepath = libsiedler2::test::inputPath / "txtAsLst.lst";
    libsiedler2::Archiv archiv;
    BOOST_TEST_REQUIRE(libsiedler2::Load(inFilepath, archiv) == 0);
    BOOST_TEST_REQUIRE(archiv.size() == 7u);
    for(unsigned i = 0; i < archiv.size(); i++)
    {
        const auto* txt = dynamic_cast<const libsiedler2::ArchivItem_Text*>(archiv[i]);
        // Item 4 is empty
        if(i == 4)
            BOOST_TEST_REQUIRE(!txt);
        else
        {
            BOOST_TEST_REQUIRE(txt);
            BOOST_TEST_REQUIRE(!txt->getText().empty());
        }
    }
}

BOOST_AUTO_TEST_CASE(ReadOriginalFiles)
{
    if(!libsiedler2::test::hasS2Data)
        return;
    for(const auto& fileEntry : bfs::recursive_directory_iterator(libsiedler2::test::s2Path))
    {
        if(is_regular_file(fileEntry.status()) && fileEntry.path().extension() == ".ENG")
        {
            BOOST_TEST_INFO_SCOPE("For " << fileEntry.path());
            libsiedler2::Archiv archiv;
            // Check only that they can be loaded and have at least 1 entry
            BOOST_TEST_REQUIRE(libsiedler2::Load(fileEntry.path(), archiv) == 0);
            BOOST_TEST_REQUIRE(archiv.size() > 0u);
            BOOST_TEST(archiv[0]);
        }
    }
}

BOOST_AUTO_TEST_SUITE_END()
