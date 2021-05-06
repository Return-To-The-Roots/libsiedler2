// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <array>
#include <cstring>

/// Return true iff lhs identifies a given chunk. Example: if(isChunk(header, "RIFF")) ...
template<size_t T_length, size_t T_lengthIdStr>
inline bool isChunk(const std::array<char, T_length>& lhs, const char (&chunkId)[T_lengthIdStr])
{
    static_assert(T_length + 1 >= T_lengthIdStr, "Chunk id is not long enough");
    return std::memcmp(lhs.data(), chunkId, T_lengthIdStr - 1) == 0;
}

/// Write the given chunkId into the target. Example: setChunkId(header, "RIFF");
template<size_t T_length, size_t T_lengthIdStr>
inline void setChunkId(std::array<char, T_length>& target, const char (&chunkId)[T_lengthIdStr])
{
    static_assert(T_length + 1 >= T_lengthIdStr, "Chunk id is not long enough");
    std::memcpy(target.data(), chunkId, T_lengthIdStr - 1);
}
