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

///////////////////////////////////////////////////////////////////////////////
// Header
#include "main.h"
#include "ArchivItem_Bitmap.h"
#include "ArchivItem_Palette.h"

// Include last!
#include "DebugNew.h"

namespace libsiedler2{

///////////////////////////////////////////////////////////////////////////////
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
 *
 *  @author FloSoft
 */
int baseArchivItem_Bitmap::print(unsigned char* buffer,
        unsigned short buffer_width,
        unsigned short buffer_height,
        TEXTURFORMAT buffer_format,
        const ArchivItem_Palette* palette,
        unsigned short to_x,
        unsigned short to_y,
        unsigned short from_x,
        unsigned short from_y,
        unsigned short from_w,
        unsigned short from_h) const
{
    if(buffer == NULL || buffer_width == 0 || buffer_height == 0)
        return 1;
    if(palette == NULL)
        palette = this->palette_;
    if(palette == NULL)
        return 2;

    if(from_w == 0 || from_x + from_w > tex_width_)
        from_w = tex_width_ - from_x;
    if(from_h == 0 || from_y + from_h > tex_height_)
        from_h = tex_height_ - from_y;

    unsigned short bpp;
    switch(buffer_format)
    {
        case FORMAT_RGBA:
            bpp = 4;
            break;
        case FORMAT_PALETTED:
            bpp = 1;
            break;
        default:
            bpp = 0;
            break;
    }

    for(unsigned short y = from_y, y2 = to_y; y2 < buffer_height && y < from_y + from_h; ++y, ++y2)
    {
        for(unsigned short x = from_x, x2 = to_x; x2 < buffer_width && x < from_x + from_w; ++x, ++x2)
        {
            size_t position = (y2 * buffer_width + x2) * bpp;
            size_t position2 = (y * tex_width_ + x) * tex_bpp_;
            switch(tex_bpp_)
            {
                case 1: // Textur ist Paletted
                {
                    switch(bpp)
                    {
                        case 1:
                        {
                            // Ziel ist auch Paletted
                            if(tex_data_[position2] != TRANSPARENT_INDEX)  // bei Transparenz wird buffer nicht verändert
                                buffer[position] = tex_data_[position2];
                        } break;
                        case 4:
                        {
                            // Ziel ist RGB+A
                            if(tex_data_[position2] != TRANSPARENT_INDEX) // bei Transparenz wird buffer nicht verändert
                            {
                                palette->get(tex_data_[position2], buffer[position + 2], buffer[position + 1], buffer[position + 0]);
                                buffer[position + 3] = 0xFF;
                            }
                        } break;
                    }
                } break;
                case 4: // Textur ist RGBA
                {
                    switch(bpp)
                    {
                        case 1:
                        {
                            // Ziel ist Paletted
                            if(tex_data_[position2] != TRANSPARENT_INDEX)  // bei Transparenz wird buffer nicht verändert
                                buffer[position] = tex_getPixel(x, y, palette);
                        } break;
                        case 4:
                        {
                            // Ziel ist auch RGB+A
                            if(tex_data_[position2 + 3] == 0xFF)  // bei Transparenz wird buffer nicht verändert
                            {
                                buffer[position + 0] = tex_data_[position2 + 0]; // b
                                buffer[position + 1] = tex_data_[position2 + 1]; // g
                                buffer[position + 2] = tex_data_[position2 + 2]; // r
                                buffer[position + 3] = tex_data_[position2 + 3]; // a
                            }
                        } break;
                    }
                } break;
            }
        }
    }

    // Alles ok
    return 0;
}

///////////////////////////////////////////////////////////////////////////////
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
 *
 *  @author FloSoft
 */
int baseArchivItem_Bitmap::create(unsigned short width,
        unsigned short height,
        const unsigned char* buffer,
        unsigned short buffer_width,
        unsigned short buffer_height,
        TEXTURFORMAT buffer_format,
        const ArchivItem_Palette* palette)
{
    if(width == 0 || height == 0 || buffer == NULL || buffer_width == 0 || buffer_height == 0)
        return 1;
    if(palette == NULL)
        palette = this->palette_;
    if(palette == NULL)
        return 2;

    this->width_ = width;
    this->height_ = height;
    this->format_ = buffer_format;

    // Texturspeicher anfordern
    tex_alloc();

    unsigned short bpp;
    switch(buffer_format)
    {
        case FORMAT_RGBA:
            bpp = 4;
            break;
        case FORMAT_PALETTED:
            bpp = 1;
            break;
        default:
            bpp = 0;
            break;
    }

    for(unsigned int y = 0, y2 = 0; y2 < buffer_height && y < height; ++y, ++y2)
    {
        for(unsigned int x = 0, x2 = 0; x2 < buffer_width && x < width; ++x, ++x2)
        {
            size_t position = (y2 * buffer_width + x2) * bpp;
            // und Pixel setzen
            switch(buffer_format)
            {
                case FORMAT_RGBA:
                    if(buffer[position + 3] != 0x00)
                        tex_setPixel(x, y, buffer[position + 2], buffer[position + 1], buffer[position], buffer[position + 3]);
                    else
                        tex_setPixel(x, y, TRANSPARENT_INDEX, palette);
                    break;
                case FORMAT_PALETTED:
                    tex_setPixel(x, y, buffer[position], palette);
                    break;
                default:
                    break; // Do nothing
            }
        }
    }

    // Alles ok
    return 0;
}

} // namespace libsiedler2
