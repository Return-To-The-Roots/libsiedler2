// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "ArchivItem_Palette.h"
#include "PixelBuffer.h"
#include <cstdint>

namespace libsiedler2 {

class PixelBufferPaletted : public PixelBuffer<uint8_t>
{
public:
    PixelBufferPaletted() = default;
    PixelBufferPaletted(uint16_t width, uint16_t height, uint8_t defValue = ArchivItem_Palette::DEFAULT_TRANSPARENT_IDX)
        : PixelBuffer<uint8_t>(width, height, defValue)
    {}
};

namespace traits {
    template<>
    struct GetFormat<PixelBufferPaletted>
    {
        static constexpr TextureFormat value = TextureFormat::Paletted;
    };
} // namespace traits

} // namespace libsiedler2
