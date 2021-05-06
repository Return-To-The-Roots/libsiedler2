// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

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
