// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <boost/endian/arithmetic.hpp>

namespace libsiedler2 {
struct BmpFileHeader
{
    std::array<char, 2> header; /// "BM"
    boost::endian::little_uint32_t fileSize;
    boost::endian::little_uint32_t reserved;    /// App specific
    boost::endian::little_uint32_t pixelOffset; /// Start adress of the pixel data
};

struct BitmapInfoHeader
{
    boost::endian::little_uint32_t headerSize; /// 40 for this
    boost::endian::little_int32_t width;
    boost::endian::little_int32_t height;
    boost::endian::little_int16_t planes;       /// 1
    boost::endian::little_int16_t bpp;          /// Bits per pixel
    boost::endian::little_uint32_t compression; /// 0: BI_RGB
    boost::endian::little_uint32_t size;        /// Image data size or 0 for BIG_RGB
    boost::endian::little_int32_t xppm;
    boost::endian::little_int32_t yppm;
    boost::endian::little_int32_t clrused;
    boost::endian::little_int32_t clrimp;
};
} // namespace libsiedler2
