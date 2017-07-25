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
#ifndef MIDI_TRACK_H_INCLUDED
#define MIDI_TRACK_H_INCLUDED

#pragma once

#include <vector>
#include <iosfwd>
#include <stdint.h>

namespace libsiedler2
{
	class MIDI_Track
	{
	    public:
	        MIDI_Track();
	        ~MIDI_Track();
	
	        int readXMid(std::istream& file, size_t length);
	        void clearXMid();
	
	        int readMid(std::istream& file, size_t length);
	        void clearMid();
	
	        int XMid2Mid();
	        const uint8_t* getMid(bool withheader = false) const;
	        uint32_t getMidLength(bool withheader = false) const;
	
	        friend class XMIDI_Track;
	
	    protected:
	        std::vector<uint8_t> xmid_data;
	        std::vector<uint8_t> mid_data;
	};
}

#endif // !MIDI_TRACK_H_INCLUDED
