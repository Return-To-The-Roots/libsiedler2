// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "ArchivItem_Sound.h"
#include "MIDI_Track.h"
#include <array>
#include <cstdint>

namespace libsiedler2 {
/// Klasse für MIDI-Sounds.
class ArchivItem_Sound_Midi : public ArchivItem_Sound
{
public:
    ArchivItem_Sound_Midi();
    RTTR_CLONEABLE(ArchivItem_Sound_Midi)

    int load(std::istream& file, uint32_t length) override;
    int write(std::ostream& file) const override;

    const MIDI_Track* getTrack(uint16_t track) const
    {
        if(track < numTracks)
            return &tracklist[track];
        return nullptr;
    }
    uint16_t getNumTracks() const { return numTracks; }
    void addTrack(const MIDI_Track& track);
    void setPPQ(uint16_t newPPQ) { ppqs = newPPQ; }

protected:
    uint16_t numTracks, ppqs;
    std::array<MIDI_Track, 256> tracklist; //-V730_NOINIT
};

} // namespace libsiedler2
