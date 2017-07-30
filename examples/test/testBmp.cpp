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
#include "ColorOutput.h"
#include "libsiedler2/src/ArchivInfo.h"
#include "libsiedler2/src/libsiedler2.h"
#include "libsiedler2/src/ArchivItem_Bitmap_Raw.h"
#include "libsiedler2/src/ArchivItem_Bitmap_Player.h"
#include "libsiedler2/src/ColorARGB.h"
#include <boost/filesystem.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/assign/std/vector.hpp>
#include <boost/foreach.hpp>
#include <algorithm>

struct Rect
{
    int x, y, w, h;
    Rect(){}
    Rect(int x, int y, int w, int h): x(x), y(y), w(w), h(h){}
    bool operator==(const Rect& rhs) const
    {
        return x == rhs.x && y == rhs.y && w == rhs.w && h == rhs.h;
    }
};

std::ostream& operator<<(std::ostream& os, const Rect& rect)
{
    return os << "(" << rect.x << ", " << rect.y << ", " << rect.w << ", " << rect.h << ")";
}

using namespace libsiedler2;
using namespace boost::assign;

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
        palette->get(inBufferPal[i]).toBGR(&inBuffer[i * 4]);
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
    std::vector<uint8_t> emptyBufferPal(outBufferPal);
    std::vector<uint8_t> emptyBuffer(outBuffer);
    // First create paletted bmp
    ArchivItem_Bitmap_Raw bmpPal;
    // Any NULL should make this fail
    BOOST_REQUIRE(!bmpPal.create(bw, bh, NULL, w, h, FORMAT_PALETTED, palette));
    BOOST_REQUIRE(!bmpPal.create(bw, bh, &inBufferPal[0], w, h, FORMAT_PALETTED, NULL));
    // These create transparent bitmaps
    BOOST_REQUIRE(bmpPal.create(bw, bh, &inBufferPal[0], 0, h, FORMAT_PALETTED, palette));
    BOOST_REQUIRE_EQUAL(bmpPal.getWidth(), bw);
    BOOST_REQUIRE_EQUAL(bmpPal.getHeight(), bh);
    BOOST_REQUIRE(bmpPal.create(bw, bh, &inBufferPal[0], w, 0, FORMAT_PALETTED, palette));
    BOOST_REQUIRE_EQUAL(bmpPal.getWidth(), bw);
    BOOST_REQUIRE_EQUAL(bmpPal.getHeight(), bh);
    // These create empty bitmaps
    BOOST_REQUIRE(bmpPal.create(0, bh, &inBufferPal[0], w, h, FORMAT_PALETTED, palette));
    BOOST_REQUIRE_EQUAL(bmpPal.getWidth(), 0u);
    BOOST_REQUIRE_EQUAL(bmpPal.getHeight(), 0u);
    BOOST_REQUIRE(bmpPal.create(bw, 0, &inBufferPal[0], w, h, FORMAT_PALETTED, palette));
    BOOST_REQUIRE_EQUAL(bmpPal.getWidth(), 0u);
    BOOST_REQUIRE_EQUAL(bmpPal.getHeight(), 0u);

    // Succeed
    BOOST_REQUIRE(bmpPal.create(bw, bh, &inBufferPal[0], w, h, FORMAT_PALETTED, palette));
    BOOST_REQUIRE_EQUAL(bmpPal.getWidth(), bw);
    BOOST_REQUIRE_EQUAL(bmpPal.getHeight(), bh);
    // Now write to paletted buffer
    // Fail on NULL
    BOOST_REQUIRE(!bmpPal.print(NULL, w, h, FORMAT_PALETTED));
    // Do nothing
    BOOST_REQUIRE(bmpPal.print(&outBufferPal[0], 0, h, FORMAT_PALETTED));
    RTTR_REQUIRE_EQUAL_COLLECTIONS(outBufferPal, emptyBufferPal);
    BOOST_REQUIRE(bmpPal.print(&outBufferPal[0], w, 0, FORMAT_PALETTED));
    RTTR_REQUIRE_EQUAL_COLLECTIONS(outBufferPal, emptyBufferPal);
    // Succeed
    BOOST_REQUIRE(bmpPal.print(&outBufferPal[0], w, h, FORMAT_PALETTED));
    RTTR_REQUIRE_EQUAL_COLLECTIONS(outBufferPal, inBufferPal);
    // Write to RGBA buffer
    BOOST_REQUIRE(bmpPal.print(&outBuffer[0], w, h, FORMAT_BGRA));
    RTTR_REQUIRE_EQUAL_COLLECTIONS(outBuffer, inBufferRGB);

    // Test partial write
    unsigned xStart = 2, xStartB = 3, partW = 4, yStart = 1, yStartB = 5, partH = 6;
    std::fill(outBufferPal.begin(), outBufferPal.end(), 42u);
    std::fill(outBuffer.begin(), outBuffer.end(), 42u);
    BOOST_REQUIRE(bmpPal.print(&outBufferPal[0], w, h, FORMAT_PALETTED, NULL, xStart, yStart, xStartB, yStartB, partW, partH));
    BOOST_REQUIRE(bmpPal.print(&outBuffer[0], w, h, FORMAT_BGRA, NULL, xStart, yStart, xStartB, yStartB, partW, partH));
    for(unsigned y = 0; y < h; y++)
    {
        for(unsigned x = 0; x < w; x++)
        {
            unsigned idxPal = x + y*w;
            unsigned idx = idxPal * 4;
            if(x < xStart || y < yStart || x >= xStart + partW || y >= yStart + partH)
            {
                BOOST_REQUIRE_EQUAL(outBufferPal[idxPal], 42u);
                BOOST_REQUIRE_EQUAL(ColorARGB::fromARGB(&outBuffer[idx]), ColorARGB(42, 42, 42, 42));
            } else
            {
                unsigned inBufferIdx = x - xStart + xStartB + (y - yStart + yStartB) * w;
                BOOST_REQUIRE_EQUAL(outBufferPal[idxPal], inBufferPal[inBufferIdx]);
                BOOST_REQUIRE_EQUAL(ColorARGB::fromARGB(&outBuffer[idx]), ColorARGB::fromARGB(&inBufferRGB[inBufferIdx * 4]));
            }
        }
    }

    // Test the same but create from BGRA buffer
    ArchivItem_Bitmap_Raw bmp;
    std::fill(outBufferPal.begin(), outBufferPal.end(), 42u);
    std::fill(outBuffer.begin(), outBuffer.end(), 42u);
    std::fill(outBufferPal.begin() + bh * w, outBufferPal.end(), 0u);
    std::fill(outBuffer.begin() + bh * w * 4, outBuffer.end(), 0u);
    // Succeed
    BOOST_REQUIRE(bmp.create(bw, bh, &inBuffer[0], w, h, FORMAT_BGRA));
    BOOST_REQUIRE_EQUAL(bmp.getWidth(), bw);
    BOOST_REQUIRE_EQUAL(bmp.getHeight(), bh);
    // Now write to paletted buffer
    // Succeed
    BOOST_REQUIRE(bmp.print(&outBufferPal[0], w, h, FORMAT_PALETTED, palette));
    RTTR_REQUIRE_EQUAL_COLLECTIONS(outBufferPal, inBufferPal);
    BOOST_REQUIRE(bmp.print(&outBuffer[0], w, h, FORMAT_BGRA));
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
    for(unsigned i = 0; i < inBuffer.size(); i += 4)
    {
        inBufferRGB[i] = rand();
        inBufferRGB[i + 1] = rand();
        inBufferRGB[i + 2] = rand();
        inBufferRGB[i + 3] = 0; // Alpha
    }
    // Create paletted
    ArchivItem_Bitmap_Raw bmpPal;
    ArchivItem_Bitmap_Player bmpPlayerPal;
    BOOST_REQUIRE(bmpPal.create(bw, bh, &inBufferPal[0], w, h, FORMAT_PALETTED, palette));
    BOOST_REQUIRE(bmpPlayerPal.create(bw, bh, &inBufferPal[0], w, h, FORMAT_PALETTED, palette));
    // Create BGRA
    ArchivItem_Bitmap_Raw bmp;
    ArchivItem_Bitmap_Player bmpPlayer;
    BOOST_REQUIRE(bmp.create(bw, bh, &inBuffer[0], w, h, FORMAT_BGRA));
    BOOST_REQUIRE(bmpPlayer.create(bw, bh, &inBuffer[0], w, h, FORMAT_BGRA, palette));
    // Writing should not change buffer
    std::vector<uint8_t> outBuffer(inBuffer.size(), 42);
    std::vector<uint8_t> outBufferCheck(outBuffer);
    BOOST_REQUIRE(bmpPal.print(&outBuffer[0], w, h, FORMAT_PALETTED));
    RTTR_REQUIRE_EQUAL_COLLECTIONS(outBuffer, outBufferCheck);
    BOOST_REQUIRE(bmpPlayerPal.print(&outBuffer[0], w, h, FORMAT_PALETTED));
    RTTR_REQUIRE_EQUAL_COLLECTIONS(outBuffer, outBufferCheck);

    BOOST_REQUIRE(bmpPal.print(&outBuffer[0], w, h, FORMAT_BGRA));
    RTTR_REQUIRE_EQUAL_COLLECTIONS(outBuffer, outBufferCheck);
    BOOST_REQUIRE(bmpPlayerPal.print(&outBuffer[0], w, h, FORMAT_BGRA));
    RTTR_REQUIRE_EQUAL_COLLECTIONS(outBuffer, outBufferCheck);

    BOOST_REQUIRE(bmp.print(&outBuffer[0], w, h, FORMAT_PALETTED, palette));
    RTTR_REQUIRE_EQUAL_COLLECTIONS(outBuffer, outBufferCheck);
    BOOST_REQUIRE(bmpPlayer.print(&outBuffer[0], w, h, FORMAT_PALETTED, palette));
    RTTR_REQUIRE_EQUAL_COLLECTIONS(outBuffer, outBufferCheck);

    BOOST_REQUIRE(bmp.print(&outBuffer[0], w, h, FORMAT_BGRA));
    RTTR_REQUIRE_EQUAL_COLLECTIONS(outBuffer, outBufferCheck);
    BOOST_REQUIRE(bmpPlayer.print(&outBuffer[0], w, h, FORMAT_BGRA));
    RTTR_REQUIRE_EQUAL_COLLECTIONS(outBuffer, outBufferCheck);
}

