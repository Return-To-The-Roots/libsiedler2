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

#ifndef BmpHeader_h__
#define BmpHeader_h__

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

#endif // BmpHeader_h__
