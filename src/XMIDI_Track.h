// Copyright (c) 2005 - 2015 Settlers Freaks (sf-team at siedler25.org)
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
#ifndef XMIDI_TRACK_H_INCLUDED
#define XMIDI_TRACK_H_INCLUDED

#pragma once

#include <vector>
#include <boost/array.hpp>

class MIDI_Track;

class XMIDI_Track
{
    private:
        struct MIDI_Event
        {
            int            time;
            unsigned char  status;

            boost::array<unsigned char, 2>  data;

            std::vector<unsigned char>  buffer;
            int            duration;
            MIDI_Event*     next_note;
            unsigned int   note_time;
            MIDI_Event*     next;
            MIDI_Event(): time(0), status(0), data(), duration(0), next_note(NULL), note_time(0), next(NULL)
            {}
        };

        struct first_state          // Status,   Data[0]
        {
            boost::array<MIDI_Event*, 16> patch;  // 0xC
            boost::array<MIDI_Event*, 16> bank;   // 0xB,      0
            boost::array<MIDI_Event*, 16> pan;    // 0xB,      7
            boost::array<MIDI_Event*, 16> vol;    // 0xB,      10
            first_state(): patch(), bank(), pan(), vol()
            {}
        };

    public:
        XMIDI_Track(MIDI_Track* track);
        ~XMIDI_Track();

        int Convert();

    private:
        int ConvertTrackToList();
        void ConvertListToMTrk();

        void ApplyFirstState(first_state& fs, int chan_mask);

        int ConvertNote(const int time, const unsigned char status, const int size);
        int ConvertEvent(const int time, const unsigned char status, const int size, first_state& fs);
        int ConvertSystemMessage(const int time, const unsigned char status);

        int GetVLQ(unsigned int& quant);
        int GetVLQ2(unsigned int& quant);
        void PutVLQ(unsigned int value);

        void CreateNewEvent(int time);

    private:
        MIDI_Track* track;
        MIDI_Event* events;
        unsigned int event_count;
        MIDI_Event* current;
        size_t position;

        enum
        {
            MIDI_STATUS_NOTE_OFF    = 0x8,
            MIDI_STATUS_NOTE_ON     = 0x9,
            MIDI_STATUS_AFTERTOUCH  = 0xA,
            MIDI_STATUS_CONTROLLER  = 0xB,
            MIDI_STATUS_PROG_CHANGE = 0xC,
            MIDI_STATUS_PRESSURE    = 0xD,
            MIDI_STATUS_PITCH_WHEEL = 0xE,
            MIDI_STATUS_SYSEX       = 0xF
        };

        boost::array<bool, 16> bank127;
};

#endif // XMIDIFILE_H_
