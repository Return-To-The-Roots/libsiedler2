// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

namespace libsiedler2 {
/// Direction that image figures are facing/walking
enum class ImgDir
{
    // Order as in S2 arrays: Right first, then clockwise
    E,
    SE,
    SW,
    W,
    NW,
    NE
};
} // namespace libsiedler2
