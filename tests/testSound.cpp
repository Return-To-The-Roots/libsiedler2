// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

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

namespace bfs = boost::filesystem;

namespace libsiedler2 {
// LCOV_EXCL_START
static std::ostream& boost_test_print_type(std::ostream& os, SoundType st)
{
    return os << static_cast<unsigned>(st);
}
// LCOV_EXCL_STOP
} // namespace libsiedler2

BOOST_AUTO_TEST_SUITE(Sounds)

BOOST_AUTO_TEST_CASE(ReadWriteOgg)
{
    const bfs::path inPath = libsiedler2::test::inputPath / "test.ogg";
    const bfs::path outPath = libsiedler2::test::outputPath / "outTest.ogg";
    libsiedler2::Archiv archiv;
    BOOST_TEST_REQUIRE(testLoad(0, inPath, archiv));
    auto* snd = dynamic_cast<libsiedler2::ArchivItem_Sound_Other*>(archiv[0]);
    BOOST_TEST_REQUIRE(snd);
    // Hacky check
    BOOST_TEST_REQUIRE(snd->getLength() == 105243u);
    BOOST_TEST_REQUIRE(std::string(reinterpret_cast<const char*>(&snd->getData()[0])) == "OggS");
    BOOST_TEST_REQUIRE(snd->getData()[49999] == uint8_t(0xe6));
    BOOST_TEST_REQUIRE(snd->getData().back() == uint8_t(0));
    BOOST_TEST_REQUIRE(libsiedler2::Write(outPath, archiv) == 0);
    BOOST_TEST_REQUIRE(testFilesEqual(outPath, inPath));
}

BOOST_AUTO_TEST_CASE(ReadOrigSoundLst)
{
    if(!libsiedler2::test::hasS2Data)
        return;
    const bfs::path inPath = libsiedler2::test::s2Path / "DATA/SOUNDDAT/SOUND.LST";
    libsiedler2::Archiv archivLst;
    BOOST_TEST_REQUIRE(libsiedler2::Load(inPath, archivLst) == 0);
    BOOST_TEST_REQUIRE(archivLst.size() == 200u);
    auto* snd = dynamic_cast<libsiedler2::ArchivItem_Sound*>(archivLst[0]);
    BOOST_TEST_REQUIRE(snd);
    BOOST_TEST_REQUIRE(snd->getType() == libsiedler2::SoundType::XMidi);
    BOOST_TEST_REQUIRE(static_cast<libsiedler2::ArchivItem_Sound_XMidi*>(snd)->getNumTracks() == 1u);
    BOOST_TEST_REQUIRE(static_cast<libsiedler2::ArchivItem_Sound_XMidi*>(snd)->getMidiTrack(0).getMidLength()
                       == 39167u);
    BOOST_TEST_REQUIRE(!archivLst[1]);
    snd = dynamic_cast<libsiedler2::ArchivItem_Sound*>(archivLst[51]);
    BOOST_TEST_REQUIRE(snd);
    BOOST_TEST_REQUIRE(snd->getType() == libsiedler2::SoundType::Wave);
    BOOST_TEST_REQUIRE(static_cast<libsiedler2::ArchivItem_Sound_Wave*>(snd)->getLength() == 9943u);
}

BOOST_AUTO_TEST_CASE(ReadOrigSng)
{
    if(!libsiedler2::test::hasS2Data)
        return;
    const bfs::path inPath = libsiedler2::test::s2Path / "DATA/SOUNDDAT/SNG/SNG_0001.DAT";
    libsiedler2::Archiv archivSNG;
    BOOST_TEST_REQUIRE(libsiedler2::Load(inPath, archivSNG) == 0);
    auto* snd = dynamic_cast<libsiedler2::ArchivItem_Sound*>(archivSNG[0]);
    BOOST_TEST_REQUIRE(snd);
    BOOST_TEST_REQUIRE(snd->getType() == libsiedler2::SoundType::XMidi);
    BOOST_TEST_REQUIRE(static_cast<libsiedler2::ArchivItem_Sound_XMidi*>(snd)->getNumTracks() == 1u);
    BOOST_TEST_REQUIRE(static_cast<libsiedler2::ArchivItem_Sound_XMidi*>(snd)->getMidiTrack(0).getMidLength() == 7069u);
}

