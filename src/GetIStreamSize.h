// Copyright (c) 2005 - 2019 Settlers Freaks (sf-team at siedler25.org)
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
#ifndef GET_I_STREAM_SIZE_H__
#define GET_I_STREAM_SIZE_H__

#include <cstddef>

namespace libsiedler2 {
template<class T_Stream>
inline size_t getIStreamSize(T_Stream& stream)
{
    stream.seekg(0, T_Stream::end);
    const auto pos = stream.tellg();
    stream.seekg(0, T_Stream::beg);
    return (pos < 0) ? 0 : size_t(pos);
}

} // namespace libsiedler2

#endif // GET_I_STREAM_SIZE_H__
