// Copyright (c) 2005 - 2020 Settlers Freaks (sf-team at siedler25.org)
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

#include "ArchivItem_BitmapBase.h"
#include "ArchivItem_Palette.h"
#include "ColorBGRA.h"
#include "ErrorCodes.h"
#include "PixelBufferBGRA.h"
#include "PixelBufferPaletted.h"
#include "libsiedler2.h"
#include <stdexcept>

namespace libsiedler2 {
/** @class ArchivItem_BitmapBase
 *
 *  Basis-Basisklasse für Bitmapitems.
 */

ArchivItem_BitmapBase::ArchivItem_BitmapBase()
    : nx_(0), ny_(0), width_(0), height_(0), palette_(nullptr), format_(TextureFormat::BGRA)
{}

ArchivItem_BitmapBase::ArchivItem_BitmapBase(const ArchivItem_BitmapBase& item) : ArchivItem(item)
{
    nx_ = item.nx_;
    ny_ = item.ny_;

    width_ = item.width_;
    height_ = item.height_;

    pxlData_ = item.pxlData_;

    palette_ = nullptr;
    if(item.palette_)
        setPaletteCopy(*item.palette_);
    format_ = item.format_;
}

ArchivItem_BitmapBase::~ArchivItem_BitmapBase() = default;

/**
 *  setzt einen Pixel auf einen bestimmten Wert.
 *
 *  @param[in] x       X Koordinate des Pixels
 *  @param[in] y       Y Koordinate des Pixels
 *  @param[in] color   Farbe des Pixels
 *  @param[in] palette Grundpalette
 */
void ArchivItem_BitmapBase::setPixel(uint16_t x, uint16_t y, uint8_t colorIdx)
{
    assert(x < width_ && y < height_);

    uint8_t* pxlPtr = getPixelPtr(x, y);
    if(getFormat() == TextureFormat::Paletted)
        *pxlPtr = colorIdx;
    else
    {
        assert(palette_);
        // RGB+A setzen
        if(palette_->isTransparent(colorIdx)) // Transparenz
            pxlPtr[3] = 0x00;
        else
            ColorBGRA(palette_->get(colorIdx)).toBGRA(pxlPtr);
    }
}

/**
 *  setzt einen Pixel auf einen bestimmten Wert.
 *
 *  @param[in] x X Koordinate des Pixels
 *  @param[in] y Y Koordinate des Pixels
 *  @param[in] r Roter Wert
 *  @param[in] g Grüner Wert
 *  @param[in] b Blauer Wert
 *  @param[in] a Alpha Wert (bei paletted nur 0xFF/0x00 unterstützt)
 */
void ArchivItem_BitmapBase::setPixel(uint16_t x, uint16_t y, const ColorBGRA clr)
{
    assert(x < width_ && y < height_);

    uint8_t* pxlPtr = getPixelPtr(x, y);
    if(getFormat() == TextureFormat::Paletted)
    {
        // Palettenindex setzen
        if(clr.getAlpha() == 0)
            *pxlPtr = palette_->getTransparentIdx();
        else
            *pxlPtr = palette_->lookup(clr);
    } else
        clr.toBGRA(pxlPtr);
}

/**
 *  liefert einen Pixel an einem bestimmten Punkt.
 *
 *  @param[in] x       X Koordinate des Pixels
 *  @param[in] y       Y Koordinate des Pixels
 *  @param[in] palette Grundpalette
 *
 *  @return liefert die Farbe des Pixels
 */
uint8_t ArchivItem_BitmapBase::getPixelClrIdx(uint16_t x, uint16_t y) const
{
    return getPixelClrIdx(x, y, palette_.get());
}

uint8_t ArchivItem_BitmapBase::getPixelClrIdx(uint16_t x, uint16_t y, const ArchivItem_Palette* palette) const
{
    assert(x < width_ && y < height_);

    if(getFormat() == TextureFormat::Paletted)
        return getPalettedPixel(x, y);
    else
    {
        assert(palette);
        ColorBGRA clr = getARGBPixel(x, y);
        // Index von RGB+A liefern
        if(clr.getAlpha() == 0) // Transparenz
            return palette->getTransparentIdx();
        else
            return palette->lookup(clr);
    }
}

ColorBGRA ArchivItem_BitmapBase::getPixel(uint16_t x, uint16_t y) const
{
    assert(x < width_ && y < height_);

    if(getFormat() == TextureFormat::Paletted)
    {
        uint8_t pxlVal = getPalettedPixel(x, y);
        return (palette_->isTransparent(pxlVal)) ? ColorBGRA() : ColorBGRA(palette_->get(pxlVal));
    } else
        return getARGBPixel(x, y);
}

uint8_t* ArchivItem_BitmapBase::getPixelPtr(uint16_t x, uint16_t y)
{
    return &pxlData_[(y * width_ + x) * getBBP()];
}

const uint8_t* ArchivItem_BitmapBase::getPixelPtr(uint16_t x, uint16_t y) const
{
    return &pxlData_[(y * width_ + x) * getBBP()];
}

uint8_t ArchivItem_BitmapBase::getPalettedPixel(uint16_t x, uint16_t y) const
{
    assert(format_ == TextureFormat::Paletted);
    return pxlData_[y * width_ + x];
}

ColorBGRA ArchivItem_BitmapBase::getARGBPixel(uint16_t x, uint16_t y) const
{
    assert(format_ == TextureFormat::BGRA);
    return ColorBGRA::fromBGRA(&pxlData_[(y * width_ + x) * 4u]);
}

PixelBufferPalettedRef ArchivItem_BitmapBase::getBufferPaletted() const
{
    if(getFormat() != TextureFormat::Paletted)
        throw std::logic_error("Image not paletted");
    assert(palette_);
    return PixelBufferPalettedRef(const_cast<uint8_t*>(pxlData_.data()), width_, height_, *palette_);
}

PixelBufferBGRARef ArchivItem_BitmapBase::getBufferARGB() const
{
    if(getFormat() != TextureFormat::BGRA)
        throw std::logic_error("Image not BGRA");
    return PixelBufferBGRARef(reinterpret_cast<uint32_t*>(const_cast<uint8_t*>(pxlData_.data())), width_, height_);
}

TextureFormat ArchivItem_BitmapBase::getWantedFormat(TextureFormat origFormat)
{
    TextureFormat globFmt = getGlobalTextureFormat();
    if(globFmt == TextureFormat::Original)
        return origFormat;
    else
        return globFmt;
}

void ArchivItem_BitmapBase::init(int16_t width, int16_t height, TextureFormat format)
{
    if(format == TextureFormat::Original)
        throw std::logic_error("Must set the actual texture format!");

    clear();
    // Consistency: width == 0 <=> height == 0
    if(width == 0)
        height = 0;
    else if(height == 0)
        width = 0;

    if(format == TextureFormat::Paletted && !palette_)
        throw std::runtime_error("Palette is missing");

    width_ = width;
    height_ = height;
    format_ = format;

    uint8_t clear = (format == TextureFormat::Paletted) ? palette_->getTransparentIdx() : 0; //-V522

    pxlData_.resize(width_ * height_ * getBBP(), clear);
}

void ArchivItem_BitmapBase::init(int16_t width, int16_t height, TextureFormat format, const ArchivItem_Palette* newPal)
{
    if(format == TextureFormat::Paletted && !newPal)
        throw std::runtime_error("Palette is missing");
    // Set new format to BGRA to allow removing of palette
    if(format == TextureFormat::BGRA)
        format_ = TextureFormat::BGRA;
    if(newPal)
        setPaletteCopy(*newPal);
    else
        removePalette();
    init(width, height, format);
}

/**
 *  räumt den Bildspeicher auf.
 */
void ArchivItem_BitmapBase::clear()
{
    width_ = 0;
    height_ = 0;
    pxlData_.clear();
}

/**
 *  liefert den X-Nullpunkt.
 *
 *  @return X-Nullpunkt
 */
int16_t ArchivItem_BitmapBase::getNx() const
{
    return nx_;
}

/**
 *  liefert den Y-Nullpunkt.
 *
 *  @return Y-Nullpunkt
 */
int16_t ArchivItem_BitmapBase::getNy() const
{
    return ny_;
}

/**
 *  setzt den X-Nullpunkt.
 *
 *  @param[in] nx X-Nullpunkt
 */
void ArchivItem_BitmapBase::setNx(int16_t nx)
{
    this->nx_ = nx;
}

/**
 *  setzt den Y-Nullpunkt.
 *
 *  @param[in] ny Y-Nullpunkt
 */
void ArchivItem_BitmapBase::setNy(int16_t ny)
{
    this->ny_ = ny;
}

int ArchivItem_BitmapBase::convertFormat(TextureFormat newFormat)
{
    // Nothing to do
    if(newFormat == TextureFormat::Original || newFormat == format_)
        return ErrorCode::NONE;

    if(!palette_)
        return ErrorCode::PALETTE_MISSING;
    if(newFormat == TextureFormat::BGRA)
    {
        PixelBufferBGRA newBuffer(width_, height_);
        for(unsigned y = 0; y < height_; y++)
        {
            for(unsigned x = 0; x < width_; x++)
                newBuffer.set(x, y, getPixel(x, y));
        }
        pxlData_.assign(newBuffer.getPixelPtr(), newBuffer.getPixelPtr() + newBuffer.getSizeInBytes());
    } else
    {
        PixelBufferPaletted newBuffer(width_, height_);
        for(unsigned y = 0; y < height_; y++)
        {
            for(unsigned x = 0; x < width_; x++)
            {
                ColorBGRA clr = getARGBPixel(x, y);
                newBuffer.set(x, y, clr.getAlpha() == 0 ? palette_->getTransparentIdx() : palette_->lookup(clr));
            }
        }
        pxlData_.assign(newBuffer.getPixelPtr(), newBuffer.getPixelPtr() + newBuffer.getSizeInBytes());
    }
    format_ = newFormat;
    return ErrorCode::NONE;
}

void ArchivItem_BitmapBase::getVisibleArea(int& vx, int& vy, unsigned& vw, unsigned& vh) const
{
    if((width_ == 0) || (height_ == 0))
    {
        vx = vy = vw = vh = 0;
        return;
    }

    const ArchivItem_Palette* palette = getPalette();
    if(getBBP() == 1 && !palette->hasTransparency())
    {
        vx = vy = 0;
        vw = width_;
        vh = height_;
        return;
    }

    if(getBBP() == 1)
        doGetVisibleArea(vx, vy, vw, vh, [this, palette](auto x, auto y) {
            return palette->isTransparent(this->getPalettedPixel(x, y));
        });
    else
        doGetVisibleArea(vx, vy, vw, vh, [this](auto x, auto y) { return this->getPixelPtr(x, y)[3] == 0u; });
}

bool ArchivItem_BitmapBase::checkPalette(const ArchivItem_Palette& palette) const
{
    if(format_ == TextureFormat::Paletted)
        return true;
    for(unsigned y = 0; y < height_; y++)
    {
        for(unsigned x = 0; x < width_; x++)
        {
            ColorBGRA clr = getARGBPixel(x, y);
            uint8_t dummyIdx;
            if(clr.getAlpha() != 0 && !palette.lookup(clr, dummyIdx))
                return false;
        }
    }
    return true;
}

/**
 *  setzt die Grundpalette.
 *
 *  @param[in] palette Palette die zukünftig verwendet werden soll.
 */
void ArchivItem_BitmapBase::setPalette(std::unique_ptr<ArchivItem_Palette> palette)
{
    if(!palette && format_ == TextureFormat::Paletted)
        throw std::runtime_error("Cannot remove palette from paletted image");
    palette_ = std::move(palette);
}

void ArchivItem_BitmapBase::setPaletteCopy(const ArchivItem_Palette& palette)
{
    if(palette_.get() == &palette)
        return; // Optimization for setting self palette
    setPalette(libsiedler2::clone(palette));
}

void ArchivItem_BitmapBase::removePalette()
{
    setPalette(nullptr);
}

} // namespace libsiedler2