BOOST_AUTO_TEST_CASE(ReadWriteWavMono)
{
    const bfs::path inPath = libsiedler2::test::inputPath / "testMono.wav";
    const bfs::path outPath = libsiedler2::test::outputPath / "outMono.wav";
    libsiedler2::Archiv archiv;
    BOOST_TEST_REQUIRE(libsiedler2::Load(inPath, archiv) == 0);
    auto* snd = dynamic_cast<libsiedler2::ArchivItem_Sound_Wave*>(archiv[0]);
    BOOST_TEST_REQUIRE(snd);
    // Hacky check
    BOOST_TEST_REQUIRE(snd->getLength() == 135944u);
    BOOST_TEST_REQUIRE(snd->getData().back() == uint8_t(0));
    BOOST_TEST_REQUIRE(libsiedler2::Write(outPath, archiv) == 0);
    BOOST_TEST_REQUIRE(testFilesEqual(outPath, inPath));
}

BOOST_AUTO_TEST_CASE(ReadWriteWavStereo)
{
    const bfs::path inPath = libsiedler2::test::inputPath / "testStereo.wav";
    const bfs::path outPath = libsiedler2::test::outputPath / "testStereo.wav";
    libsiedler2::Archiv archiv;
    BOOST_TEST_REQUIRE(libsiedler2::Load(inPath, archiv) == 0);
    auto* snd = dynamic_cast<libsiedler2::ArchivItem_Sound_Wave*>(archiv[0]);
    BOOST_TEST_REQUIRE(snd);
    // Hacky check
    BOOST_TEST_REQUIRE(snd->getLength() == 111414u);
    BOOST_TEST_REQUIRE(snd->getData().back() == uint8_t(0));
    BOOST_TEST_REQUIRE(libsiedler2::Write(outPath, archiv) == 0);
    BOOST_TEST_REQUIRE(testFilesEqual(outPath, inPath));
}

BOOST_AUTO_TEST_CASE(ReadWriteMid)
{
    const bfs::path inPath = libsiedler2::test::inputPath / "testMidi.mid";
    const bfs::path outPath = libsiedler2::test::outputPath / "outMidi.mid";
    libsiedler2::Archiv archiv;
    BOOST_TEST_REQUIRE(libsiedler2::Load(inPath, archiv) == 0);
    auto* snd = dynamic_cast<libsiedler2::ArchivItem_Sound_Midi*>(archiv[0]);
    BOOST_TEST_REQUIRE(snd);
    // Hacky check
    BOOST_TEST_REQUIRE(libsiedler2::Write(outPath, archiv) == 0);
    BOOST_TEST_REQUIRE(testFilesEqual(outPath, inPath));
}

BOOST_AUTO_TEST_CASE(ReadWriteXMid)
{
    const bfs::path inPath = libsiedler2::test::inputPath / "testXMidi.xmi";
    const bfs::path outPath = libsiedler2::test::outputPath / "outXMidi.xmi";
    libsiedler2::Archiv archiv;
    BOOST_TEST_REQUIRE(libsiedler2::Load(inPath, archiv) == 0);
    auto* snd = dynamic_cast<libsiedler2::ArchivItem_Sound_XMidi*>(archiv[0]);
    BOOST_TEST_REQUIRE(snd);
    // Hacky check
    for(unsigned i = 0; i < snd->getNumTracks(); i++)
        BOOST_REQUIRE_NO_THROW(snd->getMidiTrack(i));
    BOOST_TEST_REQUIRE(libsiedler2::Write(outPath, archiv) == 0);
    BOOST_TEST_REQUIRE(testFilesEqual(outPath, inPath));
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
    BOOST_TEST_REQUIRE(snd5);
    BOOST_TEST_REQUIRE(snd6);
    BOOST_TEST_REQUIRE(snd7);
    BOOST_TEST_REQUIRE(snd8);

    BOOST_TEST_REQUIRE(snd5->getType() == snd1.getType());
    BOOST_TEST_REQUIRE(snd6->getType() == snd2.getType());
    BOOST_TEST_REQUIRE(snd7->getType() == snd3.getType());
    BOOST_TEST_REQUIRE(snd8->getType() == snd4.getType());

    BOOST_TEST_REQUIRE(snd5->getNumTracks() == 1u);
    BOOST_TEST_REQUIRE(snd6->getNumTracks() == 1u);
    BOOST_TEST_REQUIRE(snd7->getData() == snd3.getData(), boost::test_tools::per_element());
    BOOST_TEST_REQUIRE(snd8->getData() == snd4.getData(), boost::test_tools::per_element());
}

BOOST_AUTO_TEST_SUITE_END()
