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
#include "libsiedler2/src/ArchivItem_Sound_Midi.h"
#include "libsiedler2/src/ArchivItem_Sound_Wave.h"
#include "libsiedler2/src/ArchivItem_Sound_XMidi.h"
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

BOOST_AUTO_TEST_CASE(Clone)
{
    libsiedler2::ArchivInfo archiv;
    libsiedler2::ArchivItem_Sound_Midi snd1;
    libsiedler2::ArchivItem_Sound_XMidi snd2;
    libsiedler2::ArchivItem_Sound_Wave snd3;
    libsiedler2::ArchivItem_Sound_Other snd4;
    // Add some data
    snd1.addTrack(libsiedler2::MIDI_Track());
    snd2.addTrack(libsiedler2::MIDI_Track());
    snd3.setData(std::vector<uint8_t>(10, 42));
    snd4.setData(std::vector<uint8_t>(10, 43));
    // Copy to archiv
    archiv.pushC(snd1);
    archiv.pushC(snd2);
    archiv.pushC(snd3);
    archiv.pushC(snd4);
    libsiedler2::ArchivItem_Sound_Midi* snd5 = dynamic_cast<libsiedler2::ArchivItem_Sound_Midi*>(archiv[0]);
    libsiedler2::ArchivItem_Sound_XMidi* snd6 = dynamic_cast<libsiedler2::ArchivItem_Sound_XMidi*>(archiv[1]);
    libsiedler2::ArchivItem_Sound_Wave* snd7 = dynamic_cast<libsiedler2::ArchivItem_Sound_Wave*>(archiv[2]);
    libsiedler2::ArchivItem_Sound_Other* snd8 = dynamic_cast<libsiedler2::ArchivItem_Sound_Other*>(archiv[3]);
    BOOST_REQUIRE(snd5);
    BOOST_REQUIRE(snd6);
    BOOST_REQUIRE(snd7);
    BOOST_REQUIRE(snd8);

    BOOST_REQUIRE_EQUAL(snd5->getType(), snd1.getType());
    BOOST_REQUIRE_EQUAL(snd6->getType(), snd2.getType());
    BOOST_REQUIRE_EQUAL(snd7->getType(), snd3.getType());
    BOOST_REQUIRE_EQUAL(snd8->getType(), snd4.getType());

    BOOST_REQUIRE_EQUAL(snd5->getTrackCount(), 1u);
    BOOST_REQUIRE_EQUAL(snd6->getTrackCount(), 1u);
    BOOST_REQUIRE_EQUAL_COLLECTIONS(snd7->getData().begin(), snd7->getData().end(), snd3.getData().begin(), snd3.getData().end());
    BOOST_REQUIRE_EQUAL_COLLECTIONS(snd8->getData().begin(), snd8->getData().end(), snd4.getData().begin(), snd4.getData().end());
}

BOOST_AUTO_TEST_SUITE_END()
