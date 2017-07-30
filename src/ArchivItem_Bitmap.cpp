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
#include "ArchivItem_Bitmap.h"
#include "ArchivItem_Palette.h"
#include "ColorARGB.h"
#include <cstddef>
#include <vector>

namespace libsiedler2{

/**
 *  schreibt das Bitmap in einen Puffer.
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
int baseArchivItem_Bitmap::print(uint8_t* buffer,
        uint16_t buffer_width,
        uint16_t buffer_height,
        TexturFormat buffer_format,
        const ArchivItem_Palette* palette,
        uint16_t to_x,
        uint16_t to_y,
        uint16_t from_x,
        uint16_t from_y,
        uint16_t from_w,
        uint16_t from_h) const
{
    if(buffer == NULL || buffer_width == 0 || buffer_height == 0)
        return 1;
    if(palette == NULL)
        palette = this->palette_;
    if(palette == NULL && (buffer_format == FORMAT_PALETTED || buffer_format == FORMAT_PALETTED))
        return 2;

    if(from_x >= tex_width_ || from_y >= tex_height_ || to_x >= buffer_width || to_y >= buffer_height)
        return 0;

    if(from_w == 0 || from_x + from_w > tex_width_)
        from_w = tex_width_ - from_x;
    if(from_h == 0 || from_y + from_h > tex_height_)
        from_h = tex_height_ - from_y;

    const unsigned bufBpp = getBBP(buffer_format);
    const unsigned texBpp = getBBP(getFormat());

    for(uint16_t y = from_y, y2 = to_y; y2 < buffer_height && y < from_y + from_h; ++y, ++y2)
    {
        for(uint16_t x = from_x, x2 = to_x; x2 < buffer_width && x < from_x + from_w; ++x, ++x2)
        {
            size_t posBuf = (y2 * buffer_width + x2) * bufBpp;
            size_t posTex = (y * tex_width_ + x) * texBpp;
            if(getFormat() == FORMAT_PALETTED)
            {
                if(tex_data_[posTex] == TRANSPARENT_INDEX)  // bei Transparenz wird buffer nicht verändert
                    continue;
                if(buffer_format == FORMAT_PALETTED)
                    buffer[posBuf] = tex_data_[posTex];
                else
                    ColorARGB(palette->get(tex_data_[posTex])).toBGRA(&buffer[posBuf]);
            } else
            {
                if(tex_data_[posTex + 3] == 0)  // bei Transparenz wird buffer nicht verändert
                    continue;
                if(buffer_format == FORMAT_PALETTED)
                    buffer[posBuf] = tex_getPixel(x, y, palette);
                else
                    *reinterpret_cast<ColorARGB*>(&buffer[posBuf]) = *reinterpret_cast<const ColorARGB*>(&tex_data_[posTex]);
            }
        }
    }

    // Alles ok
    return 0;
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
int baseArchivItem_Bitmap::create(uint16_t width,
        uint16_t height,
        const uint8_t* buffer,
        uint16_t buffer_width,
        uint16_t buffer_height,
        TexturFormat buffer_format,
        const ArchivItem_Palette* palette)
{
    if(width == 0 || height == 0 || buffer == NULL || buffer_width == 0 || buffer_height == 0)
        return 1;
    if(!palette && buffer_format == FORMAT_PALETTED)
        return 2;

    // Save the used palette
    if(palette)
        setPalette(*palette);
    else
        setPalette(NULL);

    // Texturspeicher anfordern
    tex_alloc(width, height, buffer_format);

    const unsigned bpp = getBBP();
    uint16_t copyWidth = std::min(buffer_width, width);
    uint16_t copyHeight = std::min(buffer_height, height);
    size_t rowSize = copyWidth * bpp;

    for(uint32_t y = 0; y < copyHeight; ++y)
    {
        size_t posFrom = y * buffer_width * bpp;
        size_t posTexFrom = y * tex_width_ * bpp;
        std::copy(&buffer[posFrom], &buffer[posFrom + rowSize], tex_data_.begin() + posTexFrom);
    }

    // Alles ok
    return 0;
}

} // namespace libsiedler2
