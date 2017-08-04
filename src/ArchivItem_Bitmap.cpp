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
#include "ArchivItem_Bitmap.h"
#include "ArchivItem_Palette.h"
#include "ColorARGB.h"
#include "ErrorCodes.h"
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
        TextureFormat buffer_format,
        const ArchivItem_Palette* palette /*= NULL*/,
        uint16_t to_x /*= 0*/,
        uint16_t to_y /*= 0*/,
        uint16_t from_x /*= 0*/,
        uint16_t from_y /*= 0*/,
        uint16_t from_w /*= 0*/,
        uint16_t from_h /*= 0*/) const
{
    if(buffer_width == 0 || buffer_height == 0)
        return ErrorCode::NONE;
    if(!buffer)
        return ErrorCode::INVALID_BUFFER;
    if(!palette)
        palette = getPalette();
    if(!palette && buffer_format == FORMAT_PALETTED)
        return ErrorCode::PALETTE_MISSING;

    if(from_x >= getWidth() || from_y >= getHeight() || to_x >= buffer_width || to_y >= buffer_height)
        return ErrorCode::NONE;

    if(from_w == 0 || from_x + from_w > getWidth())
        from_w = getWidth() - from_x;
    if(from_h == 0 || from_y + from_h > getHeight())
        from_h = getHeight() - from_y;

    const unsigned bufBpp = getBBP(buffer_format);
    const unsigned texBpp = getBBP();
    const std::vector<uint8_t>& pxlData = getData();
    for(uint16_t y = from_y, y2 = to_y; y2 < buffer_height && y < from_y + from_h; ++y, ++y2)
    {
        for(uint16_t x = from_x, x2 = to_x; x2 < buffer_width && x < from_x + from_w; ++x, ++x2)
        {
            size_t posBuf = (y2 * buffer_width + x2) * bufBpp;
            size_t posTex = (y * getWidth() + x) * texBpp;
            if(getFormat() == FORMAT_PALETTED)
            {
                if(pxlData[posTex] == TRANSPARENT_INDEX)  // bei Transparenz wird buffer nicht verändert
                    continue;
                if(buffer_format == FORMAT_PALETTED)
                    buffer[posBuf] = pxlData[posTex];
                else
                    ColorARGB(palette->get(pxlData[posTex])).toBGRA(&buffer[posBuf]);
            } else
            {
                if(pxlData[posTex + 3] == 0)  // bei Transparenz wird buffer nicht verändert
                    continue;
                if(buffer_format == FORMAT_PALETTED)
                    buffer[posBuf] = getPixelClrIdx(x, y, palette);
                else
                    *reinterpret_cast<ColorARGB*>(&buffer[posBuf]) = *reinterpret_cast<const ColorARGB*>(&pxlData[posTex]);
            }
        }
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
int baseArchivItem_Bitmap::create(uint16_t width,
        uint16_t height,
        const uint8_t* buffer,
        uint16_t buffer_width,
        uint16_t buffer_height,
        TextureFormat buffer_format,
        const ArchivItem_Palette* palette /*= NULL*/)
{
    if(buffer_width > 0 && buffer_height > 0 && !buffer)
        return ErrorCode::INVALID_BUFFER;
    if(!palette && buffer_format == FORMAT_PALETTED)
        return ErrorCode::PALETTE_MISSING;

    // Texturspeicher anfordern
    init(width, height, buffer_format, palette);

    const unsigned bpp = getBBP();
    uint16_t copyWidth = std::min(buffer_width, width);
    uint16_t copyHeight = std::min(buffer_height, height);
    size_t rowSize = copyWidth * bpp;

    for(uint32_t y = 0; y < copyHeight; ++y)
    {
        size_t posFrom = y * buffer_width * bpp;
        size_t posTexFrom = y * getWidth() * bpp;
        std::copy(&buffer[posFrom], &buffer[posFrom + rowSize], getData().begin() + posTexFrom);
    }

    // Alles ok
    return ErrorCode::NONE;
}

} // namespace libsiedler2
