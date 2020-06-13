// Copyright (c) 2017 - 2020 Settlers Freaks (sf-team at siedler25.org)
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

#include "cmpFiles.h"
#include "test/config.h"
#include "libsiedler2/Archiv.h"
#include "libsiedler2/ArchivItem_Sound_Midi.h"
#include "libsiedler2/ArchivItem_Sound_Other.h"
#include "libsiedler2/ArchivItem_Sound_Wave.h"
#include "libsiedler2/ArchivItem_Sound_XMidi.h"
#include "libsiedler2/libsiedler2.h"
#include <boost/filesystem.hpp>
#include <boost/test/unit_test.hpp>

namespace libsiedler2 {
static std::ostream& boost_test_print_type(std::ostream& os, SoundType st)
{
    return os << static_cast<unsigned>(st);
}
} // namespace libsiedler2

BOOST_AUTO_TEST_SUITE(Sounds)

BOOST_AUTO_TEST_CASE(ReadWriteOgg)
{
    const std::string inPath = libsiedler2::test::inputPath + "/test.ogg";
    const std::string outPath = libsiedler2::test::outputPath + "/outTest.ogg";
    libsiedler2::Archiv archiv;
    BOOST_REQUIRE(testLoad(0, inPath, archiv));
    auto* snd = dynamic_cast<libsiedler2::ArchivItem_Sound_Other*>(archiv[0]);
    BOOST_REQUIRE(snd);
    // Hacky check
    BOOST_REQUIRE_EQUAL(snd->getLength(), 105243u);
    BOOST_REQUIRE_EQUAL(std::string(reinterpret_cast<const char*>(&snd->getData()[0])), "OggS");
    BOOST_REQUIRE_EQUAL(snd->getData()[49999], uint8_t(0xe6));
    BOOST_REQUIRE_EQUAL(snd->getData().back(), uint8_t(0));
    BOOST_REQUIRE_EQUAL(libsiedler2::Write(outPath, archiv), 0);
    BOOST_REQUIRE(testFilesEqual(outPath, inPath));
}

BOOST_AUTO_TEST_CASE(ReadOrigSoundLst)
{
    if(!libsiedler2::test::hasS2Data)
        return;
    const std::string inPath = libsiedler2::test::s2Path + "/DATA/SOUNDDAT/SOUND.LST";
    libsiedler2::Archiv archivLst;
    BOOST_REQUIRE_EQUAL(libsiedler2::Load(inPath, archivLst), 0);
    BOOST_REQUIRE_EQUAL(archivLst.size(), 200u);
    auto* snd = dynamic_cast<libsiedler2::ArchivItem_Sound*>(archivLst[0]);
    BOOST_REQUIRE(snd);
    BOOST_REQUIRE_EQUAL(snd->getType(), libsiedler2::SoundType::XMidi);
    BOOST_REQUIRE_EQUAL(static_cast<libsiedler2::ArchivItem_Sound_XMidi*>(snd)->getNumTracks(), 1u);
    BOOST_REQUIRE_EQUAL(static_cast<libsiedler2::ArchivItem_Sound_XMidi*>(snd)->getMidiTrack(0).getMidLength(), 39167u);
    BOOST_REQUIRE(!archivLst[1]);
    snd = dynamic_cast<libsiedler2::ArchivItem_Sound*>(archivLst[51]);
    BOOST_REQUIRE(snd);
    BOOST_REQUIRE_EQUAL(snd->getType(), libsiedler2::SoundType::Wave);
    BOOST_REQUIRE_EQUAL(static_cast<libsiedler2::ArchivItem_Sound_Wave*>(snd)->getLength(), 9943u);
}

BOOST_AUTO_TEST_CASE(ReadOrigSng)
{
    if(!libsiedler2::test::hasS2Data)
        return;
    const std::string inPath = libsiedler2::test::s2Path + "/DATA/SOUNDDAT/SNG/SNG_0001.DAT";
    libsiedler2::Archiv archivSNG;
    BOOST_REQUIRE_EQUAL(libsiedler2::Load(inPath, archivSNG), 0);
    auto* snd = dynamic_cast<libsiedler2::ArchivItem_Sound*>(archivSNG[0]);
    BOOST_REQUIRE(snd);
    BOOST_REQUIRE_EQUAL(snd->getType(), libsiedler2::SoundType::XMidi);
    BOOST_REQUIRE_EQUAL(static_cast<libsiedler2::ArchivItem_Sound_XMidi*>(snd)->getNumTracks(), 1u);
    BOOST_REQUIRE_EQUAL(static_cast<libsiedler2::ArchivItem_Sound_XMidi*>(snd)->getMidiTrack(0).getMidLength(), 7069u);
}

BOOST_AUTO_TEST_CASE(ReadWriteWavMono)
{
    const std::string inPath = libsiedler2::test::inputPath + "/testMono.wav";
    const std::string outPath = libsiedler2::test::outputPath + "/outMono.wav";
    libsiedler2::Archiv archiv;
    BOOST_REQUIRE_EQUAL(libsiedler2::Load(inPath, archiv), 0);
    auto* snd = dynamic_cast<libsiedler2::ArchivItem_Sound_Wave*>(archiv[0]);
    BOOST_REQUIRE(snd);
    // Hacky check
    BOOST_REQUIRE_EQUAL(snd->getLength(), 135944u);
    BOOST_REQUIRE_EQUAL(snd->getData().back(), uint8_t(0));
    BOOST_REQUIRE_EQUAL(libsiedler2::Write(outPath, archiv), 0);
    BOOST_REQUIRE(testFilesEqual(outPath, inPath));
}

