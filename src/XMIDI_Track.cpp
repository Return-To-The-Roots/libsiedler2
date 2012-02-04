// $Id: XMIDI_Track.cpp 7817 2012-02-04 08:18:00Z FloSoft $
//
// Copyright (c) 2005 - 2011 Settlers Freaks (sf-team at siedler25.org)
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

///////////////////////////////////////////////////////////////////////////////
// Header
#include "main.h"
#include "XMIDI_Track.h"

#include "MIDI_Track.h"
#include "GammaTable.h"

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#if defined _WIN32 && defined _DEBUG && defined _MSC_VER
	#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

static const int PATCH_VOL_PAN_BIAS = 5;
static GammaTable<unsigned char> VolumeCurve(128);

XMIDI_Track::XMIDI_Track(MIDI_Track *track) : track(track)
{
	position = 0;
	current = NULL;
	events = NULL;
	event_count = 0,

	memset(bank127, 0, sizeof(bank127));
}

XMIDI_Track::~XMIDI_Track()
{
	if(event_count > 0)
	{
		MIDI_Event *next;
		current = events;
		next = events;
		while((current = next))
		{
			next = current->next;
			if ((current->status>>4) == 0xF)
				delete[] (current->buffer);
			delete current;
		}
	}
}

int XMIDI_Track::Convert(void)
{
	ConvertTrackToList();
	ConvertListToMTrk();

	return 0;
}

int XMIDI_Track::ConvertTrackToList()
{
	int            time = 0; // 120th of a second
	unsigned int   data;
	int            end = 0;
	unsigned int   status = 0;
	int            play_size = 3;
	int            retval = 0;

	position = 0;

	first_state fs;
	memset(&fs, 0, sizeof(first_state));

	while (!end && position < track->xmid_length)
	{
		GetVLQ2(data);
		time += data;
		status = (unsigned int)track->xmid_data[position++];

		switch (status >> 4)
		{
		case MIDI_STATUS_NOTE_ON:
			retval |= 1 << (status & 0xF);
			ConvertNote(time, status, play_size);
			break;

		case MIDI_STATUS_NOTE_OFF:
			ConvertNote(time, status, 2);
			break;

			// 2 byte data
		case MIDI_STATUS_AFTERTOUCH:
		case MIDI_STATUS_CONTROLLER:
		case MIDI_STATUS_PITCH_WHEEL:
			ConvertEvent(time, status, 2, fs);
			break;

			// 1 byte data
		case MIDI_STATUS_PROG_CHANGE:
		case MIDI_STATUS_PRESSURE:
			ConvertEvent(time, status, 1, fs);
			break;

		case MIDI_STATUS_SYSEX:
			if (status == 0xFF)
			{
				unsigned int pos = position;
				unsigned int data = track->xmid_data[position++];

				if (data == 0x2F)
					end = 1;
				else if (data == 0x51)
				{
					GetVLQ(data);
					position += data;
					break;
				}

				position = pos;
			}
			ConvertSystemMessage(time, status);
			break;
		}

	}
	current = events;

	ApplyFirstState(fs, retval);

	return retval;
}

int XMIDI_Track::GetVLQ(unsigned int &quant)
{
	int i;
	quant = 0;
	unsigned char data = 0;

	for (i = 0; i < 4; i++)
	{
		data = track->xmid_data[position++];
		quant <<= 7;
		quant |= data & 0x7F;

		if (!(data & 0x80))
		{
			i++;
			break;
		}
	}
	return i;
}

int XMIDI_Track::GetVLQ2(unsigned int &quant)
{
	int i;
	quant = 0;
	unsigned char data = 0;

	for (i = 0; i < 4; i++)
	{
		data = track->xmid_data[position++];
		if (data & 0x80)
		{
			position--;
			break;
		}
		quant += data;
	}
	return i;
}

int XMIDI_Track::PutVLQ(unsigned int value, bool write, unsigned int pos /*= 0*/)
{
	int buffer;
	int i = 1;
	buffer = value & 0x7F;
	while (value >>= 7)
	{
		buffer <<= 8;
		buffer |= ((value & 0x7F) | 0x80);
		i++;
	}

	if (!write)
		return i;

	for (int j = 0; j < i; j++)
	{
		track->mid_data[pos++] = buffer & 0xFF;
		buffer >>= 8;
	}

	return i;
}

int XMIDI_Track::ConvertNote(const int time, const unsigned char status, const int size)
{
	unsigned int delta = 0;
	int data;

	data = track->xmid_data[position++];

	CreateNewEvent(time);
	current->status = status;

	current->data[0] = data;
	current->data[1] = track->xmid_data[position++];

	// Volume modify the note on's, only if converting
	if ((current->status >> 4) == MIDI_STATUS_NOTE_ON && current->data[1])
		current->data[1] = VolumeCurve[current->data[1]];

	if (size == 2)
		return 2;

	// XMI Note On handling

	int i = GetVLQ(delta);
	current->duration = delta;

	// This is an optimization
	MIDI_Event *prev = current;

	// Create a note off
	CreateNewEvent(time+delta);

	current->status = status;
	current->data[0] = data;
	current->data[1] = 0;

	// Optimization
	current = prev;

	return i + 2;
}

