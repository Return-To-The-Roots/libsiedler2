// Copyright (c) 2017 - 2017 Settlers Freaks (sf-team at siedler25.org)
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

#include "config.h"
#include "cmpFiles.h"
#include "LoadPalette.h"
#include "libsiedler2/src/ArchivInfo.h"
#include "libsiedler2/src/libsiedler2.h"
#include "libsiedler2/src/ArchivItem_Bitmap_Raw.h"
#include "libsiedler2/src/ColorARGB.h"
#include <boost/filesystem.hpp>
#include <boost/test/unit_test.hpp>
#include <algorithm>

using namespace libsiedler2;

namespace libsiedler2
{
    std::ostream& operator<<(std::ostream& os, ColorARGB clr)
    {
        return os << "(" << std::hex << clr.clrValue << ")";
    }
}

BOOST_FIXTURE_TEST_SUITE(Bitmaps, LoadPalette)

BOOST_AUTO_TEST_CASE(ReadWritePlayerBitmap)
{
    std::string bmpPath = "testFiles/bmpPlayer.lst";
    std::string bmpOutPath = testOutputPath + "/bmp.lst";
    BOOST_REQUIRE(bfs::exists(bmpPath));
    ArchivInfo bmp;
    BOOST_REQUIRE_EQUAL(Load(bmpPath, bmp, palette), 0);
    BOOST_REQUIRE_EQUAL(Write(bmpOutPath, bmp, palette), 0);
    BOOST_REQUIRE(testFilesEqual(bmpOutPath, bmpPath));
}

BOOST_AUTO_TEST_CASE(ReadWriteRawBitmap)
{
    std::string bmpPath = "testFiles/bmpRaw.lst";
    std::string bmpOutPath = testOutputPath + "/bmp.lst";
    BOOST_REQUIRE(bfs::exists(bmpPath));
    ArchivInfo bmp;
    BOOST_REQUIRE_EQUAL(Load(bmpPath, bmp, palette), 0);
    BOOST_REQUIRE_EQUAL(Write(bmpOutPath, bmp, palette), 0);
    BOOST_REQUIRE(testFilesEqual(bmpOutPath, bmpPath));
}

BOOST_AUTO_TEST_CASE(ReadWriteShadowBitmap)
{
    std::string bmpPath = "testFiles/bmpShadow.lst";
    std::string bmpOutPath = testOutputPath + "/bmp.lst";
    BOOST_REQUIRE(bfs::exists(bmpPath));
    ArchivInfo bmp;
    BOOST_REQUIRE_EQUAL(Load(bmpPath, bmp, palette), 0);
    BOOST_REQUIRE_EQUAL(Write(bmpOutPath, bmp, palette), 0);
    BOOST_REQUIRE(testFilesEqual(bmpOutPath, bmpPath));
}

BOOST_AUTO_TEST_CASE(ReadWriteRLEBitmap)
{
    std::string bmpPath = "testFiles/bmpRLE.lst";
    std::string bmpOutPath = testOutputPath + "/bmp.lst";
    BOOST_REQUIRE(bfs::exists(bmpPath));
    ArchivInfo bmp;
    BOOST_REQUIRE_EQUAL(Load(bmpPath, bmp, palette), 0);
    BOOST_REQUIRE_EQUAL(Write(bmpOutPath, bmp, palette), 0);
    BOOST_REQUIRE(testFilesEqual(bmpOutPath, bmpPath));
}

BOOST_AUTO_TEST_CASE(ReadWriteBmp)
{
    std::string bmpPath = "testFiles/logo.bmp";
    std::string bmpOutPath = testOutputPath + "/outLogo.bmp";
    BOOST_REQUIRE(bfs::exists(bmpPath));
    ArchivInfo bmp;
    BOOST_REQUIRE_EQUAL(Load(bmpPath, bmp), 0);
    BOOST_REQUIRE_EQUAL(Write(bmpOutPath, bmp), 0);
    BOOST_REQUIRE(testFilesEqual(bmpOutPath, bmpPath));
}

BOOST_AUTO_TEST_CASE(ReadWritePalettedBmp)
{
    std::string bmpPath = "testFiles/pal.bmp";
    std::string bmpOutPath = testOutputPath + "/outPal.bmp";
    BOOST_REQUIRE(bfs::exists(bmpPath));
    ArchivInfo bmp;
    BOOST_REQUIRE_EQUAL(Load(bmpPath, bmp), 0);
    BOOST_REQUIRE_EQUAL(Write(bmpOutPath, bmp), 0);
    BOOST_REQUIRE(testFilesEqual(bmpOutPath, bmpPath));
}

