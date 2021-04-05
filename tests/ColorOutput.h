// Copyright (c) 2017 - 2020 Settlers Freaks (sf-team at siedler25.org)
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
