// Copyright (c) 2005 - 2020 Settlers Freaks (sf-team at siedler25.org)
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

#include <cstdint>

namespace libsiedler2 {
struct Rect
{
    uint16_t x, y, w, h;
};

/// Move and resize the rect so it fits into [(0,0), (w,h)]
Rect clipRect(Rect rect, uint16_t w, uint16_t h);

/// Copy a pixel buffer (reference) to another one potentially converting the format
/// using only the pixels in srcRect and writing only to dstRect
/// Special cases:
///     - Transparent pixels are skipped
///     - Paletted pixels are not translated to other palette if both are paletted
///     - Source and destination rects can be (partially) outside the region (clipping applied)
template<class T_Src, class T_Dst>
void CopyPixelBuffer(const T_Src& src, T_Dst& dst, Rect srcRect, Rect dstRect);
} // namespace libsiedler2
