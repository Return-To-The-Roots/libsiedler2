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

#include "ColorOutput.h"
#include "LoadPalette.h"
#include "cmpFiles.h"
#include "config.h"
#include "libsiedler2/Archiv.h"
#include "libsiedler2/ArchivItem_Bitmap_Player.h"
#include "libsiedler2/ArchivItem_Bitmap_Raw.h"
#include "libsiedler2/ColorARGB.h"
#include "libsiedler2/ErrorCodes.h"
#include "libsiedler2/IAllocator.h"
#include "libsiedler2/PixelBufferARGB.h"
#include "libsiedler2/PixelBufferPaletted.h"
#include "libsiedler2/libsiedler2.h"
#include <boost/array.hpp>
#include <boost/assign/std/vector.hpp>
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <boost/test/unit_test.hpp>
#include <algorithm>

struct Rect
{
    int x, y;
    unsigned w, h;
    Rect() {}
    Rect(int x, int y, unsigned w, unsigned h) : x(x), y(y), w(w), h(h) {}
    bool operator==(const Rect& rhs) const { return x == rhs.x && y == rhs.y && w == rhs.w && h == rhs.h; }
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
    Archiv bmp;
    BOOST_REQUIRE(testLoad(0, bmpPath, bmp, palette));
    BOOST_REQUIRE_EQUAL(Write(bmpOutPath, bmp, palette), 0);
    BOOST_REQUIRE(testFilesEqual(bmpOutPath, bmpPath));
}

BOOST_AUTO_TEST_CASE(ReadWriteRawBitmap)
{
    std::string bmpPath = "testFiles/bmpRaw.lst";
    std::string bmpOutPath = testOutputPath + "/bmp.lst";
    BOOST_REQUIRE(bfs::exists(bmpPath));
    Archiv bmp;
    BOOST_REQUIRE(testLoad(0, bmpPath, bmp, palette));
    BOOST_REQUIRE_EQUAL(Write(bmpOutPath, bmp, palette), 0);
    BOOST_REQUIRE(testFilesEqual(bmpOutPath, bmpPath));
}

BOOST_AUTO_TEST_CASE(ReadWriteShadowBitmap)
{
    std::string bmpPath = "testFiles/bmpShadow.lst";
    std::string bmpOutPath = testOutputPath + "/bmp.lst";
    BOOST_REQUIRE(bfs::exists(bmpPath));
    Archiv bmp;
    BOOST_REQUIRE(testLoad(0, bmpPath, bmp, palette));
    BOOST_REQUIRE_EQUAL(Write(bmpOutPath, bmp, palette), 0);
    BOOST_REQUIRE(testFilesEqual(bmpOutPath, bmpPath));
    // Make maximum size by alternating between transparent and non-transparent pixels
    PixelBufferPaletted buffer(10, 13, palette->getTransparentIdx());
    for(unsigned y = 0; y < buffer.getHeight(); y++)
    {
        // Start with transparent
        for(unsigned x = 1; x < buffer.getWidth(); x += 2)
            buffer.set(x, y, 1);
    }
    baseArchivItem_Bitmap* shadowBmp = dynamic_cast<baseArchivItem_Bitmap*>(getAllocator().create(BOBTYPE_BITMAP_SHADOW));
    shadowBmp->create(buffer, palette);
    bmp.clear();
    bmp.push(shadowBmp);
    BOOST_REQUIRE_EQUAL(Write(bmpOutPath, bmp, palette), 0);
}

BOOST_AUTO_TEST_CASE(ReadWriteRLEBitmap)
{
    std::string bmpPath = "testFiles/bmpRLE.lst";
    std::string bmpOutPath = testOutputPath + "/bmp.lst";
    BOOST_REQUIRE(bfs::exists(bmpPath));
    Archiv bmp;
    BOOST_REQUIRE(testLoad(0, bmpPath, bmp, palette));
    BOOST_REQUIRE_EQUAL(Write(bmpOutPath, bmp, palette), 0);
    BOOST_REQUIRE(testFilesEqual(bmpOutPath, bmpPath));
    // Make maximum size by alternating between transparent and non-transparent pixels
    PixelBufferPaletted buffer(10, 13, palette->getTransparentIdx());
    for(unsigned y = 0; y < buffer.getHeight(); y++)
    {
        // Start with transparent
        for(unsigned x = 1; x < buffer.getWidth(); x += 2)
            buffer.set(x, y, 1);
    }
    baseArchivItem_Bitmap* shadowBmp = dynamic_cast<baseArchivItem_Bitmap*>(getAllocator().create(BOBTYPE_BITMAP_RLE));
    shadowBmp->create(buffer, palette);
    bmp.clear();
    bmp.push(shadowBmp);
    BOOST_REQUIRE_EQUAL(Write(bmpOutPath, bmp, palette), 0);
}

BOOST_AUTO_TEST_CASE(ReadWriteBmp)
{
    std::string bmpPath = "testFiles/logo.bmp";
    std::string bmpOutPath = testOutputPath + "/outLogo.bmp";
    BOOST_REQUIRE(bfs::exists(bmpPath));
    Archiv bmp;
    BOOST_REQUIRE(testLoad(0, bmpPath, bmp));
    BOOST_REQUIRE_EQUAL(Write(bmpOutPath, bmp), 0);
    BOOST_REQUIRE(testFilesEqual(bmpOutPath, bmpPath));
}

BOOST_AUTO_TEST_CASE(ReadWritePalettedBmp)
{
    std::string bmpPath = "testFiles/pal.bmp";
    std::string bmpOutPath = testOutputPath + "/outPal.bmp";
    BOOST_REQUIRE(bfs::exists(bmpPath));
    Archiv archiv;
    BOOST_REQUIRE(testLoad(0, bmpPath, archiv));
    ArchivItem_BitmapBase* bmp = dynamic_cast<ArchivItem_BitmapBase*>(archiv[0]);
    BOOST_REQUIRE(bmp);
    BOOST_REQUIRE(bmp->getPalette());
    // We want to write as paletted again
    BOOST_REQUIRE_EQUAL(bmp->convertFormat(FORMAT_PALETTED), 0);
    BOOST_REQUIRE_EQUAL(Write(bmpOutPath, archiv), 0);
    BOOST_REQUIRE(testFilesEqual(bmpOutPath, bmpPath));
}

