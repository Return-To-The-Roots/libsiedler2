// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "ColorBGRA.h"
#include "GetFormat.h"
#include "PixelBuffer.h"
#include "enumTypes.h"

namespace libsiedler2 {

class PixelBufferBGRA : public PixelBuffer<ColorBGRA>
{
public:
    PixelBufferBGRA() = default;
    PixelBufferBGRA(uint16_t width, uint16_t height, ColorBGRA defValue = ColorBGRA())
        : PixelBuffer<ColorBGRA>(width, height, defValue)
    {}
};

namespace traits {
    template<>
    struct GetFormat<PixelBufferBGRA>
    {
        static constexpr TextureFormat value = TextureFormat::BGRA;
    };
} // namespace traits

} // namespace libsiedler2
