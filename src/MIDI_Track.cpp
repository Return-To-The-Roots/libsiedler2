// Copyright (c) 2005 - 2017 Settlers Freaks (sf-team at siedler25.org)
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

#include "libSiedler2Defines.h" // IWYU pragma: keep
#include "MIDI_Track.h"
#include "ErrorCodes.h"
#include <iostream>
#include <limits>

namespace libsiedler2
{
	MIDI_Track::MIDI_Track()
	{}

    MIDI_Track::MIDI_Track(const std::vector<uint8_t>& data): mid_data(data)
    {}
	
	MIDI_Track::~MIDI_Track()
	{}
	
    int MIDI_Track::read(std::istream& file, size_t length)
	{
	    clear();
	    if(length == 0)
	        return ErrorCode::NONE;
	
        mid_data.resize(length);
	    if(!file.read(reinterpret_cast<char*>(&mid_data[0]), length))
	        return ErrorCode::UNEXPECTED_EOF;
	    return ErrorCode::NONE;
	}
	
	void MIDI_Track::clear()
	{
	    mid_data.clear();
	}
	
	const uint8_t* MIDI_Track::getMid() const
	{
	    if(mid_data.empty())
	        return NULL;
        return &mid_data[0];
	}
	
	uint32_t MIDI_Track::getMidLength() const
	{
	    assert(mid_data.size() < std::numeric_limits<uint32_t>::max());
	    return static_cast<uint32_t>(mid_data.size());
	}
}
