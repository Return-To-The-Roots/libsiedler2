// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "XMIDI_TrackConverter.h"
#include "GammaTable.h"
#include "MIDI_Track.h"
#include "XMIDI_Track.h"
#include <boost/endian/conversion.hpp>
#include <algorithm>
#include <cstring>
#include <iterator>
#include <stdexcept>
#include <string>

// Enable this to log status reports of the xmidi conversion to stdout
#define RTTR_LOG_XMIDI 0
#if RTTR_LOG_XMIDI
#    include <iostream>
#endif // RTTR_LOG_XMIDI

namespace libsiedler2 {
static const int PATCH_VOL_PAN_BIAS = 5;
static GammaTable<uint8_t> VolumeCurve(128);

struct LogStatus
{
    LogStatus(const std::string& msg = "") { *this << msg; }

    template<typename T>
    LogStatus& operator<<(const T& el)
    {
#if RTTR_LOG_XMIDI
        std::cout << el;
#else
        (void)el;
#endif // RTTR_LOG_XMIDI
        return *this;
    }
};

XMIDI_TrackConverter::XMIDI_TrackConverter(const XMIDI_Track& track) : track(track)
{
    position = 0;
    curEvent = nullptr;
    events = nullptr;
    numEvents = 0,

    std::fill(bank127.begin(), bank127.end(), false);
}

XMIDI_TrackConverter::~XMIDI_TrackConverter()
{
    curEvent = events;
    while(curEvent)
    {
        MIDI_Event* next = curEvent->next;
        delete curEvent;
        curEvent = next;
    }
}

bool XMIDI_TrackConverter::Convert()
{
    // Already done -> Abort
    if(events)
        return false;
    ConvertTrackToList();
    return true;
}

int XMIDI_TrackConverter::ConvertTrackToList()
{
    int time = 0; // 120th of a second
    bool end = false;
    uint32_t status = 0;
    int retval = 0;

    position = 0;

    first_state fs;

    uint32_t curEvntNum = 0;
    while(!end && position < track.getData().size())
    {
        const uint32_t delta = GetVLQ2();
        time += delta;
        status = (uint32_t)track.getData()[position++];
        LogStatus("Event #") << (curEvntNum++) << " d:" << delta;

        const auto evntType = MidiStatus(status >> 4);
        switch(evntType)
        {
            case MIDI_STATUS_NOTE_ON:
                LogStatus(" Note on");
                retval |= 1 << (status & 0xF);
                ConvertNote(time, status, 3);
                break;

            case MIDI_STATUS_NOTE_OFF:
                LogStatus(" Note off");
                ConvertNote(time, status, 2);
                break;

            // 2 byte data
            case MIDI_STATUS_AFTERTOUCH:
            case MIDI_STATUS_CONTROLLER:
            case MIDI_STATUS_PITCH_WHEEL:
                if(evntType == MIDI_STATUS_AFTERTOUCH)
                    LogStatus(" after touch");
                else if(evntType == MIDI_STATUS_CONTROLLER)
                    LogStatus(" controller");
                else
                    LogStatus(" pitch wheel");
                ConvertEvent(time, status, 2, fs);
                break;

            // 1 byte data
            case MIDI_STATUS_PROG_CHANGE:
            case MIDI_STATUS_PRESSURE:
                if(evntType == MIDI_STATUS_PROG_CHANGE)
                    LogStatus(" prog change");
                else
                    LogStatus(" pressure");
                ConvertEvent(time, status, 1, fs);
                break;

            case MIDI_STATUS_SYSEX:
                LogStatus(" System");
                if(status == 0xFF)
                {
                    uint32_t data = track.getData()[position];
                    // Handle those 2 in here
                    if(data == 0x2F)
                        end = true;
                    else if(data == 0x51)
                    {
                        // Tempo setting -> ignore
                        position++;
                        uint32_t offset = GetVLQ();
                        LogStatus(" ignored tempo, sz ") << offset;
                        position += offset;
                        break;
                    }
                }
                ConvertSystemMessage(time, status);
                break;
        }
        LogStatus("\n");
    }
    curEvent = events;

    ApplyFirstState(fs, retval);

    return retval;
}

uint32_t XMIDI_TrackConverter::GetVLQ()
{
    uint32_t quant = 0;
    uint8_t data = 0;

    for(int i = 0; i < 4; i++)
    {
        data = track.getData()[position++];
        quant <<= 7;
        quant |= data & 0x7F;

        if(!(data & 0x80))
            return quant;
    }
    throw std::runtime_error("Could not get VLQ");
}

uint32_t XMIDI_TrackConverter::GetVLQ2()
{
    uint32_t quant = 0;
    for(int i = 0; i < 4; i++)
    {
        uint8_t data = track.getData()[position];
        if(data & 0x80)
            return quant;
        quant += data;
        ++position;
    }
    // Exceeded max size
    throw std::runtime_error("Could not get VLQ2");
}

void XMIDI_TrackConverter::PutVLQ(uint32_t value, std::vector<uint8_t>& outBuffer)
{
    int buffer;
    int i = 1;
    buffer = value & 0x7F;
    while(value >>= 7)
    {
        buffer <<= 8;
        buffer |= ((value & 0x7F) | 0x80);
        i++;
    }

    for(int j = 0; j < i; j++)
    {
        outBuffer.push_back(buffer & 0xFF);
        buffer >>= 8;
    }
}

void XMIDI_TrackConverter::ConvertNote(int time, uint8_t status, int size)
{
    int data = track.getData()[position++];

    CreateNewEvent(time);
    curEvent->status = status;

    curEvent->data[0] = data;
    curEvent->data[1] = track.getData()[position++];

    // Volume modify the note on's, only if converting
    if((curEvent->status >> 4) == MIDI_STATUS_NOTE_ON && curEvent->data[1])
        curEvent->data[1] = VolumeCurve[curEvent->data[1]];

    if(size == 2)
        return;

    // XMI Note On handling
    curEvent->duration = GetVLQ();

    LogStatus(" note duration ") << curEvent->duration;

    // This is an optimization
    MIDI_Event* prev = curEvent;

    // Create a note off
    CreateNewEvent(time + curEvent->duration);

    curEvent->status = status;
    curEvent->data[0] = data;
    curEvent->data[1] = 0;

    // Optimization
    curEvent = prev;
}

void XMIDI_TrackConverter::ConvertEvent(int time, uint8_t status, int size, first_state& fs)
{
    int data = track.getData()[position++];
    auto curStatus = MidiStatus(status >> 4);
    uint8_t statValue = status & 0xF;
    // Bank changes are handled here
    if(curStatus == MIDI_STATUS_CONTROLLER && data == 0)
        bank127[statValue] = false;
    // Disable patch changes on Track 10 is doing a conversion
    else if(curStatus == MIDI_STATUS_PROG_CHANGE && statValue == 9)
        return;

    CreateNewEvent(time);
    curEvent->status = status;
    curEvent->data[0] = data;
    LogStatus(" type ") << unsigned(statValue) << ", " << unsigned(data);

    // Check for patch change, and update fs if req
    if(curStatus == MIDI_STATUS_PROG_CHANGE)
    {
        if(!fs.patch[statValue] || fs.patch[statValue]->time > time)
            fs.patch[statValue] = curEvent;
    }

    // Controllers
    else if(curStatus == MIDI_STATUS_CONTROLLER)
    {
        // Volume
        if(curEvent->data[0] == 7)
        {
            if(!fs.vol[statValue] || fs.vol[statValue]->time > time)
                fs.vol[statValue] = curEvent;
        }
        // Pan
        else if(curEvent->data[0] == 10)
        {
            if(!fs.pan[statValue] || fs.pan[statValue]->time > time)
                fs.pan[statValue] = curEvent;
        }
    }

    if(size == 1)
        return;

    curEvent->data[1] = track.getData()[position++];
    LogStatus(", ") << unsigned(curEvent->data[1]);

    // Volume modify the volume controller, only if converting
    if(curStatus == MIDI_STATUS_CONTROLLER && curEvent->data[0] == 7)
        curEvent->data[1] = VolumeCurve[curEvent->data[1]];
}

void XMIDI_TrackConverter::ConvertSystemMessage(int time, uint8_t status)
{
    CreateNewEvent(time);
    curEvent->status = status;

    // Handling of Meta events
    if(status == 0xFF)
        curEvent->data[0] = track.getData()[position++];

    uint32_t len = GetVLQ();
    LogStatus(" curStat:") << unsigned(status) << " len: " << len;

    curEvent->buffer.clear();
    if(!len)
        return;

    std::copy(track.getData().begin() + position, track.getData().begin() + position + len,
              std::back_inserter(curEvent->buffer));
    position += len;
}

void XMIDI_TrackConverter::CreateNewEvent(int time)
{
    if(!events)
    {
        events = curEvent = new MIDI_Event;
        if(time > 0)
            curEvent->time = time;
        numEvents++;
        return;
    }

    if(time < 0 || events->time > time)
    {
        auto* event = new MIDI_Event;
        event->next = events;
        events = curEvent = event;
        numEvents++;
        return;
    }

    if(!curEvent || curEvent->time > time)
        curEvent = events;

    while(curEvent->next)
    {
        if(curEvent->next->time > time)
        {
            auto* event = new MIDI_Event;

            event->next = curEvent->next;
            curEvent->next = event;
            curEvent = event;
            curEvent->time = time;
            numEvents++;
            return;
        }

        curEvent = curEvent->next;
    }

    curEvent->next = new MIDI_Event;
    curEvent = curEvent->next;
    curEvent->time = time;

    numEvents++;
}

MIDI_Track XMIDI_TrackConverter::CreateMidiTrack() const
{
    std::vector<uint8_t> midData;
    int time = 0;
    int lasttime = 0;
    uint8_t last_status = 0;

    // Header schreiben
    midData.push_back('M');
    midData.push_back('T');
    midData.push_back('r');
    midData.push_back('k');

    // Placeholder for length
    size_t lenPos = midData.size();
    std::fill_n(std::back_inserter(midData), 4, 0);

    for(MIDI_Event* event = events; event; event = event->next)
    {
        if(event->status == 0xFF && event->data[0] == 0x2f)
        {
            lasttime = event->time;
            break;
        }

        uint32_t delta = (event->time - time);
        time = event->time;

        PutVLQ(delta, midData);

        if((event->status != last_status) || (event->status >= 0xF0))
            midData.push_back(event->status);

        last_status = event->status;

        switch(event->status >> 4)
        {
            // 2 bytes data
            // Note off, Note on, Aftertouch, Controller and Pitch Wheel
            case 0x8:
            case 0x9:
            case 0xA:
            case 0xB:
            case 0xE:
                midData.push_back(event->data[0]);
                midData.push_back(event->data[1]);
                break;

            // 1 bytes data
            // Program Change and Channel Pressure
            case 0xC:
            case 0xD: midData.push_back(event->data[0]); break;

            // Variable length
            // SysEx
            case 0xF:
                if(event->status == 0xFF)
                    midData.push_back(event->data[0]);

                PutVLQ(static_cast<uint32_t>(event->buffer.size()), midData);

                std::copy(event->buffer.begin(), event->buffer.end(), std::back_inserter(midData));
                break;
        }
    }

    // Write out end of stream marker
    if(lasttime > time)
        PutVLQ(lasttime - time, midData);
    else
        PutVLQ(0, midData);

    midData.push_back(0xFF);
    midData.push_back(0x2F);

    PutVLQ(0, midData);

    // Write length
    uint32_t length = boost::endian::native_to_big(static_cast<uint32_t>(midData.size() - 8));
    std::memcpy(&midData[lenPos], &length, sizeof(length));

    return MIDI_Track(midData);
}

void XMIDI_TrackConverter::ApplyFirstState(first_state& fs, int chan_mask)
{
    for(uint8_t channel = 0; channel < 16; channel++)
    {
        MIDI_Event* patch = fs.patch[channel];
        MIDI_Event* vol = fs.vol[channel];
        MIDI_Event* pan = fs.pan[channel];
        MIDI_Event* bank = fs.bank[channel];
        MIDI_Event* temp;

        if(!patch || !(chan_mask & 1 << channel))
            continue;

        // Copy Patch Change Event
        temp = patch;
        patch = new MIDI_Event;
        patch->time = temp->time;
        patch->status = (uint8_t)(channel | (MIDI_STATUS_PROG_CHANGE << 4));
        patch->data[0] = temp->data[0];

        // Copy Volume
        if(vol && (vol->time > patch->time + PATCH_VOL_PAN_BIAS || vol->time < patch->time - PATCH_VOL_PAN_BIAS))
            vol = nullptr;

        temp = vol;
        vol = new MIDI_Event;
        vol->status = (uint8_t)(channel | (MIDI_STATUS_CONTROLLER << 4));
        vol->data[0] = 7;

        if(!temp)
            vol->data[1] = VolumeCurve[90];
        else
            vol->data[1] = temp->data[1];

        // Copy Bank
        if(bank && (bank->time > patch->time + PATCH_VOL_PAN_BIAS || bank->time < patch->time - PATCH_VOL_PAN_BIAS))
            bank = nullptr;

        temp = bank;

        bank = new MIDI_Event;
        bank->status = (uint8_t)(channel | (MIDI_STATUS_CONTROLLER << 4));

        if(!temp)
            bank->data[1] = 0;
        else
            bank->data[1] = temp->data[1];

        // Copy Pan
        if(pan && (pan->time > patch->time + PATCH_VOL_PAN_BIAS || pan->time < patch->time - PATCH_VOL_PAN_BIAS))
            pan = nullptr;

        temp = pan;
        pan = new MIDI_Event;
        pan->status = (uint8_t)(channel | (MIDI_STATUS_CONTROLLER << 4));
        pan->data[0] = 10;

        if(!temp)
            pan->data[1] = 64;
        else
            pan->data[1] = temp->data[1];

        vol->time = 0;
        pan->time = 0;
        patch->time = 0;
        bank->time = 0;

        bank->next = vol;
        vol->next = pan;
        pan->next = patch;

        patch->next = events;

        events = bank;
    }
    // Tempo of 500.000 ms per quarter note
    // XMIDI has a fixed rate of 120Hz so PPQN should then be set to 60
    auto* ev = new MIDI_Event;
    ev->status = 0xFF;
    ev->data[0] = 0x51;
    ev->buffer.resize(3);
    ev->buffer[0] = 0x07;
    ev->buffer[1] = 0xA1;
    ev->buffer[2] = 0x20;
    ev->next = events;
    events = ev;
}
} // namespace libsiedler2
