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

#include "ColorOutput.h"
#include "LoadPalette.h"
#include "cmpFiles.h"
#include "test/config.h"
#include "libsiedler2/Archiv.h"
#include "libsiedler2/ArchivItem_Bitmap_Player.h"
#include "libsiedler2/ArchivItem_Bitmap_Raw.h"
#include "libsiedler2/ColorBGRA.h"
#include "libsiedler2/ErrorCodes.h"
#include "libsiedler2/IAllocator.h"
#include "libsiedler2/PixelBufferBGRA.h"
#include "libsiedler2/PixelBufferPaletted.h"
#include "libsiedler2/libsiedler2.h"
#include <boost/filesystem.hpp>
#include <boost/test/unit_test.hpp>
#include <algorithm>
#include <numeric>
#include <random>
#include <utility>

namespace {
struct Rect
{
    int x, y;
    unsigned w, h;
    Rect() = default;
    Rect(int x, int y, unsigned w, unsigned h) : x(x), y(y), w(w), h(h) {}
    bool operator==(const Rect& rhs) const { return x == rhs.x && y == rhs.y && w == rhs.w && h == rhs.h; }
};

std::ostream& operator<<(std::ostream& os, const Rect& rect)
{
    return os << "(" << rect.x << ", " << rect.y << ", " << rect.w << ", " << rect.h << ")";
}

auto randColor()
{
    static std::mt19937 mt(std::random_device{}());
    static std::uniform_int_distribution<> distr(0, 255);
    return libsiedler2::ColorBGRA(distr(mt), distr(mt), distr(mt), distr(mt));
}
} // namespace

namespace libsiedler2 {
static std::ostream& operator<<(std::ostream& os, libsiedler2::TextureFormat fmt)
{
    return os << static_cast<unsigned>(fmt);
}
} // namespace libsiedler2

using namespace libsiedler2;
namespace bfs = boost::filesystem;

BOOST_FIXTURE_TEST_SUITE(Bitmaps, LoadPalette)

BOOST_AUTO_TEST_CASE(ReadWritePlayerBitmap)
{
    std::string bmpPath = libsiedler2::test::inputPath + "/bmpPlayer.lst";
    std::string bmpOutPath = test::outputPath + "/bmp.lst";
    BOOST_REQUIRE(bfs::exists(bmpPath));
    Archiv bmp;
    BOOST_REQUIRE(testLoad(0, bmpPath, bmp, palette));
    BOOST_REQUIRE_EQUAL(Write(bmpOutPath, bmp, palette), 0);
    BOOST_REQUIRE(testFilesEqual(bmpOutPath, bmpPath));
}

BOOST_AUTO_TEST_CASE(ReadWriteRawBitmap)
{
    std::string bmpPath = libsiedler2::test::inputPath + "/bmpRaw.lst";
    std::string bmpOutPath = test::outputPath + "/bmp.lst";
    BOOST_REQUIRE(bfs::exists(bmpPath));
    Archiv bmp;
    BOOST_REQUIRE(testLoad(0, bmpPath, bmp, palette));
    BOOST_REQUIRE_EQUAL(Write(bmpOutPath, bmp, palette), 0);
    BOOST_REQUIRE(testFilesEqual(bmpOutPath, bmpPath));
}

BOOST_AUTO_TEST_CASE(ReadWriteShadowBitmap)
{
    std::string bmpPath = libsiedler2::test::inputPath + "/bmpShadow.lst";
    std::string bmpOutPath = test::outputPath + "/bmp.lst";
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
    auto shadowBmp = getAllocator().create<baseArchivItem_Bitmap>(BobType::BitmapShadow);
    shadowBmp->create(buffer, palette);
    bmp.clear();
    bmp.push(std::move(shadowBmp));
    BOOST_REQUIRE_EQUAL(Write(bmpOutPath, bmp, palette), 0);
}

BOOST_AUTO_TEST_CASE(ReadWriteRLEBitmap)
{
    std::string bmpPath = libsiedler2::test::inputPath + "/bmpRLE.lst";
    std::string bmpOutPath = test::outputPath + "/bmp.lst";
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
    auto shadowBmp = getAllocator().create<baseArchivItem_Bitmap>(BobType::BitmapRLE);
    shadowBmp->create(buffer, palette);
    bmp.clear();
    bmp.push(std::move(shadowBmp));
    BOOST_REQUIRE_EQUAL(Write(bmpOutPath, bmp, palette), 0);
}

BOOST_AUTO_TEST_CASE(ReadWriteBmp)
{
    std::string bmpPath = libsiedler2::test::inputPath + "/logo.bmp";
    std::string bmpOutPath = test::outputPath + "/outLogo.bmp";
    BOOST_REQUIRE(bfs::exists(bmpPath));
    Archiv bmp;
    BOOST_REQUIRE(testLoad(0, bmpPath, bmp));
    BOOST_REQUIRE_EQUAL(Write(bmpOutPath, bmp), 0);
    BOOST_REQUIRE(testFilesEqual(bmpOutPath, bmpPath));
}

BOOST_AUTO_TEST_CASE(ReadWritePalettedBmp)
{
    std::string bmpPath = libsiedler2::test::inputPath + "/pal.bmp";
    std::string bmpOutPath = test::outputPath + "/outPal.bmp";
    BOOST_REQUIRE(bfs::exists(bmpPath));
    Archiv archiv;
    BOOST_REQUIRE(testLoad(0, bmpPath, archiv));
    auto* bmp = dynamic_cast<ArchivItem_BitmapBase*>(archiv[0]);
    BOOST_REQUIRE(bmp);
    BOOST_REQUIRE(bmp->getPalette());
    // We want to write as paletted again
    BOOST_REQUIRE_EQUAL(bmp->convertFormat(TextureFormat::Paletted), 0);
    BOOST_REQUIRE_EQUAL(Write(bmpOutPath, archiv), 0);
    BOOST_REQUIRE(testFilesEqual(bmpOutPath, bmpPath));
}