BOOST_AUTO_TEST_CASE(CreatePrintBitmap)
{
    unsigned w = 10, h = 14;
    // Use different size for bitmap (bigger width, smaller height)
    unsigned bw = w + 3, bh = h - 2;
    std::vector<uint8_t> inBufferPal(w * h);
    for(unsigned i = 0; i < inBufferPal.size(); i++)
        inBufferPal[i] = (i + 5) % 256;
    // Buffer in (byte) BGRA format
    std::vector<uint8_t> inBuffer(inBufferPal.size() * 4u);
    for(unsigned i = 0; i < inBufferPal.size(); i++)
    {
        palette->get(inBufferPal[i], inBuffer[i * 4 + 2], inBuffer[i * 4 + 1], inBuffer[i * 4 + 0]);
        inBuffer[i * 4 + 3] = 0xFF - i;
    }
    // Buffer with alpha = 0xFF
    std::vector<uint8_t> inBufferRGB(inBuffer);
    for(unsigned i = 0; i < inBufferRGB.size(); i += 4)
        inBufferRGB[i + 3] = 0xFF;
    // This part is not in the bmp
    std::fill(inBufferPal.begin() + bh * w, inBufferPal.end(), 0u);
    std::fill(inBuffer.begin() + bh * w * 4, inBuffer.end(), 0u);
    std::fill(inBufferRGB.begin() + bh * w * 4, inBufferRGB.end(), 0u);
    std::vector<uint8_t> outBufferPal(inBufferPal.size(), 42);
    std::vector<uint8_t> outBuffer(inBuffer.size(), 42);
    std::fill(outBufferPal.begin() + bh * w, outBufferPal.end(), 0u);
    std::fill(outBuffer.begin() + bh * w * 4, outBuffer.end(), 0u);
    // First create paletted bmp
    ArchivItem_Bitmap_Raw bmpPal;
    // Any 0 should make this fail
    BOOST_REQUIRE_NE(bmpPal.create(0, bh, &inBufferPal[0], w, h, FORMAT_PALETTED, palette), 0);
    BOOST_REQUIRE_NE(bmpPal.create(bw, 0, &inBufferPal[0], w, h, FORMAT_PALETTED, palette), 0);
    BOOST_REQUIRE_NE(bmpPal.create(bw, bh, NULL, w, h, FORMAT_PALETTED, palette), 0);
    BOOST_REQUIRE_NE(bmpPal.create(bw, bh, &inBufferPal[0], 0, h, FORMAT_PALETTED, palette), 0);
    BOOST_REQUIRE_NE(bmpPal.create(bw, bh, &inBufferPal[0], w, 0, FORMAT_PALETTED, palette), 0);
    BOOST_REQUIRE_NE(bmpPal.create(bw, bh, &inBufferPal[0], w, h, FORMAT_PALETTED, NULL), 0);
    // Succeed
    BOOST_REQUIRE_EQUAL(bmpPal.create(bw, bh, &inBufferPal[0], w, h, FORMAT_PALETTED, palette), 0);
    BOOST_REQUIRE_EQUAL(bmpPal.getWidth(), bw);
    BOOST_REQUIRE_EQUAL(bmpPal.getHeight(), bh);
    // Now write to paletted buffer
    // Fail on 0
    BOOST_REQUIRE_NE(bmpPal.print(NULL, w, h, FORMAT_PALETTED), 0);
    BOOST_REQUIRE_NE(bmpPal.print(&outBufferPal[0], 0, h, FORMAT_PALETTED), 0);
    BOOST_REQUIRE_NE(bmpPal.print(&outBufferPal[0], w, 0, FORMAT_PALETTED), 0);
    // Succeed
    BOOST_REQUIRE_EQUAL(bmpPal.print(&outBufferPal[0], w, h, FORMAT_PALETTED), 0);
    RTTR_REQUIRE_EQUAL_COLLECTIONS(outBufferPal, inBufferPal);
    // Write to RGBA buffer
    BOOST_REQUIRE_EQUAL(bmpPal.print(&outBuffer[0], w, h, FORMAT_RGBA), 0);
    RTTR_REQUIRE_EQUAL_COLLECTIONS(outBuffer, inBufferRGB);

    // Test partial write
    unsigned xStart = 2, xStartB = 3, partW = 4, yStart = 1, yStartB = 5, partH = 6;
    std::fill(outBufferPal.begin(), outBufferPal.end(), 42u);
    std::fill(outBuffer.begin(), outBuffer.end(), 42u);
    BOOST_REQUIRE_EQUAL(bmpPal.print(&outBufferPal[0], w, h, FORMAT_PALETTED, NULL, xStart, yStart, xStartB, yStartB, partW, partH), 0);
    BOOST_REQUIRE_EQUAL(bmpPal.print(&outBuffer[0], w, h, FORMAT_RGBA, NULL, xStart, yStart, xStartB, yStartB, partW, partH), 0);
    for(unsigned y = 0; y < h; y++)
    {
        for(unsigned x = 0; x < w; x++)
        {
            unsigned idxPal = x + y*w;
            unsigned idx = idxPal * 4;
            if(x < xStart || y < yStart || x >= xStart + partW || y >= yStart + partH)
            {
                BOOST_REQUIRE_EQUAL(outBufferPal[idxPal], 42u);
                BOOST_REQUIRE_EQUAL(ColorARGB::fromABGR(&outBuffer[idx]), ColorARGB(42, 42, 42, 42));
            } else
            {
                unsigned inBufferIdx = x - xStart + xStartB + (y - yStart + yStartB) * w;
                BOOST_REQUIRE_EQUAL(outBufferPal[idxPal], inBufferPal[inBufferIdx]);
                BOOST_REQUIRE_EQUAL(ColorARGB::fromABGR(&outBuffer[idx]), ColorARGB::fromABGR(&inBufferRGB[inBufferIdx * 4]));
            }
        }
    }

    // Test the same but create from ARGB buffer
    ArchivItem_Bitmap_Raw bmp;
    std::fill(outBufferPal.begin(), outBufferPal.end(), 42u);
    std::fill(outBuffer.begin(), outBuffer.end(), 42u);
    std::fill(outBufferPal.begin() + bh * w, outBufferPal.end(), 0u);
    std::fill(outBuffer.begin() + bh * w * 4, outBuffer.end(), 0u);
    // Succeed
    BOOST_REQUIRE_EQUAL(bmp.create(bw, bh, &inBuffer[0], w, h, FORMAT_RGBA), 0);
    BOOST_REQUIRE_EQUAL(bmp.getWidth(), bw);
    BOOST_REQUIRE_EQUAL(bmp.getHeight(), bh);
    // Now write to paletted buffer
    // Succeed
    BOOST_REQUIRE_EQUAL(bmp.print(&outBufferPal[0], w, h, FORMAT_PALETTED, palette), 0);
    RTTR_REQUIRE_EQUAL_COLLECTIONS(outBufferPal, inBufferPal);
    BOOST_REQUIRE_EQUAL(bmp.print(&outBuffer[0], w, h, FORMAT_RGBA), 0);
    RTTR_REQUIRE_EQUAL_COLLECTIONS(outBuffer, inBuffer);
}