int XMIDI_Track::ConvertEvent(const int time, const unsigned char status, const int size, first_state &fs)
{
	//   Uint32 delta=0;
	int data;

	data = track->xmid_data[position++];

	// Bank changes are handled here
	if ((status >> 4) == 0xB && data == 0)
	{
		data = track->xmid_data[position++];

		bank127[status&0xF] = false;

		return 2;
	}

	// Disable patch changes on Track 10 is doing a conversion
	else if ((status >> 4) == 0xC && (status&0xF) == 9)
	{
		return size;
	}

	CreateNewEvent(time);
	current->status = status;

	current->data[0] = data;

	// Check for patch change, and update fs if req
	if ((status >> 4) == 0xC)
	{
		if (!fs.patch[status&0xF] || fs.patch[status&0xF]->time > time)
			fs.patch[status&0xF] = current;
	}

	// Controllers
	else if ((status >> 4) == 0xB)
	{
		// Volume
		if (current->data[0] == 7)
		{
			if (!fs.vol[status&0xF] || fs.vol[status&0xF]->time > time)
				fs.vol[status&0xF] = current;
		}
		// Pan
		else if (current->data[0] == 10)
		{
			if (!fs.pan[status&0xF] || fs.pan[status&0xF]->time > time)
				fs.pan[status&0xF] = current;
		}
	}

	if (size == 1)
		return 1;

	current->data[1] = track->xmid_data[position++];

	// Volume modify the volume controller, only if converting
	if ((current->status >> 4) == MIDI_STATUS_CONTROLLER && current->data[0] == 7)
		current->data[1] = VolumeCurve[current->data[1]];

	return 2;
}

int XMIDI_Track::ConvertSystemMessage(const int time, const unsigned char status)
{
	int i = 0;

	CreateNewEvent(time);
	current->status = status;

	// Handling of Meta events
	if (status == 0xFF)
	{
		current->data[0] = track->xmid_data[position++];
		i++;
	}

	i += GetVLQ(current->len);

	if (!current->len)
	{
		current->buffer = NULL;
		return i;
	}

	current->buffer = new unsigned char[current->len];
	memset(current->buffer, 0, current->len);

	memcpy(current->buffer, &track->xmid_data[position], current->len);
	position += current->len;

	return i + current->len;
}

void XMIDI_Track::CreateNewEvent(int time)
{
	if (!events)
	{
		events = current = new MIDI_Event;
		memset(events, 0, sizeof(MIDI_Event));
		if (time > 0)
			current->time = time;
		event_count++;
		return;
	}

	if (time < 0 || events->time > time)
	{
		MIDI_Event *event = new MIDI_Event;
		memset(event, 0, sizeof(MIDI_Event));
		event->next = events;
		events = current = event;
		event_count++;
		return;
	}

	if (!current || current->time > time)
		current = events;

	while (current->next)
	{
		if (current->next->time > time)
		{
			MIDI_Event *event = new MIDI_Event;
			memset(event, 0, sizeof(MIDI_Event));

			event->next = current->next;
			current->next = event;
			current = event;
			current->time = time;
			event_count++;
			return;
		}

		current = current->next;
	}

	current->next = new MIDI_Event;
	memset(current->next, 0, sizeof(MIDI_Event));
	current = current->next;
	current->time = time;

	event_count++;
}