BOOST_AUTO_TEST_CASE(CreatePalettedBmp)
{
    std::string bmpOutPath = test::outputPath + "/outPalNew.bmp";
    PixelBufferBGRA buffer(17, 19);
    BOOST_TEST_REQUIRE(palette->getTransparentIdx() > 0); // Default is 0, make sure the palette is not defaulted
    // Fill with all colors from the palette
    BOOST_TEST_REQUIRE(buffer.getNumPixels() >= 256u); // Make sure we use all colors
    int idx = 0;
    for(auto& c : buffer)
    {
        c = palette->get(idx++);
        idx %= 256;
        if(c == TRANSPARENT_COLOR)
            c = ColorBGRA();
    }
    ArchivItem_Bitmap_Raw bmp;
    bmp.create(buffer, palette);
    // Store as paletted
    BOOST_TEST_REQUIRE(bmp.convertFormat(TextureFormat::Paletted) == 0);
    Archiv archiv;
    archiv.pushC(bmp);
    BOOST_TEST_REQUIRE(Write(bmpOutPath, archiv) == 0);

    Archiv archivLoad;
    BOOST_REQUIRE(testLoad(0, bmpOutPath, archivLoad, palette));
    PixelBufferBGRA bufferLoad(buffer.getWidth(), buffer.getHeight());
    auto* bmpLoad = dynamic_cast<ArchivItem_Bitmap_Raw*>(archivLoad[0]);
    BOOST_REQUIRE(bmpLoad);
    BOOST_TEST_REQUIRE(bmpLoad->print(bufferLoad) == 0);
    BOOST_TEST_REQUIRE(bufferLoad == buffer, boost::test_tools::per_element());
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
        Info() = default;
        Info(std::string filename, bool isPaletted, bool containsPalette, bool supportsBoth = false)
            : filename(std::move(filename)), isPaletted(isPaletted), containsPalette(containsPalette), supportsBoth(supportsBoth)
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

static ArchivItem_BitmapBase* getFirstBitmap(Archiv& archiv)
{
    for(unsigned j = 0; j < archiv.size(); j++)
    {
        auto* bmp = dynamic_cast<ArchivItem_BitmapBase*>(archiv[j]);
        if(bmp)
            return bmp;
    }
    return nullptr;
}

BOOST_AUTO_TEST_CASE(DefaultTextureFormatAndPalette)
{
    const TestBitmaps testFiles;
    // Try all formats with all possible bmp types.
    for(TextureFormat curFmt : {TextureFormat::Original, TextureFormat::Paletted, TextureFormat::BGRA})
    {
        FormatSetter fmtSetter(curFmt);
        for(const TestBitmaps::Info& testFile : testFiles.files)
        {
            Archiv archiv;
            int ec = Load(libsiedler2::test::inputPath + "/" + testFile.filename, archiv, palette);
            BOOST_REQUIRE_MESSAGE(ec == 0, "Error " << getErrorString(ec) << " loading " << testFile.filename);
            const ArchivItem_BitmapBase* bmp = getFirstBitmap(archiv);
            BOOST_REQUIRE(bmp);
            if(curFmt != TextureFormat::Original)
                BOOST_REQUIRE_EQUAL(bmp->getFormat(), curFmt);
            else if(testFile.isPaletted)
                BOOST_REQUIRE_EQUAL(bmp->getFormat(), TextureFormat::Paletted);
            else
                BOOST_REQUIRE_EQUAL(bmp->getFormat(), TextureFormat::BGRA);
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
    for(TextureFormat curFmt : {TextureFormat::Original, TextureFormat::Paletted, TextureFormat::BGRA})
    {
        FormatSetter fmtSetter(curFmt);
        for(const TestBitmaps::Info& testFile : testFiles.files)
        {
            Archiv archiv;
            if((curFmt == TextureFormat::Paletted && !testFile.containsPalette) || (testFile.isPaletted && !testFile.containsPalette))
            {
                // Paletted files need a palette. For conversion to paletted we also need one
                BOOST_REQUIRE(testLoad(ErrorCode::PALETTE_MISSING, libsiedler2::test::inputPath + "/" + testFile.filename, archiv));
            } else
            {
                // Non paletted file or palette contained
                BOOST_REQUIRE(testLoad(0, libsiedler2::test::inputPath + "/" + testFile.filename, archiv));
                const ArchivItem_BitmapBase* bmp = getFirstBitmap(archiv);
                // For paletted bitmaps we must have a palette, the others must not have one
                if(bmp->getFormat() == TextureFormat::Paletted || testFile.containsPalette)
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
            BOOST_REQUIRE(testLoad(0, libsiedler2::test::inputPath + "/" + testFile.filename, archiv, usedPalette));
            const ArchivItem_BitmapBase* bmp = getFirstBitmap(archiv);
            if(bmp->getFormat() == TextureFormat::Paletted)
            {
                // Paletted formats must have a palette
                BOOST_REQUIRE(bmp->getPalette());
                // If the file contains a palette, it has to be used, otherwise the default shall be used
                if(testFile.containsPalette)
                    BOOST_REQUIRE(*usedPalette != *bmp->getPalette());
                else
                    BOOST_REQUIRE(*usedPalette == *bmp->getPalette());
            } else if(testFile.containsPalette)
            {
                // If the file contains a palette, it has to be used, otherwise no palette
                BOOST_REQUIRE(bmp->getPalette());
                BOOST_REQUIRE(*usedPalette != *bmp->getPalette());
                // If the empty palette is used, we will only find transparent and black pixels. Check that this did not happen
                bool clrFound = false;
                for(unsigned y = 0; y < bmp->getHeight(); y++)
                {
                    for(unsigned x = 0; x < bmp->getWidth(); x++)
                    {
                        ColorBGRA clr = bmp->getPixel(x, y);
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
    for(TextureFormat curFmt : {TextureFormat::Original, TextureFormat::Paletted, TextureFormat::BGRA})
    {
        FormatSetter fmtSetter(curFmt);
        for(const TestBitmaps::Info& testFile : testFiles.files)
        {
            Archiv archiv;
            std::string inFilepath = libsiedler2::test::inputPath + "/" + testFile.filename;
            std::string outFilepathRef = test::outputPath + "/reference_" + testFile.filename;
            std::string outFilepath = test::outputPath + "/" + testFile.filename;
            BOOST_REQUIRE(testLoad(0, inFilepath, archiv, palette));
            ArchivItem_BitmapBase* bmp = getFirstBitmap(archiv);
            // Not modified when writing with same palette
            BOOST_REQUIRE(testWrite(0, outFilepathRef, archiv, palette));
            if(testFile.supportsBoth && !testFile.isPaletted && bmp->getFormat() == TextureFormat::Paletted)
                BOOST_REQUIRE(!testFilesEqual(outFilepathRef, inFilepath)); // Stored as paletted
            else
                BOOST_REQUIRE(testFilesEqual(outFilepathRef, inFilepath));

            // a) no palette
            if(bmp->getFormat() == TextureFormat::BGRA)
            {
                auto bmpPal = clone(bmp->getPalette());
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
                BOOST_REQUIRE(testWrite(0, outFilepath, archiv, bmpPal ? bmpPal.get() : palette));
                if(testFile.supportsBoth && testFile.isPaletted)
                    BOOST_REQUIRE(!testFilesEqual(outFilepath, outFilepathRef)); // Still stored as RGB
                else
                    BOOST_REQUIRE(testFilesEqual(outFilepath, outFilepathRef));
                bmp->setPalette(std::move(bmpPal));
            }
            // b) use bitmaps palette if none passed
            if(!bmp->getPalette())
                bmp->setPaletteCopy(*palette);
            BOOST_REQUIRE(testWrite(0, outFilepath, archiv));
            if(testFile.supportsBoth && !testFile.isPaletted && bmp->getFormat() != TextureFormat::Paletted)
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
    PixelBufferBGRA clrBuffer(w, h);
    // Paletted
    BOOST_REQUIRE_EQUAL(bmp.create(palBuffer, palette), 0);
    BOOST_REQUIRE_EQUAL(bmp.getFormat(), TextureFormat::Paletted);
    BOOST_REQUIRE(bmp.getPalette());
    BOOST_REQUIRE(*bmp.getPalette() == *palette);
    BOOST_REQUIRE_EQUAL(bmpPl.create(palBuffer, palette), 0);
    BOOST_REQUIRE_EQUAL(bmpPl.getFormat(), TextureFormat::Paletted);
    BOOST_REQUIRE(bmpPl.getPalette());
    BOOST_REQUIRE(*bmpPl.getPalette() == *palette);
    // ARGB
    BOOST_REQUIRE_EQUAL(bmp.create(clrBuffer), 0);
    BOOST_REQUIRE_EQUAL(bmp.getFormat(), TextureFormat::BGRA);
    BOOST_REQUIRE(!bmp.getPalette());
    BOOST_REQUIRE_EQUAL(bmpPl.create(clrBuffer, palette), 0);
    BOOST_REQUIRE_EQUAL(bmpPl.getFormat(), TextureFormat::BGRA);
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
    BOOST_REQUIRE_EQUAL(bmp.create(&inBuffer[0], w, h, TextureFormat::Paletted, palette), 0);
    BOOST_REQUIRE_EQUAL(bmpPl.create(&inBufferPl[0], w, h, TextureFormat::Paletted, palette), 0);
    // Can't remove palette
    BOOST_REQUIRE_THROW(bmp.removePalette(), std::runtime_error);
    BOOST_REQUIRE_THROW(bmp.setPalette(nullptr), std::runtime_error);
    // Set mod pal to detect its use:
    bmp.setPaletteCopy(*modPal);
    bmpPl.setPaletteCopy(*modPal);
    // Paletted -> Paletted: No palette used, plain copy
    std::vector<uint8_t> outBuffer(inBuffer.size(), 42);
    BOOST_REQUIRE_EQUAL(bmp.print(&outBuffer[0], w, h, TextureFormat::Paletted), 0);
    BOOST_TEST_REQUIRE(outBuffer == inBuffer, boost::test_tools::per_element());
    ;
    BOOST_REQUIRE_EQUAL(bmpPl.print(&outBuffer[0], w, h, TextureFormat::Paletted), 0);
    BOOST_TEST_REQUIRE(outBuffer == inBufferPl, boost::test_tools::per_element());
    ;
    BOOST_REQUIRE_EQUAL(bmp.print(&outBuffer[0], w, h, TextureFormat::Paletted, modPal), 0);
    BOOST_TEST_REQUIRE(outBuffer == inBuffer, boost::test_tools::per_element());
    ;
    BOOST_REQUIRE_EQUAL(bmpPl.print(&outBuffer[0], w, h, TextureFormat::Paletted, modPal), 0);
    BOOST_TEST_REQUIRE(outBuffer == inBufferPl, boost::test_tools::per_element());
    ;

    // Paletted -> ARGB: Prefer contained
    bmp.setPaletteCopy(*palette);
    bmpPl.setPaletteCopy(*palette);
    outBuffer.resize(inBuffer.size() * 4, 42);
    std::vector<uint8_t> outBuffer2(outBuffer.size(), 42);
    BOOST_REQUIRE_EQUAL(bmp.print(&outBuffer[0], w, h, TextureFormat::Paletted), 0);
    BOOST_REQUIRE_EQUAL(bmp.print(&outBuffer2[0], w, h, TextureFormat::Paletted, modPal), 0);
    BOOST_TEST_REQUIRE(outBuffer2 == outBuffer, boost::test_tools::per_element());
    ;
    BOOST_REQUIRE_EQUAL(bmpPl.print(&outBuffer[0], w, h, TextureFormat::Paletted), 0);
    BOOST_REQUIRE_EQUAL(bmpPl.print(&outBuffer2[0], w, h, TextureFormat::Paletted, modPal), 0);
    BOOST_TEST_REQUIRE(outBuffer2 == outBuffer, boost::test_tools::per_element());
    ;

    // ARGB
    BOOST_REQUIRE_EQUAL(bmp.convertFormat(TextureFormat::BGRA), 0);
    BOOST_REQUIRE_EQUAL(bmpPl.convertFormat(TextureFormat::BGRA), 0);
    bmp.removePalette();
    bmpPl.removePalette();

    // ARGB -> ARGB: Plain copy
    BOOST_REQUIRE_EQUAL(bmp.print(&outBuffer[0], w, h, TextureFormat::BGRA), 0);
    BOOST_REQUIRE_EQUAL(bmp.print(&outBuffer2[0], w, h, TextureFormat::BGRA, modPal), 0);
    BOOST_TEST_REQUIRE(outBuffer2 == outBuffer, boost::test_tools::per_element());
    ;
    // Same if palette is set
    bmp.setPaletteCopy(*modPal);
    BOOST_REQUIRE_EQUAL(bmp.print(&outBuffer2[0], w, h, TextureFormat::BGRA, modPal), 0);
    BOOST_TEST_REQUIRE(outBuffer2 == outBuffer, boost::test_tools::per_element());
    ;

    BOOST_REQUIRE_EQUAL(bmpPl.print(&outBuffer[0], w, h, TextureFormat::BGRA, palette), 0);
    BOOST_REQUIRE_EQUAL(bmpPl.print(&outBuffer2[0], w, h, TextureFormat::BGRA, modPal), 0);
    BOOST_TEST_REQUIRE(outBuffer2 == outBuffer, boost::test_tools::per_element());
    ;
    bmpPl.setPaletteCopy(*modPal);
    BOOST_REQUIRE_EQUAL(bmpPl.print(&outBuffer2[0], w, h, TextureFormat::BGRA, modPal), 0);
    BOOST_TEST_REQUIRE(outBuffer2 == outBuffer, boost::test_tools::per_element());
    ;

    // ARGB -> Palette: Prefer passed palette
    outBuffer2.resize(inBuffer.size());
    BOOST_REQUIRE_EQUAL(bmp.print(&outBuffer2[0], w, h, TextureFormat::Paletted, palette), 0);
    BOOST_TEST_REQUIRE(outBuffer2 == inBuffer, boost::test_tools::per_element());
    ;
    bmp.setPaletteCopy(*palette);
    BOOST_REQUIRE_EQUAL(bmp.print(&outBuffer[0], w, h, TextureFormat::Paletted), 0);
    BOOST_TEST_REQUIRE(outBuffer2 == inBuffer, boost::test_tools::per_element());
    ;
    BOOST_REQUIRE_EQUAL(bmpPl.print(&outBuffer2[0], w, h, TextureFormat::Paletted, palette), 0);
    BOOST_TEST_REQUIRE(outBuffer2 == inBufferPl, boost::test_tools::per_element());
    ;
    bmpPl.setPaletteCopy(*palette);
    BOOST_REQUIRE_EQUAL(bmpPl.print(&outBuffer[0], w, h, TextureFormat::Paletted), 0);
    BOOST_TEST_REQUIRE(outBuffer2 == inBufferPl, boost::test_tools::per_element());
    ;

    // Error when no palette
    bmp.removePalette();
    bmpPl.removePalette();
    BOOST_REQUIRE_EQUAL(bmp.print(&outBuffer[0], w, h, TextureFormat::Paletted), ErrorCode::PALETTE_MISSING);
    BOOST_REQUIRE_EQUAL(bmpPl.print(&outBuffer[0], w, h, TextureFormat::Paletted), ErrorCode::PALETTE_MISSING);
}

BOOST_AUTO_TEST_CASE(CheckPalette)
{
    ArchivItem_Bitmap_Raw bmp;
    unsigned w = 10, h = 14;
    PixelBufferPaletted buffer(w, h, 22);
    BOOST_REQUIRE_EQUAL(bmp.create(buffer, palette), 0);
    BOOST_REQUIRE_EQUAL(bmp.convertFormat(TextureFormat::BGRA), 0);
    ArchivItem_Palette wrongPal;
    BOOST_REQUIRE(bmp.checkPalette(*palette));
    BOOST_REQUIRE(!bmp.checkPalette(wrongPal));
    // Let it find the color (only 1 used)
    ArchivItem_Palette okPal(wrongPal);
    okPal.set(22, palette->get(22));
    BOOST_REQUIRE(bmp.checkPalette(okPal));
    // All palettes are fine for transparent colors
    bmp.init(w, h, TextureFormat::BGRA);
    BOOST_REQUIRE(bmp.checkPalette(*palette));
    BOOST_REQUIRE(bmp.checkPalette(wrongPal));
    // Colored pixel but alpha == 0 -> Still transparent
    bmp.setPixel(0, 0, ColorBGRA(1, 2, 3, 0));
    BOOST_REQUIRE(bmp.checkPalette(*palette));
    BOOST_REQUIRE(bmp.checkPalette(wrongPal));
    // Colored pixel -> wrong
    bmp.setPixel(0, 0, ColorBGRA(2, 3, 1, 1));
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
    // First create paletted bmp
    ArchivItem_Bitmap_Raw bmpPal;
    // Any NULL should make this fail
    BOOST_REQUIRE_EQUAL(bmpPal.create(bw, bh, nullptr, w, h, TextureFormat::Paletted, palette), ErrorCode::INVALID_BUFFER);
    BOOST_REQUIRE_EQUAL(bmpPal.create(bw, bh, &inBufferPal[0], w, h, TextureFormat::Paletted, nullptr), ErrorCode::PALETTE_MISSING);
    // These create transparent bitmaps
    BOOST_REQUIRE_EQUAL(bmpPal.create(bw, bh, &inBufferPal[0], 0, h, TextureFormat::Paletted, palette), 0);
    BOOST_REQUIRE_EQUAL(bmpPal.getWidth(), bw);
    BOOST_REQUIRE_EQUAL(bmpPal.getHeight(), bh);
    BOOST_REQUIRE_EQUAL(bmpPal.create(bw, bh, &inBufferPal[0], w, 0, TextureFormat::Paletted, palette), 0);
    BOOST_REQUIRE_EQUAL(bmpPal.getWidth(), bw);
    BOOST_REQUIRE_EQUAL(bmpPal.getHeight(), bh);
    // These create empty bitmaps
    BOOST_REQUIRE_EQUAL(bmpPal.create(0, bh, &inBufferPal[0], w, h, TextureFormat::Paletted, palette), 0);
    BOOST_REQUIRE_EQUAL(bmpPal.getWidth(), 0u);
    BOOST_REQUIRE_EQUAL(bmpPal.getHeight(), 0u);
    BOOST_REQUIRE_EQUAL(bmpPal.create(bw, 0, &inBufferPal[0], w, h, TextureFormat::Paletted, palette), 0);
    BOOST_REQUIRE_EQUAL(bmpPal.getWidth(), 0u);
    BOOST_REQUIRE_EQUAL(bmpPal.getHeight(), 0u);

    // Succeed
    BOOST_REQUIRE_EQUAL(bmpPal.create(bw, bh, &inBufferPal[0], w, h, TextureFormat::Paletted, palette), 0);
    BOOST_REQUIRE_EQUAL(bmpPal.getWidth(), bw);
    BOOST_REQUIRE_EQUAL(bmpPal.getHeight(), bh);
    // Now write to paletted buffer
    // Fail on NULL
    BOOST_REQUIRE_EQUAL(bmpPal.print(nullptr, w, h, TextureFormat::Paletted), ErrorCode::INVALID_BUFFER);
    // Do nothing
    BOOST_REQUIRE_EQUAL(bmpPal.print(&outBufferPal[0], 0, h, TextureFormat::Paletted), 0);
    BOOST_TEST_REQUIRE(outBufferPal == emptyBufferPal, boost::test_tools::per_element());
    ;
    BOOST_REQUIRE_EQUAL(bmpPal.print(&outBufferPal[0], w, 0, TextureFormat::Paletted), 0);
    BOOST_TEST_REQUIRE(outBufferPal == emptyBufferPal, boost::test_tools::per_element());
    ;
    // Succeed
    BOOST_REQUIRE_EQUAL(bmpPal.print(&outBufferPal[0], w, h, TextureFormat::Paletted), 0);
    BOOST_TEST_REQUIRE(outBufferPal == inBufferPal, boost::test_tools::per_element());
    ;
    // Write to RGBA buffer
    BOOST_REQUIRE_EQUAL(bmpPal.print(&outBuffer[0], w, h, TextureFormat::BGRA), 0);
    BOOST_TEST_REQUIRE(outBuffer == inBufferRGB, boost::test_tools::per_element());
    ;

    // Test partial write
    unsigned xStart = 2, xStartB = 3, partW = 4, yStart = 1, yStartB = 5, partH = 6;
    std::fill(outBufferPal.begin(), outBufferPal.end(), 42u);
    std::fill(outBuffer.begin(), outBuffer.end(), 42u);
    BOOST_REQUIRE_EQUAL(
      bmpPal.print(&outBufferPal[0], w, h, TextureFormat::Paletted, nullptr, xStart, yStart, xStartB, yStartB, partW, partH), 0);
    BOOST_REQUIRE_EQUAL(bmpPal.print(&outBuffer[0], w, h, TextureFormat::BGRA, nullptr, xStart, yStart, xStartB, yStartB, partW, partH), 0);
    for(unsigned y = 0; y < h; y++)
    {
        for(unsigned x = 0; x < w; x++)
        {
            unsigned idxPal = x + y * w;
            unsigned idx = idxPal * 4;
            if(x < xStart || y < yStart || x >= xStart + partW || y >= yStart + partH)
            {
                BOOST_REQUIRE_EQUAL(outBufferPal[idxPal], 42u);
                BOOST_REQUIRE_EQUAL(ColorBGRA(&outBuffer[idx]), ColorBGRA(42, 42, 42, 42));
            } else
            {
                unsigned inBufferIdx = x - xStart + xStartB + (y - yStart + yStartB) * w;
                BOOST_REQUIRE_EQUAL(outBufferPal[idxPal], inBufferPal[inBufferIdx]);
                BOOST_REQUIRE_EQUAL(ColorBGRA(&outBuffer[idx]), ColorBGRA(&inBufferRGB[inBufferIdx * 4]));
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
    BOOST_REQUIRE_EQUAL(bmp.create(bw, bh, &inBuffer[0], w, h, TextureFormat::BGRA), 0);
    BOOST_REQUIRE_EQUAL(bmp.getWidth(), bw);
    BOOST_REQUIRE_EQUAL(bmp.getHeight(), bh);
    // Now write to paletted buffer
    // Succeed
    BOOST_REQUIRE_EQUAL(bmp.print(&outBufferPal[0], w, h, TextureFormat::Paletted, palette), 0);
    BOOST_TEST_REQUIRE(outBufferPal == inBufferPal, boost::test_tools::per_element());
    ;
    BOOST_REQUIRE_EQUAL(bmp.print(&outBuffer[0], w, h, TextureFormat::BGRA), 0);
    BOOST_TEST_REQUIRE(outBuffer == inBuffer, boost::test_tools::per_element());
    ;
}

BOOST_AUTO_TEST_CASE(TransparentTex)
{
    unsigned w = 10, h = 14;
    // Use different size for bitmap (bigger width, smaller height)
    unsigned bw = w + 3, bh = h - 2;
    std::vector<uint8_t> inBufferPal(w * h, palette->getTransparentIdx());
    // Buffer in (byte) BGRA format
    std::vector<uint8_t> inBuffer(inBufferPal.size() * 4u);
    // Create paletted
    ArchivItem_Bitmap_Raw bmpPal;
    ArchivItem_Bitmap_Player bmpPlayerPal;
    BOOST_REQUIRE_EQUAL(bmpPal.create(bw, bh, &inBufferPal[0], w, h, TextureFormat::Paletted, palette), 0);
    BOOST_REQUIRE_EQUAL(bmpPlayerPal.create(bw, bh, &inBufferPal[0], w, h, TextureFormat::Paletted, palette), 0);
    // Create BGRA
    ArchivItem_Bitmap_Raw bmp;
    ArchivItem_Bitmap_Player bmpPlayer;
    BOOST_REQUIRE_EQUAL(bmp.create(bw, bh, &inBuffer[0], w, h, TextureFormat::BGRA), 0);
    BOOST_REQUIRE_EQUAL(bmpPlayer.create(bw, bh, &inBuffer[0], w, h, TextureFormat::BGRA, palette), 0);
    // Writing should not change buffer
    std::vector<uint8_t> outBuffer(inBuffer.size(), 42);
    std::vector<uint8_t> outBufferCheck(outBuffer);
    BOOST_REQUIRE_EQUAL(bmpPal.print(&outBuffer[0], w, h, TextureFormat::Paletted), 0);
    BOOST_TEST_REQUIRE(outBuffer == outBufferCheck, boost::test_tools::per_element());
    ;
    BOOST_REQUIRE_EQUAL(bmpPlayerPal.print(&outBuffer[0], w, h, TextureFormat::Paletted), 0);
    BOOST_TEST_REQUIRE(outBuffer == outBufferCheck, boost::test_tools::per_element());
    ;

    BOOST_REQUIRE_EQUAL(bmpPal.print(&outBuffer[0], w, h, TextureFormat::BGRA), 0);
    BOOST_TEST_REQUIRE(outBuffer == outBufferCheck, boost::test_tools::per_element());
    ;
    BOOST_REQUIRE_EQUAL(bmpPlayerPal.print(&outBuffer[0], w, h, TextureFormat::BGRA), 0);
    BOOST_TEST_REQUIRE(outBuffer == outBufferCheck, boost::test_tools::per_element());
    ;

    BOOST_REQUIRE_EQUAL(bmp.print(&outBuffer[0], w, h, TextureFormat::Paletted, palette), 0);
    BOOST_TEST_REQUIRE(outBuffer == outBufferCheck, boost::test_tools::per_element());
    ;
    BOOST_REQUIRE_EQUAL(bmpPlayer.print(&outBuffer[0], w, h, TextureFormat::Paletted, palette), 0);
    BOOST_TEST_REQUIRE(outBuffer == outBufferCheck, boost::test_tools::per_element());
    ;

    BOOST_REQUIRE_EQUAL(bmp.print(&outBuffer[0], w, h, TextureFormat::BGRA), 0);
    BOOST_TEST_REQUIRE(outBuffer == outBufferCheck, boost::test_tools::per_element());
    ;
    BOOST_REQUIRE_EQUAL(bmpPlayer.print(&outBuffer[0], w, h, TextureFormat::BGRA, palette), 0);
    BOOST_TEST_REQUIRE(outBuffer == outBufferCheck, boost::test_tools::per_element());
    ;
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

    // First create paletted bmp
    ArchivItem_Bitmap_Player bmpPal;
    // Any NULL should make this fail
    BOOST_REQUIRE_EQUAL(bmpPal.create(bw, bh, nullptr, w, h, TextureFormat::Paletted, palette, playerClrStart), ErrorCode::INVALID_BUFFER);
    BOOST_REQUIRE_EQUAL(bmpPal.create(bw, bh, &inBufferPal[0], w, h, TextureFormat::Paletted, nullptr, playerClrStart),
                        ErrorCode::PALETTE_MISSING);
    // These create transparent bitmaps
    BOOST_REQUIRE_EQUAL(bmpPal.create(bw, bh, &inBufferPal[0], 0, h, TextureFormat::Paletted, palette, playerClrStart), 0);
    BOOST_REQUIRE_EQUAL(bmpPal.getWidth(), bw);
    BOOST_REQUIRE_EQUAL(bmpPal.getHeight(), bh);
    BOOST_REQUIRE_EQUAL(bmpPal.create(bw, bh, &inBufferPal[0], w, 0, TextureFormat::Paletted, palette, playerClrStart), 0);
    BOOST_REQUIRE_EQUAL(bmpPal.getWidth(), bw);
    BOOST_REQUIRE_EQUAL(bmpPal.getHeight(), bh);
    // These create empty bitmaps
    BOOST_REQUIRE_EQUAL(bmpPal.create(0, bh, &inBufferPal[0], w, h, TextureFormat::Paletted, palette, playerClrStart), 0);
    BOOST_REQUIRE_EQUAL(bmpPal.getWidth(), 0u);
    BOOST_REQUIRE_EQUAL(bmpPal.getHeight(), 0u);
    BOOST_REQUIRE_EQUAL(bmpPal.create(bw, 0, &inBufferPal[0], w, h, TextureFormat::Paletted, palette, playerClrStart), 0);
    BOOST_REQUIRE_EQUAL(bmpPal.getWidth(), 0u);
    BOOST_REQUIRE_EQUAL(bmpPal.getHeight(), 0u);
    // Succeed
    BOOST_REQUIRE_EQUAL(bmpPal.create(bw, bh, &inBufferPal[0], w, h, TextureFormat::Paletted, palette, playerClrStart), 0);
    BOOST_REQUIRE_EQUAL(bmpPal.getWidth(), bw);
    BOOST_REQUIRE_EQUAL(bmpPal.getHeight(), bh);
    // Now write to paletted buffer
    // Fail on NULL
    BOOST_REQUIRE_EQUAL(bmpPal.print(nullptr, w, h, TextureFormat::Paletted, nullptr, playerClrStart), ErrorCode::INVALID_BUFFER);
    BOOST_REQUIRE_EQUAL(bmpPal.print(&outBufferPal[0], 0, h, TextureFormat::Paletted, palette, playerClrStart), 0);
    BOOST_TEST_REQUIRE(outBufferPal == emptyBufferPal, boost::test_tools::per_element());
    ;
    BOOST_REQUIRE_EQUAL(bmpPal.print(&outBufferPal[0], w, 0, TextureFormat::Paletted, palette, playerClrStart), 0);
    BOOST_TEST_REQUIRE(outBufferPal == emptyBufferPal, boost::test_tools::per_element());
    ;
    // Succeed
    BOOST_REQUIRE_EQUAL(bmpPal.print(&outBufferPal[0], w, h, TextureFormat::Paletted, palette, playerClrStart), 0);
    BOOST_TEST_REQUIRE(outBufferPal == inBufferPal, boost::test_tools::per_element());
    ;
    // Write to RGBA buffer
    BOOST_REQUIRE_EQUAL(bmpPal.print(&outBuffer[0], w, h, TextureFormat::BGRA, palette, playerClrStart), 0);
    BOOST_TEST_REQUIRE(outBuffer == inBufferRGB, boost::test_tools::per_element());
    ;

    // Test partial write
    unsigned xStart = 2, xStartB = 3, partW = 4, yStart = 1, yStartB = 5, partH = 6;
    std::fill(outBufferPal.begin(), outBufferPal.end(), 42u);
    std::fill(outBuffer.begin(), outBuffer.end(), 42u);
    BOOST_REQUIRE_EQUAL(bmpPal.print(&outBufferPal[0], w, h, TextureFormat::Paletted, palette, playerClrStart, xStart, yStart, xStartB,
                                     yStartB, partW, partH),
                        0);
    BOOST_REQUIRE_EQUAL(
      bmpPal.print(&outBuffer[0], w, h, TextureFormat::BGRA, palette, playerClrStart, xStart, yStart, xStartB, yStartB, partW, partH), 0);
    for(unsigned y = 0; y < h; y++)
    {
        for(unsigned x = 0; x < w; x++)
        {
            unsigned idxPal = x + y * w;
            unsigned idx = idxPal * 4;
            if(x < xStart || y < yStart || x >= xStart + partW || y >= yStart + partH)
            {
                BOOST_REQUIRE_EQUAL(outBufferPal[idxPal], 42u);
                BOOST_REQUIRE_EQUAL(ColorBGRA(&outBuffer[idx]), ColorBGRA(42, 42, 42, 42));
            } else
            {
                unsigned inBufferIdx = x - xStart + xStartB + (y - yStart + yStartB) * w;
                BOOST_REQUIRE_EQUAL(outBufferPal[idxPal], inBufferPal[inBufferIdx]);
                BOOST_REQUIRE_EQUAL(ColorBGRA(&outBuffer[idx]), ColorBGRA(&inBufferRGB[inBufferIdx * 4]));
            }
        }
    }

    // Test the same but create from BGRA buffer
    ArchivItem_Bitmap_Player bmp;
    std::fill(outBufferPal.begin(), outBufferPal.end(), 42u);
    std::fill(outBuffer.begin(), outBuffer.end(), 42u);
    std::fill(outBufferPal.begin() + bh * w, outBufferPal.end(), 0u);
    std::fill(outBuffer.begin() + bh * w * 4, outBuffer.end(), 0u);
    BOOST_REQUIRE_EQUAL(bmp.create(bw, bh, &inBuffer[0], w, h, TextureFormat::BGRA, palette, playerClrStart), 0);
    BOOST_REQUIRE_EQUAL(bmp.getWidth(), bw);
    BOOST_REQUIRE_EQUAL(bmp.getHeight(), bh);

    BOOST_REQUIRE_EQUAL(bmp.print(&outBufferPal[0], w, h, TextureFormat::Paletted, palette, playerClrStart), 0);
    BOOST_TEST_REQUIRE(outBufferPal == inBufferPal, boost::test_tools::per_element());
    ;
    BOOST_REQUIRE_EQUAL(bmp.print(&outBuffer[0], w, h, TextureFormat::BGRA, palette, playerClrStart), 0);
    BOOST_TEST_REQUIRE(outBuffer == inBuffer, boost::test_tools::per_element());
    ;
}

BOOST_AUTO_TEST_CASE(CreatePrintPlayerBitmapPaletted)
{
    PixelBufferPaletted inBuffer(5, 4, 10);
    const uint8_t playerClrStart = 200;
    std::iota(inBuffer.begin(), inBuffer.begin() + 4, playerClrStart);

    ArchivItem_Bitmap_Player bmp;
    BOOST_REQUIRE_EQUAL(bmp.create(inBuffer.getWidth() + 2, inBuffer.getHeight() + 6, inBuffer, palette, playerClrStart), 0);

    // Try with original and another player color
    for(const uint8_t curPlrClr : {playerClrStart + 0u, 210u})
    {
        // Adjust expected result
        std::iota(inBuffer.begin(), inBuffer.begin() + 4, curPlrClr);
        // Print to paletted buffer
        {
            PixelBufferPaletted outBuffer(inBuffer.getWidth(), inBuffer.getHeight(), 42);
            BOOST_REQUIRE_EQUAL(bmp.print(outBuffer, nullptr, curPlrClr), 0);
            BOOST_TEST_REQUIRE(outBuffer == inBuffer, boost::test_tools::per_element());
            ;
        }
        // Print to RGB buffer
        {
            PixelBufferBGRA outBuffer(inBuffer.getWidth(), inBuffer.getHeight(), randColor());
            BOOST_REQUIRE_EQUAL(bmp.print(outBuffer, nullptr, curPlrClr), 0);
            for(unsigned i = 0; i < outBuffer.getNumPixels(); i++)
            {
                BOOST_TEST_INFO("Position " << i);
                BOOST_TEST(outBuffer.get(i) == palette->get(inBuffer.get(i)));
            }
        }
        // Just player colors
        // Print to paletted buffer
        {
            PixelBufferPaletted outBuffer(inBuffer.getWidth(), inBuffer.getHeight(), 42);
            BOOST_REQUIRE_EQUAL(bmp.print(outBuffer, nullptr, curPlrClr, 0, 0, 0, 0, 0, 0, true), 0);
            for(unsigned i = 0; i < outBuffer.getNumPixels(); i++)
            {
                BOOST_TEST_INFO("Position " << i);
                const auto expected = i < 4 ? inBuffer.get(i) : 42;
                BOOST_REQUIRE_EQUAL(outBuffer.get(i), expected);
            }
        }
        // Print to RGB buffer
        {
            const auto initialClr = randColor();
            PixelBufferBGRA outBuffer(inBuffer.getWidth(), inBuffer.getHeight(), initialClr);
            BOOST_REQUIRE_EQUAL(bmp.print(outBuffer, nullptr, curPlrClr, 0, 0, 0, 0, 0, 0, true), 0);
            for(unsigned i = 0; i < outBuffer.getNumPixels(); i++)
            {
                BOOST_TEST_INFO("Position " << i);
                const auto expected = i < 4 ? ColorBGRA(palette->get(inBuffer.get(i))) : initialClr;
                BOOST_TEST(outBuffer.get(i) == expected);
            }
        }
    }
}

BOOST_AUTO_TEST_CASE(CreatePrintPlayerBitmapARGB)
{
    PixelBufferBGRA inBuffer(5, 4, palette->get(3));
    const uint8_t playerClrStart = 200;
    for(int i = 0; i < 4; i++)
        inBuffer.set(i, palette->get(i + playerClrStart));

    ArchivItem_Bitmap_Player bmp;
    BOOST_REQUIRE_EQUAL(bmp.create(inBuffer.getWidth() + 2, inBuffer.getHeight() + 6, inBuffer, palette, playerClrStart), 0);

    // Try with original and another player color
    for(const uint8_t curPlrClr : {playerClrStart + 0u, 210u})
    {
        // Adjust expected result
        for(int i = 0; i < 4; i++)
            inBuffer.set(i, palette->get(i + curPlrClr));
        // Print to paletted buffer
        {
            PixelBufferPaletted outBuffer(inBuffer.getWidth(), inBuffer.getHeight(), 42);
            BOOST_REQUIRE_EQUAL(bmp.print(outBuffer, palette, curPlrClr), 0);
            for(unsigned i = 0; i < outBuffer.getNumPixels(); i++)
            {
                BOOST_TEST_INFO("Position " << i);
                BOOST_TEST(outBuffer.get(i) == palette->lookup(inBuffer.get(i)));
            }
        }
        // Print to RGB buffer
        {
            PixelBufferBGRA outBuffer(inBuffer.getWidth(), inBuffer.getHeight(), randColor());
            BOOST_REQUIRE_EQUAL(bmp.print(outBuffer, palette, curPlrClr), 0);
            BOOST_TEST_REQUIRE(outBuffer == inBuffer, boost::test_tools::per_element());
            ;
        }
        // Just player colors
        // Print to paletted buffer
        {
            PixelBufferPaletted outBuffer(inBuffer.getWidth(), inBuffer.getHeight(), 42);
            BOOST_REQUIRE_EQUAL(bmp.print(outBuffer, palette, curPlrClr, 0, 0, 0, 0, 0, 0, true), 0);
            for(unsigned i = 0; i < outBuffer.getNumPixels(); i++)
            {
                BOOST_TEST_INFO("Position " << i);
                const auto expected = i < 4 ? palette->lookup(inBuffer.get(i)) : 42;
                BOOST_REQUIRE_EQUAL(outBuffer.get(i), expected);
            }
        }
        // Print to RGB buffer
        {
            const auto initialClr = randColor();
            PixelBufferBGRA outBuffer(inBuffer.getWidth(), inBuffer.getHeight(), initialClr);
            BOOST_REQUIRE_EQUAL(bmp.print(outBuffer, palette, curPlrClr, 0, 0, 0, 0, 0, 0, true), 0);
            for(unsigned i = 0; i < outBuffer.getNumPixels(); i++)
            {
                BOOST_TEST_INFO("Position " << i);
                const auto expected = i < 4 ? inBuffer.get(i) : initialClr;
                BOOST_TEST(outBuffer.get(i) == expected);
            }
        }
    }
}

namespace {
struct PrintParams
{
    unsigned toX, toY, fromX, fromW, fromY, fromH;
};
} // namespace

BOOST_AUTO_TEST_CASE(PrintPartOfPlayerBitmap)
{
    PixelBufferBGRA inBuffer(23, 37);
    const uint8_t playerClrStart = 200;
    const auto seed = std ::random_device{}();
    std::mt19937 mt(seed);
    std::uniform_int_distribution<> distr(0, 255);
    std::generate(inBuffer.begin(), inBuffer.end(), [&]() { return ColorBGRA(this->palette->get(distr(mt)), distr(mt)); });

    ArchivItem_Bitmap_Player bmp;
    BOOST_REQUIRE_EQUAL(bmp.create(inBuffer.getWidth() + 2, inBuffer.getHeight() + 6, inBuffer, palette, playerClrStart), 0);

    PixelBufferBGRA outBuffer(inBuffer.getWidth() * 2, inBuffer.getHeight() * 2);

    std::uniform_int_distribution<unsigned> dw(1, outBuffer.getWidth() - 1);
    std::uniform_int_distribution<unsigned> dh(1, outBuffer.getHeight() - 1);
    std::uniform_int_distribution<unsigned> dw2(1, bmp.getWidth() / 2);
    std::uniform_int_distribution<unsigned> dh2(1, bmp.getHeight() / 2);

    std::vector<PrintParams> testParams;
    // Full
    testParams.emplace_back(PrintParams{});
    testParams.emplace_back(PrintParams{0, 0, 0, bmp.getWidth(), 0, bmp.getHeight()});
    // Partial with auto full width (zero)
    testParams.emplace_back(PrintParams{dw(mt), dh(mt), dw2(mt), 0, dh2(mt), 0});
    // Corner cases
    testParams.emplace_back(PrintParams{outBuffer.getWidth(), outBuffer.getHeight(), 0, bmp.getWidth(), 0, bmp.getHeight()});
    testParams.emplace_back(PrintParams{dw(mt), dh(mt), bmp.getWidth() / 2u, bmp.getWidth(), bmp.getHeight() / 2u, bmp.getHeight()});
    testParams.emplace_back(PrintParams{dw(mt), dh(mt), bmp.getWidth(), bmp.getWidth(), bmp.getHeight(), bmp.getHeight()});
    testParams.emplace_back(PrintParams{dw(mt), dh(mt), bmp.getWidth(), bmp.getWidth(), 0, 0});
    {
        // Inside
        testParams.emplace_back(PrintParams{dw(mt), dh(mt), dw2(mt), dw2(mt), dh2(mt), dh2(mt)});
    }

    for(const auto& p : testParams)
    {
        std::generate(outBuffer.begin(), outBuffer.end(), [&]() { return randColor(); });
        auto const outBufferIn = outBuffer;

        const uint8_t playerClrStart2 = 234;
        BOOST_TEST_REQUIRE(bmp.print(outBuffer, palette, playerClrStart2, p.toX, p.toY, p.fromX, p.fromY, p.fromW, p.fromH) == 0);
        // Zero width or height means full
        const auto fromW = p.fromW ? p.fromW : bmp.getWidth();
        const auto fromH = p.fromH ? p.fromH : bmp.getHeight();
        for(unsigned y = 0; y < outBuffer.getHeight(); ++y)
        {
            for(unsigned x = 0; x < outBuffer.getWidth(); ++x)
            {
                if(x == 20 && y == 42)
                    BOOST_TEST_INFO("Seed: " << seed << "; Position " << x << "x" << y);
                ColorBGRA expectedColor;
                if(x < p.toX || y < p.toY || x >= p.toX + fromW || y >= p.toY + fromH)
                    expectedColor = outBufferIn.get(x, y);
                else
                {
                    const unsigned bmpX = x - p.toX + p.fromX;
                    const unsigned bmpY = y - p.toY + p.fromY;
                    if(bmpX < bmp.getWidth() && bmpY < bmp.getHeight())
                    {
                        expectedColor = bmp.getPixel(bmpX, bmpY);
                        if(bmp.isPlayerColor(bmpX, bmpY))
                        {
                            const auto palClr = palette->get(bmp.getPlayerColorIdx(bmpX, bmpY) + playerClrStart2);
                            expectedColor = ColorBGRA(palClr, expectedColor.getAlpha());
                        } else
                        {
                            if(expectedColor.getAlpha() == 0)
                                expectedColor = outBufferIn.get(x, y);
                        }
                    } else
                        expectedColor = outBufferIn.get(x, y);
                }
                BOOST_TEST(outBuffer.get(x, y) == expectedColor);
            }
        }
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
        BOOST_REQUIRE_EQUAL(bmp.create(bw, bh, &inBufferPal[0], w, h, TextureFormat::Paletted, palette), 0);
        bmp.getVisibleArea(vis.x, vis.y, vis.w, vis.h);
        BOOST_REQUIRE_EQUAL(vis, Rect(0, 0, 0, 0));
        Rect vis2;
        // Test empty bmp
        BOOST_REQUIRE_EQUAL(bmpRaw.create(bw, bh, &inBufferPal[0], w, h, TextureFormat::Paletted, palette), 0);
        bmpRaw.getVisibleArea(vis2.x, vis2.y, vis2.w, vis2.h);
        BOOST_REQUIRE_EQUAL(vis2, Rect(0, 0, 0, 0));
    }

    std::vector<Rect> testRects = {// left-top          top               right-top
                                   Rect(0, 0, 1, 1), Rect(2, 0, 1, 1), Rect(w - 1, 0, 1, 1),
                                   // left            middle            right
                                   Rect(0, 3, 1, 1), Rect(3, 2, 1, 1), Rect(w - 1, 3, 1, 1),
                                   // left-bottom     bottom-middle     bottom-right
                                   Rect(0, h - 1, 1, 1), Rect(3, h - 1, 1, 1), Rect(w - 1, h - 1, 1, 1),
                                   // doublePixelRects;
                                   Rect(0, 0, 2, 1), Rect(0, 0, 1, 2), Rect(2, 0, 1, 3), Rect(2, 0, 3, 1), Rect(w - 3, 0, 3, 1),
                                   Rect(2, 0, 1, 3), Rect(2, 0, 3, 1), Rect(3, 2, 3, 1), Rect(3, 2, 1, 3), Rect(0, h - 3, 2, 1),
                                   Rect(0, h - 3, 1, 3), Rect(3, h - 3, 3, 1), Rect(3, h - 3, 1, 3), Rect(w - 4, h - 3, 4, 3),
                                   Rect(0, 0, w, h)};

    for(const Rect& rect : testRects)
    {
        // Test for non-player-color (127) and player color (128)
        for(unsigned i : {127, 128})
        {
            inBufferPal[rect.x + rect.y * w] = i;
            inBufferPal[rect.x + rect.w - 1 + (rect.y + rect.h - 1) * w] = i;
            BOOST_REQUIRE_EQUAL(bmp.create(bw, bh, &inBufferPal[0], w, h, TextureFormat::Paletted, palette), 0);
            Rect vis;
            bmp.getVisibleArea(vis.x, vis.y, vis.w, vis.h);
            BOOST_REQUIRE_EQUAL(vis, rect);

            BOOST_REQUIRE_EQUAL(bmpRaw.create(bw, bh, &inBufferPal[0], w, h, TextureFormat::Paletted, palette), 0);
            Rect visRaw;
            bmpRaw.getVisibleArea(visRaw.x, visRaw.y, visRaw.w, visRaw.h);
            BOOST_REQUIRE_EQUAL(visRaw, rect);

            inBufferPal[rect.x + rect.y * w] = palette->getTransparentIdx();
            inBufferPal[rect.x + rect.w - 1 + (rect.y + rect.h - 1) * w] = palette->getTransparentIdx();

            // Buffer in (byte) BGRA format
            std::vector<uint8_t> inBuffer(inBufferPal.size() * 4u, 0);
            BOOST_REQUIRE_EQUAL(bmp.print(&inBuffer[0], w, h, TextureFormat::BGRA), 0);
            ArchivItem_Bitmap_Player bmp2;
            BOOST_REQUIRE_EQUAL(bmp2.create(bw, bh, &inBuffer[0], w, h, TextureFormat::BGRA, palette), 0);
            Rect vis2;
            bmp2.getVisibleArea(vis2.x, vis2.y, vis2.w, vis2.h);
            BOOST_REQUIRE_EQUAL(vis2, rect);

            BOOST_REQUIRE_EQUAL(bmpRaw.create(bw, bh, &inBuffer[0], w, h, TextureFormat::BGRA), 0);
            Rect visRaw2;
            bmpRaw.getVisibleArea(visRaw2.x, visRaw2.y, visRaw2.w, visRaw2.h);
            BOOST_REQUIRE_EQUAL(visRaw2, rect);
            inBufferPal[rect.x + rect.y * w] = palette->getTransparentIdx();
        }
    }
}

BOOST_AUTO_TEST_SUITE_END()