struct FormatSetter
{
    const TextureFormat orig;
    FormatSetter(TextureFormat newFmt) : orig(setGlobalTextureFormat(newFmt)) {}
    ~FormatSetter() { setGlobalTextureFormat(orig); }
};

// Defines some bitmap files for testing and their properties
struct TestBitmaps
{
    struct Info
    {
        std::string filename;
        bool isPaletted, containsPalette, supportsBoth;
        Info() {}
        Info(const std::string& filename, bool isPaletted, bool containsPalette, bool supportsBoth = false)
            : filename(filename), isPaletted(isPaletted), containsPalette(containsPalette), supportsBoth(supportsBoth)
        {}
    };
    std::vector<Info> files;
    TestBitmaps()
    {
        files.push_back(Info("bmpPlayer.lst", true, false));             // Player bitmap
        files.push_back(Info("bmpRaw.lst", true, false));                // Raw bitmap in lst
        files.push_back(Info("bmpShadow.lst", true, false));             // Shadow bitmap
        files.push_back(Info("bmpRLE.lst", true, false));                // RLE bitmap
        files.push_back(Info("rawWithPalClrs.bmp", false, false, true)); // Raw bitmap
        files.push_back(Info("pal.bmp", true, true, true));              // Paletted raw bitmap
        files.push_back(Info("test.lbm", true, true));                   // Paletted bitmap with palette included in lbm
    }
};

ArchivItem_BitmapBase* getFirstBitmap(Archiv& archiv)
{
    for(unsigned j = 0; j < archiv.size(); j++)
    {
        ArchivItem_BitmapBase* bmp = dynamic_cast<ArchivItem_BitmapBase*>(archiv[j]);
        if(bmp)
            return bmp;
    }
    return NULL;
}

BOOST_AUTO_TEST_CASE(DefaultTextureFormatAndPalette)
{
    const TestBitmaps testFiles;
    // Try all formats with all possible bmp types.
    boost::array<TextureFormat, 3> fmts = {{FORMAT_ORIGINAL, FORMAT_PALETTED, FORMAT_BGRA}};
    BOOST_FOREACH(TextureFormat curFmt, fmts)
    {
        FormatSetter fmtSetter(curFmt);
        BOOST_FOREACH(const TestBitmaps::Info& testFile, testFiles.files)
        {
            Archiv archiv;
            int ec = Load("testFiles/" + testFile.filename, archiv, palette);
            BOOST_REQUIRE_MESSAGE(ec == 0, "Error " << getErrorString(ec) << " loading " << testFile.filename);
            const ArchivItem_BitmapBase* bmp = getFirstBitmap(archiv);
            BOOST_REQUIRE(bmp);
            if(curFmt != FORMAT_ORIGINAL)
                BOOST_REQUIRE_EQUAL(bmp->getFormat(), curFmt);
            else if(testFile.isPaletted)
                BOOST_REQUIRE_EQUAL(bmp->getFormat(), FORMAT_PALETTED);
            else
                BOOST_REQUIRE_EQUAL(bmp->getFormat(), FORMAT_BGRA);
        }
    }
}

BOOST_AUTO_TEST_CASE(PaletteUsageOnLoad)
{
    // Rules:
    // If the file contains a palette, it is stored in the bitmap and used.
    // If the file does not contain a palette the passed palette is used, but only stored with the bitmap if it is paletted.
    const TestBitmaps testFiles;
    ArchivItem_Palette emptyPal;

    // Try all formats with all possible bmp types.
    boost::array<TextureFormat, 3> fmts = {{FORMAT_ORIGINAL, FORMAT_PALETTED, FORMAT_BGRA}};
    BOOST_FOREACH(TextureFormat curFmt, fmts)
    {
        FormatSetter fmtSetter(curFmt);
        BOOST_FOREACH(const TestBitmaps::Info& testFile, testFiles.files)
        {
            Archiv archiv;
            if((curFmt == FORMAT_PALETTED && !testFile.containsPalette) || (testFile.isPaletted && !testFile.containsPalette))
            {
                // Paletted files need a palette. For conversion to paletted we also need one
                BOOST_REQUIRE(testLoad(ErrorCode::PALETTE_MISSING, "testFiles/" + testFile.filename, archiv));
            } else
            {
                // Non paletted file or palette contained
                BOOST_REQUIRE(testLoad(0, "testFiles/" + testFile.filename, archiv));
                const ArchivItem_BitmapBase* bmp = getFirstBitmap(archiv);
                // For paletted bitmaps we must have a palette, the others must not have one
                if(bmp->getFormat() == FORMAT_PALETTED || testFile.containsPalette)
                    BOOST_REQUIRE_MESSAGE(getFirstBitmap(archiv)->getPalette(),
                                          "No palette found for " << testFile.filename << " fmt=" << curFmt);
                else
                    BOOST_REQUIRE_MESSAGE(!getFirstBitmap(archiv)->getPalette(),
                                          "Palette found for " << testFile.filename << " fmt=" << curFmt);
            }
            const ArchivItem_Palette* usedPalette;
            // Use the empty pal to detect if it was used for conversion
            if(testFile.containsPalette)
                usedPalette = &emptyPal;
            else
                usedPalette = modPal; // Files are saved with palette, so use another one to detect difference
            archiv.clear();
            BOOST_REQUIRE(testLoad(0, "testFiles/" + testFile.filename, archiv, usedPalette));
            const ArchivItem_BitmapBase* bmp = getFirstBitmap(archiv);
            if(bmp->getFormat() == FORMAT_PALETTED)
            {
                // Paletted formats must have a palette
                BOOST_REQUIRE(bmp->getPalette());
                // If the file contains a palette, it has to be used, otherwise the default shall be used
                if(testFile.containsPalette)
                    BOOST_REQUIRE(!usedPalette->isEqual(*bmp->getPalette()));
                else
                    BOOST_REQUIRE(usedPalette->isEqual(*bmp->getPalette()));
            } else if(testFile.containsPalette)
            {
                // If the file contains a palette, it has to be used, otherwise no palette
                BOOST_REQUIRE(bmp->getPalette());
                BOOST_REQUIRE(!usedPalette->isEqual(*bmp->getPalette()));
                // If the empty palette is used, we will only find transparent and black pixels. Check that this did not happen
                bool clrFound = false;
                for(unsigned y = 0; y < bmp->getHeight(); y++)
                {
                    for(unsigned x = 0; x < bmp->getWidth(); x++)
                    {
                        ColorARGB clr = bmp->getPixel(x, y);
                        if(clr.getAlpha() != 0 && clr.getBlue() != 0)
                        {
                            clrFound = true;
                            break;
                        }
                    }
                }
                BOOST_REQUIRE(clrFound);
            } else
                BOOST_REQUIRE_MESSAGE(!bmp->getPalette(), "Palette found for " << testFile.filename << " fmt=" << curFmt);
        }
    }
}

