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
#include "libsiedler2/src/ArchivItem_Text.h"
#include "libsiedler2/src/libsiedler2.h"
#include <boost/filesystem.hpp>
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(TextFiles)

void createTxt(libsiedler2::ArchivInfo& archiv, const std::string& outFile, unsigned numEntries)
{
    for(unsigned i = 0; i < numEntries; i++)
    {
        // Add some empty entries
        if(i % 4 == 0 && i > 0)
            archiv.push(NULL);
        libsiedler2::ArchivItem_Text txt;
        std::stringstream ss;
        ss << "Example data " << i << " with\r\n some \r\n line breaks and special chars";
        std::string myText = ss.str();
        // Add an umlaut
        myText[myText.size() - 7] = '\xF6';
        txt.setText(myText);
        archiv.pushC(txt);
    }
    BOOST_REQUIRE_EQUAL(libsiedler2::Write(outFile, archiv), 0);
}

BOOST_AUTO_TEST_CASE(ReadWriteENG)
{
    std::string outFilepath = testOutputPath + "/outText.ENG";
    libsiedler2::ArchivInfo archiv, archivIn;
    createTxt(archiv, outFilepath, 3);
    BOOST_REQUIRE_EQUAL(libsiedler2::Load(outFilepath, archivIn), 0);
    BOOST_REQUIRE_EQUAL(archiv.size(), archivIn.size());
    for(unsigned i = 0; i < archiv.size(); i++)
    {
        const libsiedler2::ArchivItem_Text* txtOut = dynamic_cast<const libsiedler2::ArchivItem_Text*>(archiv[i]);
        const libsiedler2::ArchivItem_Text* txtIn = dynamic_cast<const libsiedler2::ArchivItem_Text*>(archivIn[i]);
        if(txtOut)
            BOOST_REQUIRE(txtIn);
        else
            BOOST_REQUIRE(!txtIn);
        if(txtIn && txtOut)
            BOOST_REQUIRE_EQUAL(txtIn->getText(), txtOut->getText());
    }
}

BOOST_AUTO_TEST_CASE(ReadWriteGER)
{
    std::string outFilepath = testOutputPath + "/outText.GER";
    libsiedler2::ArchivInfo archiv, archivIn;
    createTxt(archiv, outFilepath, 6);
    BOOST_REQUIRE_EQUAL(libsiedler2::Load(outFilepath, archivIn), 0);
    BOOST_REQUIRE_EQUAL(archiv.size(), archivIn.size());
    for(unsigned i = 0; i < archiv.size(); i++)
    {
        const libsiedler2::ArchivItem_Text* txtOut = dynamic_cast<const libsiedler2::ArchivItem_Text*>(archiv[i]);
        const libsiedler2::ArchivItem_Text* txtIn = dynamic_cast<const libsiedler2::ArchivItem_Text*>(archivIn[i]);
        if(txtOut)
            BOOST_REQUIRE(txtIn);
        else
            BOOST_REQUIRE(!txtIn);
        if(txtIn && txtOut)
            BOOST_REQUIRE_EQUAL(txtIn->getText(), txtOut->getText());
    }
}

BOOST_AUTO_TEST_CASE(ReadWritePlain)
{
    std::string outFilepath = testOutputPath + "/outTextPlain.GER";
    libsiedler2::ArchivInfo archiv, archivIn;
    createTxt(archiv, outFilepath, 1);
    BOOST_REQUIRE_EQUAL(libsiedler2::Load(outFilepath, archivIn), 0);
    BOOST_REQUIRE_EQUAL(archivIn.size(), 1u);
    const libsiedler2::ArchivItem_Text* txtOut = dynamic_cast<const libsiedler2::ArchivItem_Text*>(archiv[0]);
    const libsiedler2::ArchivItem_Text* txtIn = dynamic_cast<const libsiedler2::ArchivItem_Text*>(archivIn[0]);
    BOOST_REQUIRE(txtOut && txtIn);
    BOOST_REQUIRE_EQUAL(txtIn->getText(), txtOut->getText());
}

BOOST_AUTO_TEST_CASE(ReadTxtAsLst)
{
    std::string inFilepath = "testFiles/txtAsLst.lst";
    libsiedler2::ArchivInfo archiv;
    BOOST_REQUIRE_EQUAL(libsiedler2::Load(inFilepath, archiv), 0);
    BOOST_REQUIRE_EQUAL(archiv.size(), 7u);
    for(unsigned i = 0; i < archiv.size(); i++)
    {
        const libsiedler2::ArchivItem_Text* txt = dynamic_cast<const libsiedler2::ArchivItem_Text*>(archiv[i]);
        // Item 4 is empty
        if(i == 4)
            BOOST_REQUIRE(!txt);
        else
        {
            BOOST_REQUIRE(txt);
            BOOST_REQUIRE(!txt->getText().empty());
        }
    }
}

BOOST_AUTO_TEST_SUITE_END()
