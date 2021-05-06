// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

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