BOOST_AUTO_TEST_CASE(PaletteUsageOnWrite)
{
    // Rules:
    // If the file format contains a palette and the bitmap contains one, it is used instead of the passed palette.
    // Otherwise the palette passed is used with the bitmaps palette as a fallback. It is an error not to pass a palette when conversion is
    // required unless the bitmap contains one. If the file format supports paletted and unpaletted images then the paletted format is used,
    // if(and only if) the bitmap is paletted or contains a palette.
    const TestBitmaps testFiles;
    ArchivItem_Palette emptyPal;

    // Try all formats with all possible bmp types.
    boost::array<TextureFormat, 3> fmts = {{FORMAT_ORIGINAL, FORMAT_PALETTED, FORMAT_BGRA}};
    BOOST_FOREACH(TextureFormat curFmt, fmts)
    {
        FormatSetter fmtSetter(curFmt);
        BOOST_FOREACH(const TestBitmaps::Info& testFile, testFiles.files)
        {
            Archiv archiv;
            std::string inFilepath = "testFiles/" + testFile.filename;
            std::string outFilepathRef = testOutputPath + "/reference_" + testFile.filename;
            std::string outFilepath = testOutputPath + "/" + testFile.filename;
            BOOST_REQUIRE(testLoad(0, inFilepath, archiv, palette));
            ArchivItem_BitmapBase* bmp = getFirstBitmap(archiv);
            // Not modified when writing with same palette
            BOOST_REQUIRE(testWrite(0, outFilepathRef, archiv, palette));
            if(testFile.supportsBoth && !testFile.isPaletted && bmp->getFormat() == FORMAT_PALETTED)
                BOOST_REQUIRE(!testFilesEqual(outFilepathRef, inFilepath)); // Stored as paletted
            else
                BOOST_REQUIRE(testFilesEqual(outFilepathRef, inFilepath));

            // a) no palette
            if(bmp->getFormat() == FORMAT_BGRA)
            {
                libsiedler2::ArchivItem_Palette* bmpPal = bmp->getPalette() ? bmp->getPalette()->clone() : NULL;
                bmp->removePalette();
                // If conversion is required -> error
                if(testFile.isPaletted && !testFile.supportsBoth)
                    BOOST_REQUIRE(testWrite(ErrorCode::PALETTE_MISSING, outFilepath, archiv));
                else
                {
                    BOOST_REQUIRE(testWrite(0, outFilepath, archiv));
                    if(testFile.supportsBoth && testFile.isPaletted)
                        BOOST_REQUIRE(!testFilesEqual(outFilepath, outFilepathRef)); // Stored as RGB
                    else
                        BOOST_REQUIRE(testFilesEqual(outFilepath, outFilepathRef));
                }
                BOOST_REQUIRE(testWrite(0, outFilepath, archiv, palette));
                if(testFile.supportsBoth && testFile.isPaletted)
                    BOOST_REQUIRE(!testFilesEqual(outFilepath, outFilepathRef)); // Still stored as RGB
                else
                    BOOST_REQUIRE(testFilesEqual(outFilepath, outFilepathRef));
                bmp->setPalette(bmpPal);
            }
            // b) use bitmaps palette if none passed
            if(!bmp->getPalette())
                bmp->setPaletteCopy(*palette);
            BOOST_REQUIRE(testWrite(0, outFilepath, archiv));
            if(testFile.supportsBoth && !testFile.isPaletted && bmp->getFormat() != FORMAT_PALETTED)
            {
                // Non-paletted file is written as paletted because it now contains a palette
                // If the format is paletted, then the reference is already paletted -> other branch
                BOOST_REQUIRE(!testFilesEqual(outFilepath, outFilepathRef));
                // Take the paletted as new reference
                bfs::copy_file(outFilepath, outFilepathRef, bfs::copy_option::overwrite_if_exists);
            } else
                BOOST_REQUIRE(testFilesEqual(outFilepath, outFilepathRef));
            // c) Passed palette is used unless file contains one (or may)
            // File cannot contain palette: bitmap pal = invalid (modPal)
            // File (may) contain palette: argPal = invalid (modPal)
            const ArchivItem_Palette* argPal = palette;
            if(testFile.containsPalette || testFile.supportsBoth)
                argPal = modPal;
            else
                bmp->setPaletteCopy(*modPal);
            BOOST_REQUIRE(testWrite(0, outFilepath, archiv, argPal));
            BOOST_REQUIRE(testFilesEqual(outFilepath, outFilepathRef));
        }
    }
}

BOOST_AUTO_TEST_CASE(PaletteAfterCreateBitmap)
{
    ArchivItem_Bitmap_Raw bmp;
    ArchivItem_Bitmap_Player bmpPl;
    unsigned w = 10, h = 14;
    PixelBufferPaletted palBuffer(w, h);
    PixelBufferARGB clrBuffer(w, h);
    // Paletted
    BOOST_REQUIRE_EQUAL(bmp.create(palBuffer, palette), 0);
    BOOST_REQUIRE_EQUAL(bmp.getFormat(), FORMAT_PALETTED);
    BOOST_REQUIRE(bmp.getPalette());
    BOOST_REQUIRE(bmp.getPalette()->isEqual(*palette));
    BOOST_REQUIRE_EQUAL(bmpPl.create(palBuffer, palette), 0);
    BOOST_REQUIRE_EQUAL(bmpPl.getFormat(), FORMAT_PALETTED);
    BOOST_REQUIRE(bmpPl.getPalette());
    BOOST_REQUIRE(bmpPl.getPalette()->isEqual(*palette));
    // ARGB
    BOOST_REQUIRE_EQUAL(bmp.create(clrBuffer), 0);
    BOOST_REQUIRE_EQUAL(bmp.getFormat(), FORMAT_BGRA);
    BOOST_REQUIRE(!bmp.getPalette());
    BOOST_REQUIRE_EQUAL(bmpPl.create(clrBuffer, palette), 0);
    BOOST_REQUIRE_EQUAL(bmpPl.getFormat(), FORMAT_BGRA);
    BOOST_REQUIRE(!bmpPl.getPalette());
}

