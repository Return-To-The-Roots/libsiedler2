// Copyright (c) 2005 - 2020 Settlers Freaks (sf-team at siedler25.org)
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

#pragma once

#include <boost/endian/arithmetic.hpp>
#include <array>

namespace libsiedler2 {
struct WAV_Header
{
    std::array<char, 4> RIFF_ID; /// "RIFF"
    boost::endian::little_uint32_t fileSize;
    std::array<char, 4> WAVE_ID;            /// "WAVE"
    std::array<char, 4> fmt_ID;             // "fmt "
    boost::endian::little_uint32_t fmtSize; // 16
    boost::endian::little_uint16_t fmtTag;  // 1=PCM
    boost::endian::little_uint16_t numChannels;
    boost::endian::little_uint32_t samplesPerSec, bytesPerSec;
    boost::endian::little_uint16_t frameSize, bitsPerSample;
    std::array<char, 4> data_ID; // "data"
    boost::endian::little_uint32_t dataSize;
};

static_assert(sizeof(WAV_Header) == 44u, "Invalid padding detected");
} // namespace libsiedler2
