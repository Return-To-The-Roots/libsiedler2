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

#ifndef WAV_Header_h__
#define WAV_Header_h__

#pragma once

#include <boost/endian/arithmetic.hpp>

namespace libsiedler2 {
struct WAV_Header
{
    char RIFF_ID[4]; /// "RIFF"
    boost::endian::little_uint32_t fileSize;
    char WAVE_ID[4];                        /// "WAVE"
    char fmt_ID[4];                         // "fmt "
    boost::endian::little_uint32_t fmtSize; // 16
    boost::endian::little_uint16_t fmtTag;  // 1=PCM
    boost::endian::little_uint16_t numChannels;
    boost::endian::little_uint32_t samplesPerSec, bytesPerSec;
    boost::endian::little_uint16_t frameSize, bitsPerSample;
    char data_ID[4]; // "data"
    boost::endian::little_uint32_t dataSize;
};

static_assert(sizeof(WAV_Header) == 44u, "Invalid padding detected");
} // namespace libsiedler2

#endif // WAV_Header_h__