BOOST_AUTO_TEST_CASE(PaletteUsageForPrint)
{
    ArchivItem_Bitmap_Raw bmp;
    ArchivItem_Bitmap_Player bmpPl;
    unsigned w = 10, h = 14;
    std::vector<uint8_t> inBuffer(w * h, modClr1 + 0); // + 0 to avoid a undefined reference error to the constant
    std::vector<uint8_t> inBufferPl(w * h, modClr2 + 0);
    // Swap colors to check palette usage
    // Paletted
    BOOST_REQUIRE_EQUAL(bmp.create(&inBuffer[0], w, h, FORMAT_PALETTED, palette), 0);
    BOOST_REQUIRE_EQUAL(bmpPl.create(&inBufferPl[0], w, h, FORMAT_PALETTED, palette), 0);
    // Can't remove palette
    BOOST_REQUIRE_THROW(bmp.removePalette(), std::runtime_error);
    BOOST_REQUIRE_THROW(bmp.setPalette(NULL), std::runtime_error);
    // Set mod pal to detect its use:
    bmp.setPaletteCopy(*modPal);
    bmpPl.setPaletteCopy(*modPal);
    // Paletted -> Paletted: No palette used, plain copy
    std::vector<uint8_t> outBuffer(inBuffer.size(), 42);
    BOOST_REQUIRE_EQUAL(bmp.print(&outBuffer[0], w, h, FORMAT_PALETTED), 0);
    RTTR_REQUIRE_EQUAL_COLLECTIONS(outBuffer, inBuffer);
    BOOST_REQUIRE_EQUAL(bmpPl.print(&outBuffer[0], w, h, FORMAT_PALETTED), 0);
    RTTR_REQUIRE_EQUAL_COLLECTIONS(outBuffer, inBufferPl);
    BOOST_REQUIRE_EQUAL(bmp.print(&outBuffer[0], w, h, FORMAT_PALETTED, modPal), 0);
    RTTR_REQUIRE_EQUAL_COLLECTIONS(outBuffer, inBuffer);
    BOOST_REQUIRE_EQUAL(bmpPl.print(&outBuffer[0], w, h, FORMAT_PALETTED, modPal), 0);
    RTTR_REQUIRE_EQUAL_COLLECTIONS(outBuffer, inBufferPl);

    // Paletted -> ARGB: Prefer contained
    bmp.setPaletteCopy(*palette);
    bmpPl.setPaletteCopy(*palette);
    outBuffer.resize(inBuffer.size() * 4, 42);
    std::vector<uint8_t> outBuffer2(outBuffer.size(), 42);
    BOOST_REQUIRE_EQUAL(bmp.print(&outBuffer[0], w, h, FORMAT_PALETTED), 0);
    BOOST_REQUIRE_EQUAL(bmp.print(&outBuffer2[0], w, h, FORMAT_PALETTED, modPal), 0);
    RTTR_REQUIRE_EQUAL_COLLECTIONS(outBuffer2, outBuffer);
    BOOST_REQUIRE_EQUAL(bmpPl.print(&outBuffer[0], w, h, FORMAT_PALETTED), 0);
    BOOST_REQUIRE_EQUAL(bmpPl.print(&outBuffer2[0], w, h, FORMAT_PALETTED, modPal), 0);
    RTTR_REQUIRE_EQUAL_COLLECTIONS(outBuffer2, outBuffer);

    // ARGB
    BOOST_REQUIRE_EQUAL(bmp.convertFormat(FORMAT_BGRA), 0);
    BOOST_REQUIRE_EQUAL(bmpPl.convertFormat(FORMAT_BGRA), 0);
    bmp.removePalette();
    bmpPl.removePalette();

    // ARGB -> ARGB: Plain copy
    BOOST_REQUIRE_EQUAL(bmp.print(&outBuffer[0], w, h, FORMAT_BGRA), 0);
    BOOST_REQUIRE_EQUAL(bmp.print(&outBuffer2[0], w, h, FORMAT_BGRA, modPal), 0);
    RTTR_REQUIRE_EQUAL_COLLECTIONS(outBuffer2, outBuffer);
    // Same if palette is set
    bmp.setPaletteCopy(*modPal);
    BOOST_REQUIRE_EQUAL(bmp.print(&outBuffer2[0], w, h, FORMAT_BGRA, modPal), 0);
    RTTR_REQUIRE_EQUAL_COLLECTIONS(outBuffer2, outBuffer);

    BOOST_REQUIRE_EQUAL(bmpPl.print(&outBuffer[0], w, h, FORMAT_BGRA, palette), 0);
    BOOST_REQUIRE_EQUAL(bmpPl.print(&outBuffer2[0], w, h, FORMAT_BGRA, modPal), 0);
    RTTR_REQUIRE_EQUAL_COLLECTIONS(outBuffer2, outBuffer);
    bmpPl.setPaletteCopy(*modPal);
    BOOST_REQUIRE_EQUAL(bmpPl.print(&outBuffer2[0], w, h, FORMAT_BGRA, modPal), 0);
    RTTR_REQUIRE_EQUAL_COLLECTIONS(outBuffer2, outBuffer);

    // ARGB -> Palette: Prefer passed palette
    outBuffer2.resize(inBuffer.size());
    BOOST_REQUIRE_EQUAL(bmp.print(&outBuffer2[0], w, h, FORMAT_PALETTED, palette), 0);
    RTTR_REQUIRE_EQUAL_COLLECTIONS(outBuffer2, inBuffer);
    bmp.setPaletteCopy(*palette);
    BOOST_REQUIRE_EQUAL(bmp.print(&outBuffer[0], w, h, FORMAT_PALETTED), 0);
    RTTR_REQUIRE_EQUAL_COLLECTIONS(outBuffer2, inBuffer);
    BOOST_REQUIRE_EQUAL(bmpPl.print(&outBuffer2[0], w, h, FORMAT_PALETTED, palette), 0);
    RTTR_REQUIRE_EQUAL_COLLECTIONS(outBuffer2, inBufferPl);
    bmpPl.setPaletteCopy(*palette);
    BOOST_REQUIRE_EQUAL(bmpPl.print(&outBuffer[0], w, h, FORMAT_PALETTED), 0);
    RTTR_REQUIRE_EQUAL_COLLECTIONS(outBuffer2, inBufferPl);

    // Error when no palette
    bmp.removePalette();
    bmpPl.removePalette();
    BOOST_REQUIRE_EQUAL(bmp.print(&outBuffer[0], w, h, FORMAT_PALETTED), ErrorCode::PALETTE_MISSING);
    BOOST_REQUIRE_EQUAL(bmpPl.print(&outBuffer[0], w, h, FORMAT_PALETTED), ErrorCode::PALETTE_MISSING);
}

