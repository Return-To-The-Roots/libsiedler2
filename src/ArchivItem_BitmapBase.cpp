// Copyright (c) 2005 - 2015 Settlers Freaks (sf-team at siedler25.org)
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
#include "libsiedler2.h"
#include "IAllocator.h"
#include <stdexcept>

namespace libsiedler2{
/** @class ArchivItem_BitmapBase
 *
 *  Basis-Basisklasse für Bitmapitems.
 */

ArchivItem_BitmapBase::ArchivItem_BitmapBase() : ArchivItem()
{
    width_ = 0;
    height_ = 0;

    nx_ = 0;
    ny_ = 0;

    tex_width_ = 0;
    tex_height_ = 0;

    tex_bpp_ = 0;

    palette_ = NULL;
    format_ = FORMAT_UNKNOWN;
}

ArchivItem_BitmapBase::ArchivItem_BitmapBase(const ArchivItem_BitmapBase& item) : ArchivItem( item )
{
    width_ = item.width_;
    height_ = item.height_;

    nx_ = item.nx_;
    ny_ = item.ny_;

    tex_width_ = item.tex_width_;
    tex_height_ = item.tex_height_;

    tex_bpp_ = item.tex_bpp_;

    tex_data_ = item.tex_data_;

    palette_ = NULL;
    if(item.palette_)
        setPalette(*item.palette_);
    setFormat(item.format_);
}

ArchivItem_BitmapBase::~ArchivItem_BitmapBase()
{
    tex_clear();

    delete palette_;
    palette_ = NULL;
}

ArchivItem_BitmapBase& ArchivItem_BitmapBase::operator=(const ArchivItem_BitmapBase& item)
{
    if(this == &item)
        return *this;

    ArchivItem::operator=(item);

    width_ = item.width_;
    height_ = item.height_;

    nx_ = item.nx_;
    ny_ = item.ny_;

    tex_width_ = item.tex_width_;
    tex_height_ = item.tex_height_;

    tex_bpp_ = item.tex_bpp_;

    tex_data_ = item.tex_data_;

    palette_ = NULL;
    setPalette(*item.palette_);
    setFormat(item.format_);

    return *this;
}

/**
 *  setzt einen Pixel auf einen bestimmten Wert.
 *
 *  @param[in] x       X Koordinate des Pixels
 *  @param[in] y       Y Koordinate des Pixels
 *  @param[in] color   Farbe des Pixels
 *  @param[in] palette Grundpalette
 */
void ArchivItem_BitmapBase::tex_setPixel(uint16_t x,
        uint16_t y,
        uint8_t colorIdx,
        const ArchivItem_Palette* palette)
{
    if(tex_data_.empty())
        return;
    if(palette == NULL)
        palette = this->palette_;
    if(palette == NULL)
        throw std::runtime_error("No palette found");

    if(x < tex_width_ && y < tex_height_)
    {
        uint32_t position = (y * tex_width_ + x) * tex_bpp_;
        switch(tex_bpp_)
        {
            case 1:
            {
                // Palettenindex setzen
                tex_data_[position] = colorIdx;
            } break;
            case 4:
            {
                // RGB+A setzen
                if(colorIdx == TRANSPARENT_INDEX) // Transparenz
                    tex_data_[position + 3] = 0x00;
                else
                {
                    palette->get(colorIdx, tex_data_[position + 2], tex_data_[position + 1], tex_data_[position + 0]);
                    tex_data_[position + 3] = 0xFF;
                }
            } break;
        }
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
void ArchivItem_BitmapBase::tex_setPixel(uint16_t x,
        uint16_t y,
        uint8_t r,
        uint8_t g,
        uint8_t b,
        uint8_t a)
{
    if(tex_bpp_ == 1 && palette_ == NULL)
        return;

    if(x < tex_width_ && y < tex_height_)
    {
        uint32_t position = (y * tex_width_ + x) * tex_bpp_;
        switch(tex_bpp_)
        {
            case 1:
            {
                // Palettenindex setzen
                if(a == 0x00)
                    tex_data_[position] = TRANSPARENT_INDEX;
                else
                    tex_data_[position] = palette_->lookup(Color(r, g, b));
            } break;
            case 4:
            {
                // RGBA setzen
                tex_data_[position + 0] = b;
                tex_data_[position + 1] = g;
                tex_data_[position + 2] = r;
                tex_data_[position + 3] = a;
            } break;
        }
    }

}


/**
 *  liefert einen Pixel an einem bestimmten Punkt.
 *
 *  @param[in] x       X Koordinate des Pixels
 *  @param[in] y       Y Koordinate des Pixels
 *  @param[in] palette Grundpalette
 *
 *  @return liefert die Farbe des Pixels
 *
 *  @bug Keine Fehlererkennung!
 */
uint8_t ArchivItem_BitmapBase::tex_getPixel(uint16_t x,
        uint16_t y,
        const ArchivItem_Palette* palette) const
{
    if(tex_data_.empty())
        return 0;
    if(palette == NULL)
        palette = this->palette_;
    if(palette == NULL)
        return 0;

    if(x < tex_width_ && y < tex_height_)
    {
        uint32_t position = (y * tex_width_ + x) * tex_bpp_;
        switch(tex_bpp_)
        {
            case 1:
            {
                // Palettenindex liefern
                return tex_data_[position];
            } break;
            case 4:
            {
                // Index von RGB+A liefern
                if(tex_data_[position + 3] == 0x00) // Transparenz
                    return TRANSPARENT_INDEX;
                else
                    return palette->lookup(tex_data_[position + 2], tex_data_[position + 1], tex_data_[position + 0]);
            } break;
        }
    }
    return 0;
}

/**
 *  alloziert Bildspeicher für die gewünschte Größe.
 */
void ArchivItem_BitmapBase::tex_alloc()
{
    tex_clear();

    tex_width_ = tex_pow2(width_);
    tex_height_ = tex_pow2(height_);

    if(format_ == FORMAT_UNKNOWN)
        format_ = getTextureFormat();

    uint8_t clear;
    switch(format_)
    {
        case FORMAT_RGBA:
            tex_bpp_ = 4;
            clear = 0x00;
            break;
        case FORMAT_PALETTED:
            tex_bpp_ = 1;
            clear = TRANSPARENT_INDEX;
            break;
        default:
            tex_bpp_ = 0;
            clear = 0x7F;
    }

    tex_data_.resize(tex_width_ * tex_height_ * tex_bpp_, clear);
}

/**
 *  räumt den Bildspeicher auf.
 */
void ArchivItem_BitmapBase::tex_clear()
{
    tex_width_ = 0;
    tex_height_ = 0;

    tex_bpp_ = 0;

    tex_data_.clear();
}

/**
 *  liefert die nächste 2er Potenz von einer Zahl.
 *
 *  @param[in] n Zahl zu der die nächste 2er Potenz gesucht werden soll
 *
 *  @return nächste 2er Potenz
 */
uint16_t ArchivItem_BitmapBase::tex_pow2(uint16_t n)
{
    uint16_t t = 2;
    while(true)
    {
        if(t >= n)
            return t;
        t *= 2;
    }
}

/**
 *  liefert den Textur-Datenblock.
 *
 *  @return Der Textur-Datenblock
 */
const std::vector<uint8_t>& ArchivItem_BitmapBase::getTexData() const
{
    return tex_data_;
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
 *  liefert die Breite des Bildes.
 *
 *  @return Breite des Bildes
 */
uint16_t ArchivItem_BitmapBase::getWidth() const
{
    return width_;
}

/**
 *  liefert die Höhe des Bildes.
 *
 *  @return Höhe des Bildes
 */
uint16_t ArchivItem_BitmapBase::getHeight() const
{
    return height_;
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

/**
 *  setzt die Breite des Bildes.
 *
 *  @param[in] width Breite des Bildes
 */
void ArchivItem_BitmapBase::setWidth(uint16_t width)
{
    this->width_ = width;
}

/**
 *  setzt die Höhe des Bildes.
 *
 *  @param[in] height Höhe des Bildes
 */
void ArchivItem_BitmapBase::setHeight(uint16_t height)
{
    this->height_ = height;
}

void ArchivItem_BitmapBase::getVisibleArea(int& vx, int& vy, int& vw, int& vh)
{
    int x, y, lx, ly;

    vx = vy = 0;
    lx = ly = -1;

    if ((tex_width_ == 0) || (tex_height_ == 0))
    {
        return;
    }

    // find empty rows at left
    for (x = 0; x < tex_width_; ++x)
    {
        for (y = 0; y < tex_height_; ++y)
        {
            if ((tex_bpp_ == 1) && (tex_data_[tex_width_ * y + x] != TRANSPARENT_INDEX))
            {
                vx = x;
                break;
            }
            else if ((tex_bpp_ == 4) && (tex_data_[((tex_width_ * y + x) << 2) + 3] != 0x00))
            {
                vx = x;
                break;
            }
        }

        if (y != tex_height_)
        {
            break;
        }
    }

    // find empty rows at bottom
    for (x = tex_width_ - 1; x >= 0; --x)
    {
        for (y = 0; y < tex_height_; ++y)
        {
            if ((tex_bpp_ == 1) && (tex_data_[tex_width_ * y + x] != TRANSPARENT_INDEX))
            {
                lx = x;
                break;
            }
            else if ((tex_bpp_ == 4) && (tex_data_[((tex_width_ * y + x) << 2) + 3] != 0x00))
            {
                lx = x;
                break;
            }
        }

        if (y != tex_height_)
        {
            break;
        }
    }

    // find empty rows at top
    for (y = 0; y < tex_height_; ++y)
    {
        for (x = 0; x < tex_width_; ++x)
        {
            if ((tex_bpp_ == 1) && (tex_data_[tex_width_ * y + x] != TRANSPARENT_INDEX))
            {
                vy = y;
                break;
            }
            else if ((tex_bpp_ == 4) && (tex_data_[((tex_width_ * y + x) << 2) + 3] != 0x00))
            {
                vy = y;
                break;
            }
        }

        if (x != tex_width_)
        {
            break;
        }
    }

    // find empty rows at bottom
    for (y = tex_height_ - 1; y >= 0; --y)
    {
        for (x = 0; x < tex_width_; ++x)
        {
            if ((tex_bpp_ == 1) && (tex_data_[tex_width_ * y + x] != TRANSPARENT_INDEX))
            {
                ly = y;
                break;
            }
            else if ((tex_bpp_ == 4) && (tex_data_[((tex_width_ * y + x) << 2) + 3] != 0x00))
            {
                ly = y;
                break;
            }
        }

        if (x != tex_width_)
        {
            break;
        }
    }

    vw = lx + 1 - vx;
    vh = ly + 1 - vy;
}

/**
 *  setzt die Grundpalette.
 *
 *  @param[in] palette Palette die zukünftig verwendet werden soll.
 */
void ArchivItem_BitmapBase::setPalette(ArchivItem_Palette* palette)
{
    removePalette();
    palette_ = palette;
}

void ArchivItem_BitmapBase::setPalette(const ArchivItem_Palette& palette)
{
    palette_ = dynamic_cast<ArchivItem_Palette*>(getAllocator().clone(palette));
}

void ArchivItem_BitmapBase::removePalette()
{
    delete palette_;
    palette_ = NULL;
}

} // namespace libsiedler2