BOOST_AUTO_TEST_CASE(ReadWriteWavStereo)
{
    const std::string inPath = libsiedler2::test::inputPath + "/testStereo.wav";
    const std::string outPath = libsiedler2::test::outputPath + "/testStereo.wav";
    libsiedler2::Archiv archiv;
    BOOST_REQUIRE_EQUAL(libsiedler2::Load(inPath, archiv), 0);
    auto* snd = dynamic_cast<libsiedler2::ArchivItem_Sound_Wave*>(archiv[0]);
    BOOST_REQUIRE(snd);
    // Hacky check
    BOOST_REQUIRE_EQUAL(snd->getLength(), 111414u);
    BOOST_REQUIRE_EQUAL(snd->getData().back(), uint8_t(0));
    BOOST_REQUIRE_EQUAL(libsiedler2::Write(outPath, archiv), 0);
    BOOST_REQUIRE(testFilesEqual(outPath, inPath));
}

BOOST_AUTO_TEST_CASE(ReadWriteMid)
{
    const std::string inPath = libsiedler2::test::inputPath + "/testMidi.mid";
    const std::string outPath = libsiedler2::test::outputPath + "/outMidi.mid";
    libsiedler2::Archiv archiv;
    BOOST_REQUIRE_EQUAL(libsiedler2::Load(inPath, archiv), 0);
    auto* snd = dynamic_cast<libsiedler2::ArchivItem_Sound_Midi*>(archiv[0]);
    BOOST_REQUIRE(snd);
    // Hacky check
    BOOST_REQUIRE_EQUAL(libsiedler2::Write(outPath, archiv), 0);
    BOOST_REQUIRE(testFilesEqual(outPath, inPath));
}

BOOST_AUTO_TEST_CASE(ReadWriteXMid)
{
    const std::string inPath = libsiedler2::test::inputPath + "/testXMidi.xmi";
    const std::string outPath = libsiedler2::test::outputPath + "/outXMidi.xmi";
    libsiedler2::Archiv archiv;
    BOOST_REQUIRE_EQUAL(libsiedler2::Load(inPath, archiv), 0);
    auto* snd = dynamic_cast<libsiedler2::ArchivItem_Sound_XMidi*>(archiv[0]);
    BOOST_REQUIRE(snd);
    // Hacky check
    for(unsigned i = 0; i < snd->getNumTracks(); i++)
        BOOST_REQUIRE_NO_THROW(snd->getMidiTrack(i));
    BOOST_REQUIRE_EQUAL(libsiedler2::Write(outPath, archiv), 0);
    BOOST_REQUIRE(testFilesEqual(outPath, inPath));
}

BOOST_AUTO_TEST_CASE(Clone)
{
    libsiedler2::Archiv archiv;
    libsiedler2::ArchivItem_Sound_Midi snd1;
    libsiedler2::ArchivItem_Sound_XMidi snd2;
    libsiedler2::ArchivItem_Sound_Wave snd3;
    libsiedler2::ArchivItem_Sound_Other snd4(libsiedler2::SoundType::MP3);
    // Add some data
    snd1.addTrack(libsiedler2::MIDI_Track());
    snd2.addTrack(libsiedler2::XMIDI_Track());
    snd3.setData(std::vector<uint8_t>(10, 42));
    snd4.setData(std::vector<uint8_t>(10, 43));
    // Copy to archiv
    archiv.pushC(snd1);
    archiv.pushC(snd2);
    archiv.pushC(snd3);
    archiv.pushC(snd4);
    auto* snd5 = dynamic_cast<libsiedler2::ArchivItem_Sound_Midi*>(archiv[0]);
    auto* snd6 = dynamic_cast<libsiedler2::ArchivItem_Sound_XMidi*>(archiv[1]);
    auto* snd7 = dynamic_cast<libsiedler2::ArchivItem_Sound_Wave*>(archiv[2]);
    auto* snd8 = dynamic_cast<libsiedler2::ArchivItem_Sound_Other*>(archiv[3]);
    BOOST_REQUIRE(snd5);
    BOOST_REQUIRE(snd6);
    BOOST_REQUIRE(snd7);
    BOOST_REQUIRE(snd8);

    BOOST_REQUIRE_EQUAL(snd5->getType(), snd1.getType());
    BOOST_REQUIRE_EQUAL(snd6->getType(), snd2.getType());
    BOOST_REQUIRE_EQUAL(snd7->getType(), snd3.getType());
    BOOST_REQUIRE_EQUAL(snd8->getType(), snd4.getType());

    BOOST_REQUIRE_EQUAL(snd5->getNumTracks(), 1u);
    BOOST_REQUIRE_EQUAL(snd6->getNumTracks(), 1u);
    BOOST_REQUIRE_EQUAL_COLLECTIONS(snd7->getData().begin(), snd7->getData().end(), snd3.getData().begin(), snd3.getData().end());
    BOOST_REQUIRE_EQUAL_COLLECTIONS(snd8->getData().begin(), snd8->getData().end(), snd4.getData().begin(), snd4.getData().end());
}

BOOST_AUTO_TEST_SUITE_END()
