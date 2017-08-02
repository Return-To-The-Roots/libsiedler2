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

#pragma once

#ifndef fileFormatHelpers_h__
#define fileFormatHelpers_h__

#include <boost/static_assert.hpp>
#include <cstring>

/// Return true iff lhs identifies a given chunk. Example: if(isChunk(header, "RIFF")) ...
template<size_t T_length, size_t T_lengthIdStr>
inline bool isChunk(const char (&lhs)[T_length], const char (&chunkId)[T_lengthIdStr])
{
    BOOST_STATIC_ASSERT_MSG(T_length + 1 >= T_lengthIdStr, "Chunk id is not long enough");
    return memcmp(lhs, chunkId, T_lengthIdStr - 1) == 0;
}

/// Write the given chunkId into the target. Example: setChunkId(header, "RIFF");
template<size_t T_length, size_t T_lengthIdStr>
inline void setChunkId(char (&target)[T_length], const char (&chunkId)[T_lengthIdStr])
{
    BOOST_STATIC_ASSERT_MSG(T_length + 1 >= T_lengthIdStr, "Chunk id is not long enough");
    memcpy(target, chunkId, T_lengthIdStr - 1);
}

#endif // fileFormatHelpers_h__
