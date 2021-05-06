// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "ArchivItem_Sound.h"
#include "MIDI_Track.h"
#include "XMIDI_Track.h"
#include <array>
#include <cstdint>

namespace libsiedler2 {
/// Klasse für XMIDI-Sounds.
class ArchivItem_Sound_XMidi : public ArchivItem_Sound
{
    bool isTrackConverted(uint16_t track) const;

public:
    ArchivItem_Sound_XMidi();
    RTTR_CLONEABLE(ArchivItem_Sound_XMidi)

    int load(std::istream& file, uint32_t length) override;
    int write(std::ostream& file) const override;

    XMIDI_Track* getTrack(uint16_t track)
    {
        if(track < numTracks)
            return &tracklist[track];
        return nullptr;
    }
    const XMIDI_Track* getTrack(uint16_t track) const
    {
        if(track < numTracks)
            return &tracklist[track];
        return nullptr;
    }
    const MIDI_Track& getMidiTrack(uint16_t trackIdx);
    MIDI_Track getMidiTrack(uint16_t trackIdx) const;
    uint16_t getNumTracks() const { return numTracks; }
    uint16_t getPPQN() const { return ppqs; }

    void addTrack(const XMIDI_Track& track);

protected:
    uint16_t numTracks, ppqs;
    std::array<XMIDI_Track, 256> tracklist;    //-V730_NOINIT
    std::array<MIDI_Track, 256> midiTracklist; //-V730_NOINIT
};

} // namespace libsiedler2
