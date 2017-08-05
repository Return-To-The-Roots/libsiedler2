// Copyright (c) 2005 - 2017 Settlers Freaks (sf-team at siedler25.org)
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

#include "libSiedler2Defines.h" // IWYU pragma: keep
#include "ArchivItem_BitmapBase.h"
#include "ArchivItem_Palette.h"
#include "ColorARGB.h"
#include "ErrorCodes.h"
#include "IAllocator.h"
#include "PixelBufferARGB.h"
#include "PixelBufferPaletted.h"
#include "libsiedler2.h"
#include <stdexcept>

namespace libsiedler2 {
/** @class ArchivItem_BitmapBase
 *
 *  Basis-Basisklasse für Bitmapitems.
 */

ArchivItem_BitmapBase::ArchivItem_BitmapBase() : ArchivItem(), nx_(0), ny_(0), width_(0), height_(0), palette_(NULL), format_(FORMAT_BGRA)
{
}

ArchivItem_BitmapBase::ArchivItem_BitmapBase(const ArchivItem_BitmapBase& item) : ArchivItem(item)
{
    nx_ = item.nx_;
    ny_ = item.ny_;

    width_ = item.width_;
    height_ = item.height_;

    data_ = item.data_;

    palette_ = NULL;
    if(item.palette_)
        setPaletteCopy(*item.palette_);
    format_ = item.format_;
}

ArchivItem_BitmapBase::~ArchivItem_BitmapBase()
{
    delete palette_;
    palette_ = NULL;
}

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

