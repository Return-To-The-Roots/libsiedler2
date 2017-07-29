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
#ifndef XMIDI_TRACK_H_INCLUDED
#define XMIDI_TRACK_H_INCLUDED

#pragma once

#include <vector>
#include <iosfwd>
#include <stdint.h>

namespace libsiedler2
{
    
	class XMIDI_Track
	{
	    public:
            struct Timbre
            {
                uint8_t patch, bank;
            };
            XMIDI_Track();
	        ~XMIDI_Track();
	
	        bool read(std::istream& file, size_t length);
	        void clear();

            const std::vector<uint8_t>& getData() const { return data_; }
            std::vector<Timbre>& getTimbres() { return timbres_; }
            const std::vector<Timbre>& getTimbres() const { return timbres_; }

	    protected:
	        std::vector<uint8_t> data_;
            std::vector<Timbre> timbres_;
	};
}

#endif // XMIDI_TRACK_H_INCLUDED
