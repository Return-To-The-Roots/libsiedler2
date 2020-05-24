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

#include "ArchivItem_Bitmap.h"
#include "CopyPixelBuffer.h"
#include "ErrorCodes.h"
#include "PixelBufferRef.h"
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <vector>

namespace libsiedler2 {

/**
 *  Write the bitmap into a buffer.
 *  Conditions: If the bitmap is paletted it has a palette
 *              If the buffer is paletted it has a palette (parameter)
 *  If both are paletted values are copied (no translation)
 *  Transparent source pixels are not copied
 *
 *  @param[in,out] buffer        Zielpuffer
 *  @param[in]     buffer_width  Breite des Puffers
 *  @param[in]     buffer_height Höhe des Puffers
 *  @param[in]     buffer_format Texturformat des Puffers
 *  @param[in]     palette       Grundpalette
 *  @param[in]     to_x          Ziel-X-Koordinate
 *  @param[in]     to_y          Ziel-Y-Koordinate
 *  @param[in]     from_x        Quell-X-Koordinate
 *  @param[in]     from_y        Quell-Y-Koordinate
 *  @param[in]     from_w        zu kopierende Breite
 *  @param[in]     from_h        zu kopierende Höhe
 *
 *  @return Null falls Bitmap in Puffer geschrieben worden ist, ungleich Null bei Fehler
 */
int baseArchivItem_Bitmap::print(uint8_t* buffer, uint16_t buffer_width, uint16_t buffer_height, TextureFormat buffer_format,
                                 const ArchivItem_Palette* dstPalette /*= nullptr*/, uint16_t to_x /*= 0*/, uint16_t to_y /*= 0*/,
                                 uint16_t from_x /*= 0*/, uint16_t from_y /*= 0*/, uint16_t from_w /*= 0*/, uint16_t from_h /*= 0*/) const
{
    if(buffer_width == 0 || buffer_height == 0)
        return ErrorCode::NONE;
    if(!buffer)
        return ErrorCode::INVALID_BUFFER;

    if(from_w == 0 && from_x < getWidth())
        from_w = getWidth() - from_x;
    if(from_h == 0 && from_y < getHeight())
        from_h = getHeight() - from_y;

    const Rect fromRect{from_x, from_y, from_w, from_h};
    const Rect toRect{to_x, to_y, buffer_width, buffer_height};

    auto doCall = [this, fromRect, toRect](auto&& dstBuf) {
        if(this->getFormat() == TextureFormat::Paletted)
        {
            const PixelBufferPalettedRef srcBuf = this->getBufferPaletted();
            CopyPixelBuffer(srcBuf, dstBuf, fromRect, toRect);
        } else
        {
            const PixelBufferBGRARef srcBuf = this->getBufferARGB();
            CopyPixelBuffer(srcBuf, dstBuf, fromRect, toRect);
        }
    };

    if(buffer_format == TextureFormat::Paletted)
    {
        if(!dstPalette)
        {
            dstPalette = getPalette();
            if(!dstPalette)
                return ErrorCode::PALETTE_MISSING;
        }
        PixelBufferPalettedRef dstBuf(buffer, buffer_width, buffer_height, *dstPalette);
        doCall(dstBuf);
    } else
    {
        PixelBufferBGRARef dstBuf(reinterpret_cast<uint32_t*>(buffer), buffer_width, buffer_height);
        doCall(dstBuf);
    }
    // Alles ok
    return ErrorCode::NONE;
}

/**
 *  erzeugt ein Bitmap aus einem Puffer.
 *
 *  "überstehende" Ränder werden mit Transparenz aufgefüllt.
 *
 *  @param[in]     width         Breite des neuen Bildes
 *  @param[in]     height        Höhe des neuen Bildes
 *  @param[in]     buffer        Quellpuffer
 *  @param[in]     buffer_width  Breite des Puffers
 *  @param[in]     buffer_height Höhe des Puffers
 *  @param[in]     buffer_format Texturformat des Puffers
 *  @param[in]     palette       Grundpalette
 *
 *  @return Null falls Bitmap erfolgreich erstellt worden ist, ungleich Null bei Fehler
 */
int baseArchivItem_Bitmap::create(uint16_t width, uint16_t height, const uint8_t* buffer, uint16_t buffer_width, uint16_t buffer_height,
                                  TextureFormat buffer_format, const ArchivItem_Palette* palette /*= nullptr*/)
{
    if(buffer_width > 0 && buffer_height > 0 && !buffer)
        return ErrorCode::INVALID_BUFFER;
    if(!palette && buffer_format == TextureFormat::Paletted)
        palette = getPalette();
    if(!palette && buffer_format == TextureFormat::Paletted)
        return ErrorCode::PALETTE_MISSING;

    // Texturspeicher anfordern
    init(width, height, buffer_format, palette);

    const unsigned bpp = getBBP();
    auto copyWidth = size_t(std::min(buffer_width, width));
    auto copyHeight = size_t(std::min(buffer_height, height));
    size_t rowSize = copyWidth * bpp;

    for(uint32_t y = 0; y < copyHeight; ++y)
    {
        size_t posFrom = y * buffer_width * bpp;
        size_t posTexFrom = y * size_t(getWidth()) * bpp;
        std::copy(&buffer[posFrom], &buffer[posFrom + rowSize], getPixelData().begin() + posTexFrom); //-V522
    }

    // Alles ok
    return ErrorCode::NONE;
}

void baseArchivItem_Bitmap::flipVertical()
{
    if(getFormat() == TextureFormat::BGRA)
        libsiedler2::flipVertical(getBufferARGB());
    else
        libsiedler2::flipVertical(getBufferPaletted());
}

} // namespace libsiedler2
