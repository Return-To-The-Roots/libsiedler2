// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

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