    uint32_t position = (y * width_ + x) * getBBP();
    if(getFormat() == FORMAT_PALETTED)
        data_[position] = colorIdx;
    else
    {
        assert(palette_);
        // RGB+A setzen
        if(colorIdx == TRANSPARENT_INDEX) // Transparenz
            data_[position + 3] = 0x00;
        else
            ColorARGB(palette_->get(colorIdx)).toBGRA(&data_[position]);
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
void ArchivItem_BitmapBase::setPixel(uint16_t x, uint16_t y, const ColorARGB clr)
{
    assert(x < width_ && y < height_);

    uint32_t position = (y * width_ + x) * getBBP();
    if(getFormat() == FORMAT_PALETTED)
    {
        // Palettenindex setzen
        if(clr.getAlpha() == 0)
            data_[position] = TRANSPARENT_INDEX;
        else
            data_[position] = palette_->lookup(clr);
    } else
        clr.toBGRA(&data_[position]);
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
    return getPixelClrIdx(x, y, palette_);
}

uint8_t ArchivItem_BitmapBase::getPixelClrIdx(uint16_t x, uint16_t y, const ArchivItem_Palette* palette) const
{
    assert(x < width_ && y < height_);

    if(getFormat() == FORMAT_PALETTED)
        return getPalettedPixel(x, y);
    else
    {
        assert(palette);
        ColorARGB clr = getARGBPixel(x, y);
        // Index von RGB+A liefern
        if(clr.getAlpha() == 0) // Transparenz
            return TRANSPARENT_INDEX;
        else
            return palette->lookup(clr);
    }
}

libsiedler2::ColorARGB ArchivItem_BitmapBase::getPixel(uint16_t x, uint16_t y) const
{
    assert(x < width_ && y < height_);

    if(getFormat() == FORMAT_PALETTED)
        return palette_->get(getPalettedPixel(x, y));
    else
        return getARGBPixel(x, y);
}

uint8_t* ArchivItem_BitmapBase::getPixelPtr(uint16_t x, uint16_t y)
{
    return &data_[(y * width_ + x) * getBBP()];
}

const boost::uint8_t* ArchivItem_BitmapBase::getPixelPtr(uint16_t x, uint16_t y) const
{
    return &data_[(y * width_ + x) * getBBP()];
}

uint8_t ArchivItem_BitmapBase::getPalettedPixel(uint16_t x, uint16_t y) const
{
    assert(format_ == FORMAT_PALETTED);
    return data_[y * width_ + x];
}

ColorARGB ArchivItem_BitmapBase::getARGBPixel(uint16_t x, uint16_t y) const
{
    assert(format_ == FORMAT_BGRA);
    return ColorARGB::fromBGRA(&data_[(y * width_ + x) * 4u]);
}

void ArchivItem_BitmapBase::init(int16_t width, int16_t height, TextureFormat format)
{
    clear();
    // Consistency: width == 0 <=> height == 0
    if(width == 0)
        height = 0;
    else if(height == 0)
        width = 0;

    if(format == FORMAT_PALETTED && !palette_)
        throw std::runtime_error("Palette is missing");

    width_ = width;
    height_ = height;
    format_ = format;

    uint8_t clear = (format == FORMAT_PALETTED) ? TRANSPARENT_INDEX : 0;

    data_.resize(width_ * height_ * getBBP(), clear);
}

void ArchivItem_BitmapBase::init(int16_t width, int16_t height, TextureFormat format, const ArchivItem_Palette* newPal)
{
    if(format == FORMAT_PALETTED && !newPal)
        throw std::runtime_error("Palette is missing");
    // Set new format to BGRA to allow removing of palette
    if(format == FORMAT_BGRA)
        format_ = FORMAT_BGRA;
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
    data_.clear();
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
    if(newFormat == format_)
        return ErrorCode::NONE;

    if(!palette_)
        return ErrorCode::PALETTE_MISSING;
    if(newFormat == FORMAT_BGRA)
    {
        PixelBufferARGB newBuffer(width_, height_);
        for(unsigned y = 0; y < height_; y++)
        {
            for(unsigned x = 0; x < width_; x++)
            {
                uint8_t clrIdx = getPalettedPixel(x, y);
                newBuffer.set(x, y, clrIdx == TRANSPARENT_INDEX ? ColorARGB(0, 0, 0, 0) : ColorARGB(palette_->get(clrIdx)));
            }
        }
        data_.assign(newBuffer.getPixelPtr(), newBuffer.getPixelPtr() + newBuffer.getSize());
    } else
    {
        PixelBufferPaletted newBuffer(width_, height_);
        for(unsigned y = 0; y < height_; y++)
        {
            for(unsigned x = 0; x < width_; x++)
            {
                ColorARGB clr = getARGBPixel(x, y);
                newBuffer.set(x, y, clr.getAlpha() == 0 ? TRANSPARENT_INDEX : palette_->lookup(clr));
            }
        }
        data_.assign(newBuffer.getPixelPtr(), newBuffer.getPixelPtr() + newBuffer.getSize());
    }
    format_ = newFormat;
    return ErrorCode::NONE;
}

void ArchivItem_BitmapBase::getVisibleArea(int& vx, int& vy, unsigned& vw, unsigned& vh)
{
    int x, y, lx, ly;

    vx = vy = 0;
    lx = ly = -1;

    if((width_ == 0) || (height_ == 0))
    {
        return;
    }

    // find empty rows at left
    for(x = 0; x < width_; ++x)
    {
        for(y = 0; y < height_; ++y)
        {
            if((getBBP() == 1) && (data_[width_ * y + x] != TRANSPARENT_INDEX))
            {
                vx = x;
                break;
            } else if((getBBP() == 4) && (data_[((width_ * y + x) * 4) + 3] != 0x00))
            {
                vx = x;
                break;
            }
        }

        if(y != height_)
            break;
    }

    // find empty rows at bottom
    for(x = width_ - 1; x >= 0; --x)
    {
        for(y = 0; y < height_; ++y)
        {
            if((getBBP() == 1) && (data_[width_ * y + x] != TRANSPARENT_INDEX))
            {
                lx = x;
                break;
            } else if((getBBP() == 4) && (data_[((width_ * y + x) * 4) + 3] != 0x00))
            {
                lx = x;
                break;
            }
        }

        if(y != height_)
            break;
    }

    // find empty rows at top
    for(y = 0; y < height_; ++y)
    {
        for(x = 0; x < width_; ++x)
        {
            if((getBBP() == 1) && (data_[width_ * y + x] != TRANSPARENT_INDEX))
            {
                vy = y;
                break;
            } else if((getBBP() == 4) && (data_[((width_ * y + x) * 4) + 3] != 0x00))
            {
                vy = y;
                break;
            }
        }

        if(x != width_)
            break;
    }

    // find empty rows at bottom
    for(y = height_ - 1; y >= 0; --y)
    {
        for(x = 0; x < width_; ++x)
        {
            if((getBBP() == 1) && (data_[width_ * y + x] != TRANSPARENT_INDEX))
            {
                ly = y;
                break;
            } else if((getBBP() == 4) && (data_[((width_ * y + x) * 4) + 3] != 0x00))
            {
                ly = y;
                break;
            }
        }

        if(x != width_)
            break;
    }

    vw = lx + 1 - vx;
    vh = ly + 1 - vy;
}

bool ArchivItem_BitmapBase::checkPalette(const ArchivItem_Palette& palette) const
{
    if(format_ == FORMAT_PALETTED)
        return true;
    for(unsigned y = 0; y < height_; y++)
    {
        for(unsigned x = 0; x < width_; x++)
        {
            ColorARGB clr = getARGBPixel(x, y);
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
void ArchivItem_BitmapBase::setPalette(ArchivItem_Palette* palette)
{
    // Can happen if we create the bitmap with its own palette
    if(palette_ == palette)
        return;
    if(!palette && format_ == FORMAT_PALETTED)
        throw std::runtime_error("Cannot remove palette from paletted image");
    delete palette_;
    palette_ = palette;
}

void ArchivItem_BitmapBase::setPaletteCopy(const ArchivItem_Palette& palette)
{
    setPalette(dynamic_cast<ArchivItem_Palette*>(getAllocator().clone(palette)));
}

void ArchivItem_BitmapBase::removePalette()
{
    setPalette(NULL);
}

} // namespace libsiedler2