BOOST_AUTO_TEST_CASE(CheckPalette)
{
    ArchivItem_Bitmap_Raw bmp;
    unsigned w = 10, h = 14;
    PixelBufferPaletted buffer(w, h, 22);
    BOOST_REQUIRE_EQUAL(bmp.create(buffer, palette), 0);
    BOOST_REQUIRE_EQUAL(bmp.convertFormat(FORMAT_BGRA), 0);
    ArchivItem_Palette wrongPal;
    BOOST_REQUIRE(bmp.checkPalette(*palette));
    BOOST_REQUIRE(!bmp.checkPalette(wrongPal));
    // Let it find the color (only 1 used)
    ArchivItem_Palette okPal(wrongPal);
    okPal.set(22, palette->get(22));
    BOOST_REQUIRE(bmp.checkPalette(okPal));
    // All palettes are fine for transparent colors
    bmp.init(w, h, FORMAT_BGRA);
    BOOST_REQUIRE(bmp.checkPalette(*palette));
    BOOST_REQUIRE(bmp.checkPalette(wrongPal));
    // Colored pixel but alpha == 0 -> Still transparent
    bmp.setPixel(0, 0, ColorARGB(0, 1, 2, 3));
    BOOST_REQUIRE(bmp.checkPalette(*palette));
    BOOST_REQUIRE(bmp.checkPalette(wrongPal));
    // Colored pixel -> wrong
    bmp.setPixel(0, 0, ColorARGB(1, 1, 2, 3));
    BOOST_REQUIRE(!bmp.checkPalette(wrongPal));
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
    BOOST_REQUIRE_EQUAL(bmpPal.create(bw, bh, NULL, w, h, FORMAT_PALETTED, palette), ErrorCode::INVALID_BUFFER);
    BOOST_REQUIRE_EQUAL(bmpPal.create(bw, bh, &inBufferPal[0], w, h, FORMAT_PALETTED, NULL), ErrorCode::PALETTE_MISSING);
    // These create transparent bitmaps
    BOOST_REQUIRE_EQUAL(bmpPal.create(bw, bh, &inBufferPal[0], 0, h, FORMAT_PALETTED, palette), 0);
    BOOST_REQUIRE_EQUAL(bmpPal.getWidth(), bw);
    BOOST_REQUIRE_EQUAL(bmpPal.getHeight(), bh);
    BOOST_REQUIRE_EQUAL(bmpPal.create(bw, bh, &inBufferPal[0], w, 0, FORMAT_PALETTED, palette), 0);
    BOOST_REQUIRE_EQUAL(bmpPal.getWidth(), bw);
    BOOST_REQUIRE_EQUAL(bmpPal.getHeight(), bh);
    // These create empty bitmaps
    BOOST_REQUIRE_EQUAL(bmpPal.create(0, bh, &inBufferPal[0], w, h, FORMAT_PALETTED, palette), 0);
    BOOST_REQUIRE_EQUAL(bmpPal.getWidth(), 0u);
    BOOST_REQUIRE_EQUAL(bmpPal.getHeight(), 0u);
    BOOST_REQUIRE_EQUAL(bmpPal.create(bw, 0, &inBufferPal[0], w, h, FORMAT_PALETTED, palette), 0);
    BOOST_REQUIRE_EQUAL(bmpPal.getWidth(), 0u);
    BOOST_REQUIRE_EQUAL(bmpPal.getHeight(), 0u);

    // Succeed
    BOOST_REQUIRE_EQUAL(bmpPal.create(bw, bh, &inBufferPal[0], w, h, FORMAT_PALETTED, palette), 0);
    BOOST_REQUIRE_EQUAL(bmpPal.getWidth(), bw);
    BOOST_REQUIRE_EQUAL(bmpPal.getHeight(), bh);
    // Now write to paletted buffer
    // Fail on NULL
    BOOST_REQUIRE_EQUAL(bmpPal.print(NULL, w, h, FORMAT_PALETTED), ErrorCode::INVALID_BUFFER);
    // Do nothing
    BOOST_REQUIRE_EQUAL(bmpPal.print(&outBufferPal[0], 0, h, FORMAT_PALETTED), 0);
    RTTR_REQUIRE_EQUAL_COLLECTIONS(outBufferPal, emptyBufferPal);
    BOOST_REQUIRE_EQUAL(bmpPal.print(&outBufferPal[0], w, 0, FORMAT_PALETTED), 0);
    RTTR_REQUIRE_EQUAL_COLLECTIONS(outBufferPal, emptyBufferPal);
    // Succeed
    BOOST_REQUIRE_EQUAL(bmpPal.print(&outBufferPal[0], w, h, FORMAT_PALETTED), 0);
    RTTR_REQUIRE_EQUAL_COLLECTIONS(outBufferPal, inBufferPal);
    // Write to RGBA buffer
    BOOST_REQUIRE_EQUAL(bmpPal.print(&outBuffer[0], w, h, FORMAT_BGRA), 0);
    RTTR_REQUIRE_EQUAL_COLLECTIONS(outBuffer, inBufferRGB);

    // Test partial write
    unsigned xStart = 2, xStartB = 3, partW = 4, yStart = 1, yStartB = 5, partH = 6;
    std::fill(outBufferPal.begin(), outBufferPal.end(), 42u);
    std::fill(outBuffer.begin(), outBuffer.end(), 42u);
    BOOST_REQUIRE_EQUAL(bmpPal.print(&outBufferPal[0], w, h, FORMAT_PALETTED, NULL, xStart, yStart, xStartB, yStartB, partW, partH), 0);
    BOOST_REQUIRE_EQUAL(bmpPal.print(&outBuffer[0], w, h, FORMAT_BGRA, NULL, xStart, yStart, xStartB, yStartB, partW, partH), 0);
    for(unsigned y = 0; y < h; y++)
    {
        for(unsigned x = 0; x < w; x++)
        {
            unsigned idxPal = x + y * w;
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
    BOOST_REQUIRE_EQUAL(bmp.create(bw, bh, &inBuffer[0], w, h, FORMAT_BGRA), 0);
    BOOST_REQUIRE_EQUAL(bmp.getWidth(), bw);
    BOOST_REQUIRE_EQUAL(bmp.getHeight(), bh);
    // Now write to paletted buffer
    // Succeed
    BOOST_REQUIRE_EQUAL(bmp.print(&outBufferPal[0], w, h, FORMAT_PALETTED, palette), 0);
    RTTR_REQUIRE_EQUAL_COLLECTIONS(outBufferPal, inBufferPal);
    BOOST_REQUIRE_EQUAL(bmp.print(&outBuffer[0], w, h, FORMAT_BGRA), 0);
    RTTR_REQUIRE_EQUAL_COLLECTIONS(outBuffer, inBuffer);
}

BOOST_AUTO_TEST_CASE(TransparentTex)
{
    unsigned w = 10, h = 14;
    // Use different size for bitmap (bigger width, smaller height)
    unsigned bw = w + 3, bh = h - 2;
    std::vector<uint8_t> inBufferPal(w * h, palette->getTransparentIdx());
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
    BOOST_REQUIRE_EQUAL(bmpPal.create(bw, bh, &inBufferPal[0], w, h, FORMAT_PALETTED, palette), 0);
    BOOST_REQUIRE_EQUAL(bmpPlayerPal.create(bw, bh, &inBufferPal[0], w, h, FORMAT_PALETTED, palette), 0);
    // Create BGRA
    ArchivItem_Bitmap_Raw bmp;
    ArchivItem_Bitmap_Player bmpPlayer;
    BOOST_REQUIRE_EQUAL(bmp.create(bw, bh, &inBuffer[0], w, h, FORMAT_BGRA), 0);
    BOOST_REQUIRE_EQUAL(bmpPlayer.create(bw, bh, &inBuffer[0], w, h, FORMAT_BGRA, palette), 0);
    // Writing should not change buffer
    std::vector<uint8_t> outBuffer(inBuffer.size(), 42);
    std::vector<uint8_t> outBufferCheck(outBuffer);
    BOOST_REQUIRE_EQUAL(bmpPal.print(&outBuffer[0], w, h, FORMAT_PALETTED), 0);
    RTTR_REQUIRE_EQUAL_COLLECTIONS(outBuffer, outBufferCheck);
    BOOST_REQUIRE_EQUAL(bmpPlayerPal.print(&outBuffer[0], w, h, FORMAT_PALETTED), 0);
    RTTR_REQUIRE_EQUAL_COLLECTIONS(outBuffer, outBufferCheck);

    BOOST_REQUIRE_EQUAL(bmpPal.print(&outBuffer[0], w, h, FORMAT_BGRA), 0);
    RTTR_REQUIRE_EQUAL_COLLECTIONS(outBuffer, outBufferCheck);
    BOOST_REQUIRE_EQUAL(bmpPlayerPal.print(&outBuffer[0], w, h, FORMAT_BGRA), 0);
    RTTR_REQUIRE_EQUAL_COLLECTIONS(outBuffer, outBufferCheck);

    BOOST_REQUIRE_EQUAL(bmp.print(&outBuffer[0], w, h, FORMAT_PALETTED, palette), 0);
    RTTR_REQUIRE_EQUAL_COLLECTIONS(outBuffer, outBufferCheck);
    BOOST_REQUIRE_EQUAL(bmpPlayer.print(&outBuffer[0], w, h, FORMAT_PALETTED, palette), 0);
    RTTR_REQUIRE_EQUAL_COLLECTIONS(outBuffer, outBufferCheck);

    BOOST_REQUIRE_EQUAL(bmp.print(&outBuffer[0], w, h, FORMAT_BGRA), 0);
    RTTR_REQUIRE_EQUAL_COLLECTIONS(outBuffer, outBufferCheck);
    BOOST_REQUIRE_EQUAL(bmpPlayer.print(&outBuffer[0], w, h, FORMAT_BGRA, palette), 0);
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
    BOOST_REQUIRE_EQUAL(bmpPal.create(bw, bh, NULL, w, h, FORMAT_PALETTED, palette, playerClrStart), ErrorCode::INVALID_BUFFER);
    BOOST_REQUIRE_EQUAL(bmpPal.create(bw, bh, &inBufferPal[0], w, h, FORMAT_PALETTED, NULL, playerClrStart), ErrorCode::PALETTE_MISSING);
    // These create transparent bitmaps
    BOOST_REQUIRE_EQUAL(bmpPal.create(bw, bh, &inBufferPal[0], 0, h, FORMAT_PALETTED, palette, playerClrStart), 0);
    BOOST_REQUIRE_EQUAL(bmpPal.getWidth(), bw);
    BOOST_REQUIRE_EQUAL(bmpPal.getHeight(), bh);
    BOOST_REQUIRE_EQUAL(bmpPal.create(bw, bh, &inBufferPal[0], w, 0, FORMAT_PALETTED, palette, playerClrStart), 0);
    BOOST_REQUIRE_EQUAL(bmpPal.getWidth(), bw);
    BOOST_REQUIRE_EQUAL(bmpPal.getHeight(), bh);
    // These create empty bitmaps
    BOOST_REQUIRE_EQUAL(bmpPal.create(0, bh, &inBufferPal[0], w, h, FORMAT_PALETTED, palette, playerClrStart), 0);
    BOOST_REQUIRE_EQUAL(bmpPal.getWidth(), 0u);
    BOOST_REQUIRE_EQUAL(bmpPal.getHeight(), 0u);
    BOOST_REQUIRE_EQUAL(bmpPal.create(bw, 0, &inBufferPal[0], w, h, FORMAT_PALETTED, palette, playerClrStart), 0);
    BOOST_REQUIRE_EQUAL(bmpPal.getWidth(), 0u);
    BOOST_REQUIRE_EQUAL(bmpPal.getHeight(), 0u);
    // Succeed
    BOOST_REQUIRE_EQUAL(bmpPal.create(bw, bh, &inBufferPal[0], w, h, FORMAT_PALETTED, palette, playerClrStart), 0);
    BOOST_REQUIRE_EQUAL(bmpPal.getWidth(), bw);
    BOOST_REQUIRE_EQUAL(bmpPal.getHeight(), bh);
    // Now write to paletted buffer
    // Fail on NULL
    BOOST_REQUIRE_EQUAL(bmpPal.print(NULL, w, h, FORMAT_PALETTED, NULL, playerClrStart), ErrorCode::INVALID_BUFFER);
    BOOST_REQUIRE_EQUAL(bmpPal.print(&outBufferPal[0], 0, h, FORMAT_PALETTED, palette, playerClrStart), 0);
    RTTR_REQUIRE_EQUAL_COLLECTIONS(outBufferPal, emptyBufferPal);
    BOOST_REQUIRE_EQUAL(bmpPal.print(&outBufferPal[0], w, 0, FORMAT_PALETTED, palette, playerClrStart), 0);
    RTTR_REQUIRE_EQUAL_COLLECTIONS(outBufferPal, emptyBufferPal);
    // Succeed
    BOOST_REQUIRE_EQUAL(bmpPal.print(&outBufferPal[0], w, h, FORMAT_PALETTED, palette, playerClrStart), 0);
    RTTR_REQUIRE_EQUAL_COLLECTIONS(outBufferPal, inBufferPal);
    // Write to RGBA buffer
    BOOST_REQUIRE_EQUAL(bmpPal.print(&outBuffer[0], w, h, FORMAT_BGRA, palette, playerClrStart), 0);
    RTTR_REQUIRE_EQUAL_COLLECTIONS(outBuffer, inBufferRGB);

    // Test partial write
    unsigned xStart = 2, xStartB = 3, partW = 4, yStart = 1, yStartB = 5, partH = 6;
    std::fill(outBufferPal.begin(), outBufferPal.end(), 42u);
    std::fill(outBuffer.begin(), outBuffer.end(), 42u);
    BOOST_REQUIRE_EQUAL(
      bmpPal.print(&outBufferPal[0], w, h, FORMAT_PALETTED, palette, playerClrStart, xStart, yStart, xStartB, yStartB, partW, partH), 0);
    BOOST_REQUIRE_EQUAL(
      bmpPal.print(&outBuffer[0], w, h, FORMAT_BGRA, palette, playerClrStart, xStart, yStart, xStartB, yStartB, partW, partH), 0);
    for(unsigned y = 0; y < h; y++)
    {
        for(unsigned x = 0; x < w; x++)
        {
            unsigned idxPal = x + y * w;
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
    BOOST_REQUIRE_EQUAL(bmp.create(bw, bh, &inBuffer[0], w, h, FORMAT_BGRA, palette, playerClrStart), 0);
    BOOST_REQUIRE_EQUAL(bmp.getWidth(), bw);
    BOOST_REQUIRE_EQUAL(bmp.getHeight(), bh);

    BOOST_REQUIRE_EQUAL(bmp.print(&outBufferPal[0], w, h, FORMAT_PALETTED, palette, playerClrStart), 0);
    RTTR_REQUIRE_EQUAL_COLLECTIONS(outBufferPal, inBufferPal);
    BOOST_REQUIRE_EQUAL(bmp.print(&outBuffer[0], w, h, FORMAT_BGRA, palette, playerClrStart), 0);
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
    BOOST_REQUIRE_EQUAL(bmpPal.create(bw, bh, &inBufferPal[0], w, h, FORMAT_PALETTED, palette, playerClrStart), 0);
    // Now write to paletted buffer
    BOOST_REQUIRE_EQUAL(bmpPal.print(&outBufferPal[0], w, h, FORMAT_PALETTED, NULL, playerClrStart), 0);
    RTTR_REQUIRE_EQUAL_COLLECTIONS(outBufferPal, inBufferPal);
    // Write to RGBA buffer
    BOOST_REQUIRE_EQUAL(bmpPal.print(&outBuffer[0], w, h, FORMAT_BGRA, NULL, playerClrStart), 0);
    RTTR_REQUIRE_EQUAL_COLLECTIONS(outBuffer, inBufferRGB);
    std::fill(outBufferPal.begin(), outBufferPal.end(), 42);
    std::fill(outBuffer.begin(), outBuffer.end(), 42);
    // paletted buffer, 2nd player color (recolor)
    BOOST_REQUIRE_EQUAL(bmpPal.print(&outBufferPal[0], w, h, FORMAT_PALETTED, NULL, playerClrStart2), 0);
    RTTR_REQUIRE_EQUAL_COLLECTIONS(outBufferPal, inBufferPal2);
    // Same for BGRA
    BOOST_REQUIRE_EQUAL(bmpPal.print(&outBuffer[0], w, h, FORMAT_BGRA, NULL, playerClrStart2), 0);
    RTTR_REQUIRE_EQUAL_COLLECTIONS(outBuffer, inBufferRGB2);

    std::fill(outBufferPal.begin(), outBufferPal.end(), 42);
    std::fill(outBuffer.begin(), outBuffer.end(), 42);
    // Player colors only
    BOOST_REQUIRE_EQUAL(bmpPal.print(&outBufferPal[0], w, h, FORMAT_PALETTED, NULL, playerClrStart2, 0, 0, 0, 0, 0, 0, true), 0);
    BOOST_REQUIRE_EQUAL(bmpPal.print(&outBuffer[0], w, h, FORMAT_BGRA, NULL, playerClrStart2, 0, 0, 0, 0, 0, 0, true), 0);
    for(unsigned i = 0; i < 4; i++)
    {
        BOOST_REQUIRE_EQUAL(outBufferPal[i], inBufferPal2[i]);
        BOOST_REQUIRE_EQUAL(ColorARGB::fromARGB(&outBuffer[i * 4]), ColorARGB::fromARGB(&inBufferRGB2[i * 4]));
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
    BOOST_REQUIRE_EQUAL(bmp.create(bw, bh, &inBuffer[0], w, h, FORMAT_BGRA, palette, playerClrStart), 0);
    // Write to paletted
    BOOST_REQUIRE_EQUAL(bmp.print(&outBufferPal[0], w, h, FORMAT_PALETTED, palette, playerClrStart), 0);
    RTTR_REQUIRE_EQUAL_COLLECTIONS(outBufferPal, inBufferPal);
    // Write to BGRA
    BOOST_REQUIRE_EQUAL(bmp.print(&outBuffer[0], w, h, FORMAT_BGRA, palette, playerClrStart), 0);
    RTTR_REQUIRE_EQUAL_COLLECTIONS(outBuffer, inBuffer);

    std::fill(outBufferPal.begin(), outBufferPal.end(), 42);
    std::fill(outBuffer.begin(), outBuffer.end(), 42);
    // recolored paletted
    BOOST_REQUIRE_EQUAL(bmp.print(&outBufferPal[0], w, h, FORMAT_PALETTED, palette, playerClrStart2), 0);
    RTTR_REQUIRE_EQUAL_COLLECTIONS(outBufferPal, inBufferPal2);
    // Same for BGRA
    BOOST_REQUIRE_EQUAL(bmp.print(&outBuffer[0], w, h, FORMAT_BGRA, palette, playerClrStart2), 0);
    RTTR_REQUIRE_EQUAL_COLLECTIONS(outBuffer, inBuffer2);

    std::fill(outBufferPal.begin(), outBufferPal.end(), 42);
    std::fill(outBuffer.begin(), outBuffer.end(), 42);
    // Player colors only
    BOOST_REQUIRE_EQUAL(bmp.print(&outBufferPal[0], w, h, FORMAT_PALETTED, palette, playerClrStart2, 0, 0, 0, 0, 0, 0, true), 0);
    BOOST_REQUIRE_EQUAL(bmp.print(&outBuffer[0], w, h, FORMAT_BGRA, palette, playerClrStart2, 0, 0, 0, 0, 0, 0, true), 0);
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
    std::vector<uint8_t> inBufferPal(w * h, palette->getTransparentIdx());
    ArchivItem_Bitmap_Player bmp;
    ArchivItem_Bitmap_Raw bmpRaw;
    {
        Rect vis;
        // Test empty bmp
        BOOST_REQUIRE_EQUAL(bmp.create(bw, bh, &inBufferPal[0], w, h, FORMAT_PALETTED, palette), 0);
        bmp.getVisibleArea(vis.x, vis.y, vis.w, vis.h);
        BOOST_REQUIRE_EQUAL(vis, Rect(0, 0, 0, 0));
        Rect vis2;
        // Test empty bmp
        BOOST_REQUIRE_EQUAL(bmpRaw.create(bw, bh, &inBufferPal[0], w, h, FORMAT_PALETTED, palette), 0);
        bmpRaw.getVisibleArea(vis2.x, vis2.y, vis2.w, vis2.h);
        BOOST_REQUIRE_EQUAL(vis2, Rect(0, 0, 0, 0));
    }

    std::vector<Rect> testRects;
    //                  left-top          top               right-top
    testRects += Rect(0, 0, 1, 1), Rect(2, 0, 1, 1), Rect(w - 1, 0, 1, 1),
      // left            middle            right
      Rect(0, 3, 1, 1), Rect(3, 2, 1, 1), Rect(w - 1, 3, 1, 1),
      // left-bottom     bottom-middle     bottom-right
      Rect(0, h - 1, 1, 1), Rect(3, h - 1, 1, 1), Rect(w - 1, h - 1, 1, 1),
      // doublePixelRects;
      Rect(0, 0, 2, 1), Rect(0, 0, 1, 2), Rect(2, 0, 1, 3), Rect(2, 0, 3, 1), Rect(w - 3, 0, 3, 1), Rect(2, 0, 1, 3), Rect(2, 0, 3, 1),
      Rect(3, 2, 3, 1), Rect(3, 2, 1, 3), Rect(0, h - 3, 2, 1), Rect(0, h - 3, 1, 3), Rect(3, h - 3, 3, 1), Rect(3, h - 3, 1, 3),
      Rect(w - 4, h - 3, 4, 3), Rect(0, 0, w, h);

    BOOST_FOREACH(const Rect& rect, testRects)
    {
        // Test for non-player-color (127) and player color (128)
        for(unsigned i = 127; i < 129; i++)
        {
            inBufferPal[rect.x + rect.y * w] = i;
            inBufferPal[rect.x + rect.w - 1 + (rect.y + rect.h - 1) * w] = i;
            BOOST_REQUIRE_EQUAL(bmp.create(bw, bh, &inBufferPal[0], w, h, FORMAT_PALETTED, palette), 0);
            Rect vis;
            bmp.getVisibleArea(vis.x, vis.y, vis.w, vis.h);
            BOOST_REQUIRE_EQUAL(vis, rect);

            BOOST_REQUIRE_EQUAL(bmpRaw.create(bw, bh, &inBufferPal[0], w, h, FORMAT_PALETTED, palette), 0);
            Rect visRaw;
            bmpRaw.getVisibleArea(visRaw.x, visRaw.y, visRaw.w, visRaw.h);
            BOOST_REQUIRE_EQUAL(visRaw, rect);

            inBufferPal[rect.x + rect.y * w] = palette->getTransparentIdx();
            inBufferPal[rect.x + rect.w - 1 + (rect.y + rect.h - 1) * w] = palette->getTransparentIdx();

            // Buffer in (byte) BGRA format
            std::vector<uint8_t> inBuffer(inBufferPal.size() * 4u, 0);
            BOOST_REQUIRE_EQUAL(bmp.print(&inBuffer[0], w, h, FORMAT_BGRA), 0);
            ArchivItem_Bitmap_Player bmp2;
            BOOST_REQUIRE_EQUAL(bmp2.create(bw, bh, &inBuffer[0], w, h, FORMAT_BGRA, palette), 0);
            Rect vis2;
            bmp2.getVisibleArea(vis2.x, vis2.y, vis2.w, vis2.h);
            BOOST_REQUIRE_EQUAL(vis2, rect);

            BOOST_REQUIRE_EQUAL(bmpRaw.create(bw, bh, &inBuffer[0], w, h, FORMAT_BGRA), 0);
            Rect visRaw2;
            bmpRaw.getVisibleArea(visRaw2.x, visRaw2.y, visRaw2.w, visRaw2.h);
            BOOST_REQUIRE_EQUAL(visRaw2, rect);
            inBufferPal[rect.x + rect.y * w] = palette->getTransparentIdx();
        }
    }
}

BOOST_AUTO_TEST_SUITE_END()
