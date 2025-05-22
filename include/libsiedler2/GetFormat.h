// Copyright (C) 2005 - 2025 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "enumTypes.h"

namespace libsiedler2::traits {
/// Return the texture/buffer format of the given type as the value member
template<class T_PixelBuffer>
struct GetFormat;

template<class T_PixelBuffer>
constexpr TextureFormat GetFormat_v = GetFormat<T_PixelBuffer>::value;
} // namespace libsiedler2::traits
