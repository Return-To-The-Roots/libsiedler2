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
#include "libsiedler2/src/ArchivItem_Sound_Other.h"
#include <boost/filesystem.hpp>
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(Sounds)

BOOST_AUTO_TEST_CASE(ReadOgg)
{
    std::string inPath = "testFiles/test.ogg";
    std::string outPath = testOutputPath + "/outTest.ogg";
    BOOST_REQUIRE(bfs::exists(inPath));
    libsiedler2::ArchivInfo act;
    BOOST_REQUIRE_EQUAL(libsiedler2::Load(inPath, act), 0);
    libsiedler2::ArchivItem_Sound_Other* snd = dynamic_cast<libsiedler2::ArchivItem_Sound_Other*>(act[0]);
    BOOST_REQUIRE(snd);
    // Hacky check as we don't have a writer
    BOOST_REQUIRE_EQUAL(snd->getLength(), 105243u);
    BOOST_REQUIRE_EQUAL(std::string(reinterpret_cast<const char*>(&snd->getData()[0])), "OggS");
    BOOST_REQUIRE_EQUAL(snd->getData()[49999], uint8_t(0xe6));
    BOOST_REQUIRE_EQUAL(snd->getData().back(), uint8_t(0));
}

BOOST_AUTO_TEST_SUITE_END()
