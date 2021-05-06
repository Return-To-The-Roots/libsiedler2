// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "libsiedler2/ColorBGRA.h"
#include "libsiedler2/ColorRGB.h"
#include <iostream>

namespace libsiedler2 {
// LCOV_EXCL_START
inline std::ostream& operator<<(std::ostream& os, const ColorRGB& clr)
{
    return os << "Color(" << unsigned(clr.r) << ", " << unsigned(clr.g) << ", " << unsigned(clr.b) << ")";
}
inline std::ostream& operator<<(std::ostream& os, const ColorBGRA& clr)
{
    return os << "Color(" << unsigned(clr.getAlpha()) << ", " << unsigned(clr.getRed()) << ", "
              << unsigned(clr.getGreen()) << ", " << unsigned(clr.getBlue()) << ")";
}
// LCOV_EXCL_STOP
} // namespace libsiedler2