BOOST_AUTO_TEST_CASE(TransparentTex)
{
    unsigned w = 10, h = 14;
    // Use different size for bitmap (bigger width, smaller height)
    unsigned bw = w + 3, bh = h - 2;
    std::vector<uint8_t> inBufferPal(w * h, TRANSPARENT_INDEX);
    // Buffer in (byte) BGRA format
    std::vector<uint8_t> inBuffer(inBufferPal.size() * 4u);
    // With all 0
    std::vector<uint8_t> inBufferRGB(inBufferPal.size() * 4u, 0u);
    for(unsigned i = 0; i < inBuffer.size(); i+=4)
    {
        inBufferRGB[i] = rand();
        inBufferRGB[i + 1] = rand();
        inBufferRGB[i + 2] = rand();
        inBufferRGB[i + 3] = 0; // Alpha
    }
    // Create paletted
    ArchivItem_Bitmap_Raw bmpPal;
    BOOST_REQUIRE_EQUAL(bmpPal.create(bw, bh, &inBufferPal[0], w, h, FORMAT_PALETTED, palette), 0);
    // Create ARGB
    ArchivItem_Bitmap_Raw bmp;
    BOOST_REQUIRE_EQUAL(bmp.create(bw, bh, &inBuffer[0], w, h, FORMAT_RGBA), 0);
    // Writing should not change buffer
    std::vector<uint8_t> outBuffer(inBuffer.size(), 42);
    std::vector<uint8_t> outBufferCheck(outBuffer);
    BOOST_REQUIRE_EQUAL(bmpPal.print(&outBuffer[0], w, h, FORMAT_PALETTED), 0);
    RTTR_REQUIRE_EQUAL_COLLECTIONS(outBuffer, outBufferCheck);
    BOOST_REQUIRE_EQUAL(bmpPal.print(&outBuffer[0], w, h, FORMAT_RGBA), 0);
    RTTR_REQUIRE_EQUAL_COLLECTIONS(outBuffer, outBufferCheck);
    BOOST_REQUIRE_EQUAL(bmp.print(&outBuffer[0], w, h, FORMAT_PALETTED, palette), 0);
    RTTR_REQUIRE_EQUAL_COLLECTIONS(outBuffer, outBufferCheck);
    BOOST_REQUIRE_EQUAL(bmp.print(&outBuffer[0], w, h, FORMAT_RGBA), 0);
    RTTR_REQUIRE_EQUAL_COLLECTIONS(outBuffer, outBufferCheck);
}

BOOST_AUTO_TEST_SUITE_END()