unsigned int XMIDI_Track::ConvertListToMTrk()
{
	int time = 0;
	int lasttime = 0;
	MIDI_Event *event;
	unsigned int delta;
	unsigned char last_status = 0;
	unsigned int j;

	track->clearMid();
	unsigned int mid_length = 8;

	// Größe des Tracks ermitteln
	for (event = events; event; event = event->next)
	{
		if (event->status == 0xFF && event->data[0] == 0x2f)
		{
			lasttime = event->time;
			break;
		}

		delta = (event->time - time);
		time = event->time;

		mid_length += PutVLQ(delta, false);

		if ((event->status != last_status) || (event->status >= 0xF0))
			mid_length++;

		last_status = event->status;

		switch (event->status >> 4)
		{
			// 2 bytes data
			// Note off, Note on, Aftertouch, Controller and Pitch Wheel
		case 0x8: case 0x9: case 0xA: case 0xB: case 0xE:
			mid_length += 2;
			break;

			// 1 bytes data
			// Program Change and Channel Pressure
		case 0xC: case 0xD:
			mid_length++;
			break;

			// Variable length
			// SysEx
		case 0xF:
			if (event->status == 0xFF)
				mid_length++;

			mid_length += PutVLQ(event->len, false);

			if (event->len)
			{
				for (j = 0; j < event->len; j++)
					mid_length++;
			}
			break;
		}
	}

	// Write out end of stream marker
	if (lasttime > time)
		mid_length += PutVLQ(lasttime-time, false);
	else
		mid_length += PutVLQ(0, false);

	mid_length += 2;

	mid_length += PutVLQ(0, false);


	// Speicher anlegen
	track->allocMid(mid_length);

	// nun Stream in die das Array schreiben
	unsigned int i = 14;
	time = 0;
	lasttime = 0;
	delta = 0;
	last_status = 0;

	// Header schreiben
	memcpy(&track->mid_data[i], "MTrk", 4);
	i += 4;

	unsigned int length = mid_length-8;
#if BYTE_ORDER == LITTLE_ENDIAN
	length = libendian::swap_ui(length);
#endif
	memcpy(&track->mid_data[i], (unsigned char *)&length, 4);
	i += 4;

	for (event = events; event; event = event->next)
	{
		if (event->status == 0xFF && event->data[0] == 0x2f)
		{
			lasttime = event->time;
			break;
		}

		delta = (event->time - time);
		time = event->time;

		i += PutVLQ(delta, true, i);

		if ((event->status != last_status) || (event->status >= 0xF0))
			track->mid_data[i++] = event->status;

		last_status = event->status;

		switch (event->status >> 4)
		{
			// 2 bytes data
			// Note off, Note on, Aftertouch, Controller and Pitch Wheel
		case 0x8: case 0x9: case 0xA: case 0xB: case 0xE:
			track->mid_data[i++] = event->data[0];
			track->mid_data[i++] = event->data[1];
			break;

			// 1 bytes data
			// Program Change and Channel Pressure
		case 0xC: case 0xD:
			track->mid_data[i++] = event->data[0];
			break;

			// Variable length
			// SysEx
		case 0xF:
			if (event->status == 0xFF)
				track->mid_data[i++] = event->data[0];

			i += PutVLQ(event->len, true, i);

			if (event->len)
			{
				for (j = 0; j < event->len; j++)
					track->mid_data[i++] = event->buffer[j];
			}
			break;
		}
	}

	// Write out end of stream marker
	if (lasttime > time)
		i += PutVLQ(lasttime-time, true, i);
	else
		i += PutVLQ(0, true, i);

	track->mid_data[i++] = 0xFF;
	track->mid_data[i++] = 0x2F;

	i += PutVLQ(0, true, i);

	return i;
}

void XMIDI_Track::ApplyFirstState(first_state &fs, int chan_mask)
{
	for (unsigned char channel = 0; channel < 16; channel++)
	{
		MIDI_Event *patch = fs.patch[channel];
		MIDI_Event *vol = fs.vol[channel];
		MIDI_Event *pan = fs.pan[channel];
		MIDI_Event *bank = fs.bank[channel];
		MIDI_Event *temp;

		if (!patch || !(chan_mask & 1 << channel))
			continue;

		// Copy Patch Change Event
		temp = patch;
		patch = new MIDI_Event;
		memset(patch, 0, sizeof(MIDI_Event));
		patch->time = temp->time;
		patch->status = (unsigned char)(channel|(MIDI_STATUS_PROG_CHANGE << 4));
		patch->data[0] = temp->data[0];

		// Copy Volume
		if (vol && (vol->time > patch->time+PATCH_VOL_PAN_BIAS ||
			vol->time < patch->time-PATCH_VOL_PAN_BIAS))
			vol = NULL;

		temp = vol;
		vol = new MIDI_Event;
		memset(vol, 0, sizeof(MIDI_Event));
		vol->status = (unsigned char)(channel|(MIDI_STATUS_CONTROLLER << 4));
		vol->data[0] = 7;

		if (!temp)
			vol->data[1] = VolumeCurve[90];
		else
			vol->data[1] = temp->data[1];


		// Copy Bank
		if (bank && (bank->time > patch->time+PATCH_VOL_PAN_BIAS ||
			bank->time < patch->time-PATCH_VOL_PAN_BIAS))
			bank = NULL;

		temp = bank;

		bank = new MIDI_Event;
		memset(bank, 0, sizeof(MIDI_Event));
		bank->status = (unsigned char)(channel|(MIDI_STATUS_CONTROLLER << 4));

		if (!temp)
			bank->data[1] = 0;
		else
			bank->data[1] = temp->data[1];

		// Copy Pan
		if (pan && (pan->time > patch->time+PATCH_VOL_PAN_BIAS ||
			pan->time < patch->time-PATCH_VOL_PAN_BIAS))
			pan = NULL;

		temp = pan;
		pan = new MIDI_Event;
		memset(pan, 0, sizeof(MIDI_Event));
		pan->status = (unsigned char)(channel|(MIDI_STATUS_CONTROLLER << 4));
		pan->data[0] = 10;

		if (!temp)
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
}
