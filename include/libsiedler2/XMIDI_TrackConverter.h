// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace libsiedler2 {
class MIDI_Track;
class XMIDI_Track;

class XMIDI_TrackConverter
{
private:
    struct MIDI_Event
    {
        int time;
        uint8_t status;

        std::array<uint8_t, 2> data;

        std::vector<uint8_t> buffer;
        int duration;
        MIDI_Event* next_note;
        uint32_t note_time;
        MIDI_Event* next;
        MIDI_Event() : time(0), status(0), data(), duration(0), next_note(nullptr), note_time(0), next(nullptr) {}
    };

    struct first_state // Status,   Data[0]
    {
        std::array<MIDI_Event*, 16> patch; // 0xC
        std::array<MIDI_Event*, 16> bank;  // 0xB,      0
        std::array<MIDI_Event*, 16> pan;   // 0xB,      7
        std::array<MIDI_Event*, 16> vol;   // 0xB,      10
        first_state() : patch(), bank(), pan(), vol() {}
    };

public:
    XMIDI_TrackConverter(const XMIDI_Track& track);
    ~XMIDI_TrackConverter();

    bool Convert();
    MIDI_Track CreateMidiTrack() const;

private:
    int ConvertTrackToList();

    void ApplyFirstState(first_state& fs, int chan_mask);

    void ConvertNote(int time, uint8_t status, int size);
    void ConvertEvent(int time, uint8_t status, int size, first_state& fs);
    void ConvertSystemMessage(int time, uint8_t status);

    uint32_t GetVLQ();
    uint32_t GetVLQ2();
    static void PutVLQ(uint32_t value, std::vector<uint8_t>& outBuffer);

    void CreateNewEvent(int time);

    const XMIDI_Track& track;
    MIDI_Event* events;
    uint32_t numEvents;
    MIDI_Event* curEvent;
    size_t position;

    enum MidiStatus
    {
        MIDI_STATUS_NOTE_OFF = 0x8,
        MIDI_STATUS_NOTE_ON = 0x9,
        MIDI_STATUS_AFTERTOUCH = 0xA,
        MIDI_STATUS_CONTROLLER = 0xB,
        MIDI_STATUS_PROG_CHANGE = 0xC,
        MIDI_STATUS_PRESSURE = 0xD,
        MIDI_STATUS_PITCH_WHEEL = 0xE,
        MIDI_STATUS_SYSEX = 0xF
    };

    std::array<bool, 16> bank127;
};
} // namespace libsiedler2