BOOST_AUTO_TEST_CASE(CreatePrintPlayerBitmapNoPlayer)
{
    unsigned w = 10, h = 14;
    // Use different size for bitmap (bigger width, smaller height)
    unsigned bw = w + 3, bh = h - 2;
    uint8_t playerClrStart = 200;
    std::vector<uint8_t> inBufferPal(w * h);
    for(unsigned i = 0; i < inBufferPal.size(); i++)
        inBufferPal[i] = (i + 5) % 256;
    BOOST_REQUIRE_LT(w * h, playerClrStart);
    // Buffer in (byte) BGRA format
    std::vector<uint8_t> inBuffer(inBufferPal.size() * 4u);
    for(unsigned i = 0; i < inBufferPal.size(); i++)
    {
        palette->get(inBufferPal[i]).toBGR(&inBuffer[i * 4]);
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
    std::vector<uint8_t> emptyBufferPal(outBufferPal);
    std::vector<uint8_t> emptyBuffer(outBuffer);

    // First create paletted bmp
    ArchivItem_Bitmap_Player bmpPal;
    // Any NULL should make this fail
    BOOST_REQUIRE(!bmpPal.create(bw, bh, NULL, w, h, FORMAT_PALETTED, palette, playerClrStart));
    BOOST_REQUIRE(!bmpPal.create(bw, bh, &inBufferPal[0], w, h, FORMAT_PALETTED, NULL, playerClrStart));
    // These create transparent bitmaps
    BOOST_REQUIRE(bmpPal.create(bw, bh, &inBufferPal[0], 0, h, FORMAT_PALETTED, palette, playerClrStart));
    BOOST_REQUIRE_EQUAL(bmpPal.getWidth(), bw);
    BOOST_REQUIRE_EQUAL(bmpPal.getHeight(), bh);
    BOOST_REQUIRE(bmpPal.create(bw, bh, &inBufferPal[0], w, 0, FORMAT_PALETTED, palette, playerClrStart));
    BOOST_REQUIRE_EQUAL(bmpPal.getWidth(), bw);
    BOOST_REQUIRE_EQUAL(bmpPal.getHeight(), bh);
    // These create empty bitmaps
    BOOST_REQUIRE(bmpPal.create(0, bh, &inBufferPal[0], w, h, FORMAT_PALETTED, palette, playerClrStart));
    BOOST_REQUIRE_EQUAL(bmpPal.getWidth(), 0u);
    BOOST_REQUIRE_EQUAL(bmpPal.getHeight(), 0u);
    BOOST_REQUIRE(bmpPal.create(bw, 0, &inBufferPal[0], w, h, FORMAT_PALETTED, palette, playerClrStart));
    BOOST_REQUIRE_EQUAL(bmpPal.getWidth(), 0u);
    BOOST_REQUIRE_EQUAL(bmpPal.getHeight(), 0u);
    // Succeed
    BOOST_REQUIRE(bmpPal.create(bw, bh, &inBufferPal[0], w, h, FORMAT_PALETTED, palette, playerClrStart));
    BOOST_REQUIRE_EQUAL(bmpPal.getWidth(), bw);
    BOOST_REQUIRE_EQUAL(bmpPal.getHeight(), bh);
    // Now write to paletted buffer
    // Fail on NULL
    BOOST_REQUIRE(!bmpPal.print(NULL, w, h, FORMAT_PALETTED, NULL, playerClrStart));
    BOOST_REQUIRE(bmpPal.print(&outBufferPal[0], 0, h, FORMAT_PALETTED, NULL, playerClrStart));
    RTTR_REQUIRE_EQUAL_COLLECTIONS(outBufferPal, emptyBufferPal);
    BOOST_REQUIRE(bmpPal.print(&outBufferPal[0], w, 0, FORMAT_PALETTED, NULL, playerClrStart));
    RTTR_REQUIRE_EQUAL_COLLECTIONS(outBufferPal, emptyBufferPal);
    // Succeed
    BOOST_REQUIRE(bmpPal.print(&outBufferPal[0], w, h, FORMAT_PALETTED, NULL, playerClrStart));
    RTTR_REQUIRE_EQUAL_COLLECTIONS(outBufferPal, inBufferPal);
    // Write to RGBA buffer
    BOOST_REQUIRE(bmpPal.print(&outBuffer[0], w, h, FORMAT_BGRA, NULL, playerClrStart));
    RTTR_REQUIRE_EQUAL_COLLECTIONS(outBuffer, inBufferRGB);

    // Test partial write
    unsigned xStart = 2, xStartB = 3, partW = 4, yStart = 1, yStartB = 5, partH = 6;
    std::fill(outBufferPal.begin(), outBufferPal.end(), 42u);
    std::fill(outBuffer.begin(), outBuffer.end(), 42u);
    BOOST_REQUIRE(bmpPal.print(&outBufferPal[0], w, h, FORMAT_PALETTED, NULL, playerClrStart, xStart, yStart, xStartB, yStartB, partW, partH));
    BOOST_REQUIRE(bmpPal.print(&outBuffer[0], w, h, FORMAT_BGRA, NULL, playerClrStart, xStart, yStart, xStartB, yStartB, partW, partH));
    for(unsigned y = 0; y < h; y++)
    {
        for(unsigned x = 0; x < w; x++)
        {
            unsigned idxPal = x + y*w;
            unsigned idx = idxPal * 4;
            if(x < xStart || y < yStart || x >= xStart + partW || y >= yStart + partH)
            {
                BOOST_REQUIRE_EQUAL(outBufferPal[idxPal], 42u);
                BOOST_REQUIRE_EQUAL(ColorARGB::fromARGB(&outBuffer[idx]), ColorARGB(42, 42, 42, 42));
            } else
            {
                unsigned inBufferIdx = x - xStart + xStartB + (y - yStart + yStartB) * w;
                BOOST_REQUIRE_EQUAL(outBufferPal[idxPal], inBufferPal[inBufferIdx]);
                BOOST_REQUIRE_EQUAL(ColorARGB::fromARGB(&outBuffer[idx]), ColorARGB::fromARGB(&inBufferRGB[inBufferIdx * 4]));
            }
        }
    }

    // Test the same but create from BGRA buffer
    ArchivItem_Bitmap_Player bmp;
    std::fill(outBufferPal.begin(), outBufferPal.end(), 42u);
    std::fill(outBuffer.begin(), outBuffer.end(), 42u);
    std::fill(outBufferPal.begin() + bh * w, outBufferPal.end(), 0u);
    std::fill(outBuffer.begin() + bh * w * 4, outBuffer.end(), 0u);
    BOOST_REQUIRE(bmp.create(bw, bh, &inBuffer[0], w, h, FORMAT_BGRA, palette, playerClrStart));
    BOOST_REQUIRE_EQUAL(bmp.getWidth(), bw);
    BOOST_REQUIRE_EQUAL(bmp.getHeight(), bh);

    BOOST_REQUIRE(bmp.print(&outBufferPal[0], w, h, FORMAT_PALETTED, NULL, playerClrStart));
    RTTR_REQUIRE_EQUAL_COLLECTIONS(outBufferPal, inBufferPal);
    BOOST_REQUIRE(bmp.print(&outBuffer[0], w, h, FORMAT_BGRA, NULL, playerClrStart));
    RTTR_REQUIRE_EQUAL_COLLECTIONS(outBuffer, inBuffer);
}

BOOST_AUTO_TEST_CASE(CreatePrintPlayerBitmap)
{
    unsigned w = 5, h = 4;
    // Use bigger size for bitmap
    unsigned bw = w + 2, bh = h + 6;
    // Original and target player color (start) idx
    uint8_t playerClrStart = 200, playerClrStart2 = 210;
    std::vector<uint8_t> inBufferPal(w * h, 10);
    std::vector<uint8_t> inBufferPal2(inBufferPal);
    // Set player colors
    for(unsigned i = 0; i < 4; i++)
    {
        inBufferPal[i] = playerClrStart + i;
        inBufferPal2[i] = playerClrStart2 + i;
    }
    // Buffer in (byte) BGRA format
    std::vector<uint8_t> inBuffer(inBufferPal.size() * 4u);
    for(unsigned i = 0; i < inBufferPal.size(); i++)
    {
        palette->get(inBufferPal[i]).toBGR(&inBuffer[i * 4]);
        inBuffer[i * 4 + 3] = 0xFF - i;
    }
    std::vector<uint8_t> inBuffer2(inBuffer);
    for(unsigned i = 0; i < 4; i++)
        palette->get(inBufferPal2[i]).toBGR(&inBuffer2[i * 4]);

    // Buffer with alpha = 0xFF
    std::vector<uint8_t> inBufferRGB(inBuffer);
    std::vector<uint8_t> inBufferRGB2(inBuffer2);
    for(unsigned i = 0; i < inBufferRGB.size(); i += 4)
        inBufferRGB[i + 3] = inBufferRGB2[i + 3] = 0xFF;
    std::vector<uint8_t> outBufferPal(inBufferPal.size(), 42);
    std::vector<uint8_t> outBuffer(inBuffer.size(), 42);

    // First create paletted bmp
    ArchivItem_Bitmap_Player bmpPal;
    BOOST_REQUIRE(bmpPal.create(bw, bh, &inBufferPal[0], w, h, FORMAT_PALETTED, palette, playerClrStart));
    // Now write to paletted buffer
    BOOST_REQUIRE(bmpPal.print(&outBufferPal[0], w, h, FORMAT_PALETTED, NULL, playerClrStart));
    RTTR_REQUIRE_EQUAL_COLLECTIONS(outBufferPal, inBufferPal);
    // Write to RGBA buffer
    BOOST_REQUIRE(bmpPal.print(&outBuffer[0], w, h, FORMAT_BGRA, NULL, playerClrStart));
    RTTR_REQUIRE_EQUAL_COLLECTIONS(outBuffer, inBufferRGB);
    std::fill(outBufferPal.begin(), outBufferPal.end(), 42);
    std::fill(outBuffer.begin(), outBuffer.end(), 42);
    // paletted buffer, 2nd player color (recolor)
    BOOST_REQUIRE(bmpPal.print(&outBufferPal[0], w, h, FORMAT_PALETTED, NULL, playerClrStart2));
    RTTR_REQUIRE_EQUAL_COLLECTIONS(outBufferPal, inBufferPal2);
    // Same for BGRA
    BOOST_REQUIRE(bmpPal.print(&outBuffer[0], w, h, FORMAT_BGRA, NULL, playerClrStart2));
    RTTR_REQUIRE_EQUAL_COLLECTIONS(outBuffer, inBufferRGB2);

    std::fill(outBufferPal.begin(), outBufferPal.end(), 42);
    std::fill(outBuffer.begin(), outBuffer.end(), 42);
    // Player colors only
    BOOST_REQUIRE(bmpPal.print(&outBufferPal[0], w, h, FORMAT_PALETTED, NULL, playerClrStart2, 0, 0, 0, 0, 0, 0, true));
    BOOST_REQUIRE(bmpPal.print(&outBuffer[0], w, h, FORMAT_BGRA, NULL, playerClrStart2, 0, 0, 0, 0, 0, 0, true));
    for(unsigned i = 0; i < 4; i++)
    {
        BOOST_REQUIRE_EQUAL(outBufferPal[i], inBufferPal2[i]);
        BOOST_REQUIRE_EQUAL(ColorARGB::fromARGB(&outBuffer[i*4]), ColorARGB::fromARGB(&inBufferRGB2[i*4]));
    }
    for(unsigned i = 4; i < outBufferPal.size(); i++)
    {
        BOOST_REQUIRE_EQUAL(outBufferPal[i], 42);
        BOOST_REQUIRE_EQUAL(ColorARGB::fromARGB(&outBuffer[i * 4]), ColorARGB(42, 42, 42, 42));
    }
    // Test the same but create from BGRA buffer
    std::fill(outBufferPal.begin(), outBufferPal.end(), 42);
    std::fill(outBuffer.begin(), outBuffer.end(), 42);
    ArchivItem_Bitmap_Player bmp;
    BOOST_REQUIRE(bmp.create(bw, bh, &inBuffer[0], w, h, FORMAT_BGRA, palette, playerClrStart));
    // Write to paletted
    BOOST_REQUIRE(bmp.print(&outBufferPal[0], w, h, FORMAT_PALETTED, NULL, playerClrStart));
    RTTR_REQUIRE_EQUAL_COLLECTIONS(outBufferPal, inBufferPal);
    // Write to BGRA
    BOOST_REQUIRE(bmp.print(&outBuffer[0], w, h, FORMAT_BGRA, NULL, playerClrStart));
    RTTR_REQUIRE_EQUAL_COLLECTIONS(outBuffer, inBuffer);

    std::fill(outBufferPal.begin(), outBufferPal.end(), 42);
    std::fill(outBuffer.begin(), outBuffer.end(), 42);
    // recolored paletted
    BOOST_REQUIRE(bmp.print(&outBufferPal[0], w, h, FORMAT_PALETTED, NULL, playerClrStart2));
    RTTR_REQUIRE_EQUAL_COLLECTIONS(outBufferPal, inBufferPal2);
    // Same for BGRA
    BOOST_REQUIRE(bmp.print(&outBuffer[0], w, h, FORMAT_BGRA, NULL, playerClrStart2));
    RTTR_REQUIRE_EQUAL_COLLECTIONS(outBuffer, inBuffer2);

    std::fill(outBufferPal.begin(), outBufferPal.end(), 42);
    std::fill(outBuffer.begin(), outBuffer.end(), 42);
    // Player colors only
    BOOST_REQUIRE(bmp.print(&outBufferPal[0], w, h, FORMAT_PALETTED, NULL, playerClrStart2, 0, 0, 0, 0, 0, 0, true));
    BOOST_REQUIRE(bmp.print(&outBuffer[0], w, h, FORMAT_BGRA, NULL, playerClrStart2, 0, 0, 0, 0, 0, 0, true));
    for(unsigned i = 0; i < 4; i++)
    {
        BOOST_REQUIRE_EQUAL(outBufferPal[i], inBufferPal2[i]);
        BOOST_REQUIRE_EQUAL(ColorARGB::fromARGB(&outBuffer[i * 4]), ColorARGB::fromARGB(&inBuffer2[i * 4]));
    }
    for(unsigned i = 4; i < outBufferPal.size(); i++)
    {
        BOOST_REQUIRE_EQUAL(outBufferPal[i], 42);
        BOOST_REQUIRE_EQUAL(ColorARGB::fromARGB(&outBuffer[i * 4]), ColorARGB(42, 42, 42, 42));
    }
}

BOOST_AUTO_TEST_CASE(GetVisibleArea)
{
    unsigned w = 7, h = 8;
    // Use bigger size for bitmap
    unsigned bw = w + 2, bh = h + 6;
    std::vector<uint8_t> inBufferPal(w * h, TRANSPARENT_INDEX);
    ArchivItem_Bitmap_Player bmp;
    ArchivItem_Bitmap_Raw bmpRaw;
    {
        Rect vis;
        // Test empty bmp
        BOOST_REQUIRE(bmp.create(bw, bh, &inBufferPal[0], w, h, FORMAT_PALETTED, palette));
        bmp.getVisibleArea(vis.x, vis.y, vis.w, vis.h);
        BOOST_REQUIRE_EQUAL(vis, Rect(0, 0, 0, 0));
        Rect vis2;
        // Test empty bmp
        BOOST_REQUIRE(bmpRaw.create(bw, bh, &inBufferPal[0], w, h, FORMAT_PALETTED, palette));
        bmp.getVisibleArea(vis2.x, vis2.y, vis2.w, vis2.h);
        BOOST_REQUIRE_EQUAL(vis2, Rect(0, 0, 0, 0));
    }

    std::vector<Rect> testRects;
    //                  left-top          top               right-top           
    testRects += Rect(0, 0, 1, 1), Rect(2, 0, 1, 1), Rect(w - 1, 0, 1, 1),
        //left            middle            right           
        Rect(0, 3, 1, 1), Rect(3, 2, 1, 1), Rect(w - 1, 3, 1, 1),
        //left-bottom     bottom-middle     bottom-right           
        Rect(0, h - 1, 1, 1), Rect(3, h - 1, 1, 1), Rect(w - 1, h - 1, 1, 1),
    // doublePixelRects;
        Rect(0, 0, 2, 1), Rect(0, 0, 1, 2), Rect(2, 0, 1, 3), Rect(2, 0, 3, 1), Rect(w - 3, 0, 3, 1),
        Rect(2, 0, 1, 3), Rect(2, 0, 3, 1), Rect(3, 2, 3, 1), Rect(3, 2, 1, 3),
        Rect(0, h - 3, 2, 1), Rect(0, h - 3, 1, 3), Rect(3, h - 3, 3, 1), Rect(3, h - 3, 1, 3), Rect(w - 4, h - 3, 4, 3),
        Rect(0, 0, w, h);

    BOOST_FOREACH(const Rect& rect, testRects)
    {
        // Test for non-player-color (127) and player color (128)
        for(unsigned i = 127; i < 129; i++)
        {
            inBufferPal[rect.x + rect.y * w] = i;
            inBufferPal[rect.x + rect.w - 1 + (rect.y + rect.h - 1) * w] = i;
            BOOST_REQUIRE(bmp.create(bw, bh, &inBufferPal[0], w, h, FORMAT_PALETTED, palette));
            Rect vis;
            bmp.getVisibleArea(vis.x, vis.y, vis.w, vis.h);
            BOOST_REQUIRE_EQUAL(vis, rect);

            BOOST_REQUIRE(bmpRaw.create(bw, bh, &inBufferPal[0], w, h, FORMAT_PALETTED, palette));
            Rect visRaw;
            bmp.getVisibleArea(visRaw.x, visRaw.y, visRaw.w, visRaw.h);
            BOOST_REQUIRE_EQUAL(visRaw, rect);

            inBufferPal[rect.x + rect.y * w] = TRANSPARENT_INDEX;
            inBufferPal[rect.x + rect.w - 1 + (rect.y + rect.h - 1) * w] = TRANSPARENT_INDEX;

            // Buffer in (byte) BGRA format
            std::vector<uint8_t> inBuffer(inBufferPal.size() * 4u, 0);
            BOOST_REQUIRE(bmp.print(&inBuffer[0], w, h, FORMAT_BGRA));
            ArchivItem_Bitmap_Player bmp2;
            BOOST_REQUIRE(bmp2.create(bw, bh, &inBuffer[0], w, h, FORMAT_BGRA, palette));
            Rect vis2;
            bmp2.getVisibleArea(vis2.x, vis2.y, vis2.w, vis2.h);
            BOOST_REQUIRE_EQUAL(vis2, rect);

            BOOST_REQUIRE(bmpRaw.create(bw, bh, &inBuffer[0], w, h, FORMAT_BGRA, palette));
            Rect visRaw2;
            bmp.getVisibleArea(visRaw2.x, visRaw2.y, visRaw2.w, visRaw2.h);
            BOOST_REQUIRE_EQUAL(visRaw2, rect);
            inBufferPal[rect.x + rect.y * w] = TRANSPARENT_INDEX;
        }
    }
}

BOOST_AUTO_TEST_SUITE_END()
