// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <boost/endian/arithmetic.hpp>

namespace libsiedler2 {
enum class BmpCompression : uint32_t
{
    RGB,      /// RGB without compression
    RLE8,     /// 8 bit index with RLE compression
    RLE4,     /// 4 bit index with RLE compression
    Bitfield, /// 16 or 32 bit fields without compression
    Jpeg,
    Png /// compressed in this image format
};

struct BmpFileHeader
{
    std::array<char, 2> header; /// "BM"
    boost::endian::little_uint32_t fileSize;
    boost::endian::little_uint32_t reserved;    /// App specific
    boost::endian::little_uint32_t pixelOffset; /// Start address of the pixel data
};

struct BitmapInfoHeader
{
    boost::endian::little_uint32_t headerSize; /// 40 for this
    boost::endian::little_int32_t width;
    boost::endian::little_int32_t height;
    boost::endian::little_int16_t planes;       /// 1
    boost::endian::little_int16_t bpp;          /// Bits per pixel
    boost::endian::little_uint32_t compression; /// See @BmpCompression
    boost::endian::little_uint32_t size;        /// Image data size
    boost::endian::little_int32_t xppm;
    boost::endian::little_int32_t yppm;
    boost::endian::little_int32_t clrused;
    boost::endian::little_int32_t clrimp;
};

struct BitmapInfoHeaderV5 : BitmapInfoHeader
{
    struct CIEXYZ
    {
        boost::endian::little_uint32_t x, y, z;
    };
    struct CIEXYZTriple
    {
        CIEXYZ red, green, blue;
    };
    boost::endian::little_uint32_t redMask, greenMask, blueMask, alphaMask;
    boost::endian::little_uint32_t csType; /// Color space
    CIEXYZTriple endpoints;                /// Endpoints in logical color space
    boost::endian::little_uint32_t gammaRed, gammaGreen, gammaBlue;
    boost::endian::little_uint32_t intent;            /// Render intent
    boost::endian::little_uint32_t profileDataOffset; /// Start address of the profile data
    boost::endian::little_uint32_t profileSize;
    uint32_t reserved;
};

} // namespace libsiedler2
