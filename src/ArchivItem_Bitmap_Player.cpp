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
#include "ArchivItem_Bitmap_Player.h"
#include "ArchivItem_Palette.h"
#include "ColorARGB.h"
#include "libsiedler2.h"
#include "libendian/src/EndianIStreamAdapter.h"
#include "libendian/src/EndianOStreamAdapter.h"
#include <vector>
#include <cstddef>
#include <iostream>

/** @class libsiedler2::ArchivItem_Bitmap_Player
 *
 *  Basisklasse für Player-Bitmaps.
 */

/** @class libsiedler2::ArchivItem_Bitmap_Player
 *
 *  Klasse für Player-Bitmaps.
 */

/** @var libsiedler2::ArchivItem_Bitmap_Player::tex_pdata
 *
 *  Die Spielerfarbedaten.
 */

/** @var libsiedler2::ArchivItem_Bitmap_Player::tex_plength
 *
 *  Länge der Spielerfarbendaten.
 */

libsiedler2::ArchivItem_Bitmap_Player::ArchivItem_Bitmap_Player()
{
    setBobType(BOBTYPE_BITMAP_PLAYER);
}

/**
 *  Konstruktor von @p ArchivItem_Bitmap_Player mit Laden der Bilddaten aus
 *  einer Datei.
 *
 *  @param[in] file    Dateihandle der Datei
 *  @param[in] palette Grundpalette
 */
libsiedler2::ArchivItem_Bitmap_Player::ArchivItem_Bitmap_Player(std::istream& file, const ArchivItem_Palette* palette) : ArchivItem_BitmapBase()
{
    setBobType(BOBTYPE_BITMAP_PLAYER);

    load(file, palette);
}

libsiedler2::ArchivItem_Bitmap_Player::~ArchivItem_Bitmap_Player()
{}

libsiedler2::ArchivItem_Bitmap_Player& libsiedler2::ArchivItem_Bitmap_Player::operator=(const ArchivItem_Bitmap_Player& other)
 {
     if(&other == this)
         return *this;

     ArchivItem_BitmapBase::operator=(other);
     tex_pdata = other.tex_pdata;
     return *this;
 }

/**
 *  lädt die Bilddaten aus einer Datei.
 *
 *  @param[in] file    Dateihandle der Datei
 *  @param[in] palette Grundpalette
 *
 *  @return liefert Null bei Erfolg, ungleich Null bei Fehler
 */
int libsiedler2::ArchivItem_Bitmap_Player::load(std::istream& file, const ArchivItem_Palette* palette)
{
    if(!file)
        return 1;
    if(palette == NULL)
        palette = getPalette();
    else
        setPalette(*palette);
    if(palette == NULL)
        return 2;

    tex_clear();
    libendian::EndianIStreamAdapter<false, std::istream&> fs(file);
    uint16_t width, height;
    char unknown1[4], unknown2[2];
    uint32_t length;

    fs >> nx_ >> ny_ >> unknown1 >> width >> height >> unknown2 >> length;

    if(!fs)
        return 4;

    std::vector<uint16_t> starts;
    std::vector<uint8_t> data;
    // Daten einlesen
    if(length >= height * sizeof(uint16_t))
    {
        starts.resize(height);
        data.resize(length - height * sizeof(uint16_t));
        if(!(fs >> starts >> data))
            return 6;
    } else
        return 5;

    if(load(width, height, data, starts, false, palette) != 0)
        return 9;

    return (!fs) ? 99 : 0;
}

/**
 *  lädt die Bilddaten aus einem Puffer.
 *
 *  @param[in] width    Breite des Puffers
 *  @param[in] height   Höhe des Puffers
 *  @param[in] image    Bilddaten (komprimierte Form)
 *  @param[in] starts   Startadressen der Bildzeilen
 *  @param[in] absolute Die Startadressen sind absolut/relativ
 *  @param[in] length   Länge des Puffers
 *  @param[in] palette  Grundpalette
 *
 *  @return liefert Null bei Erfolg, ungleich Null bei Fehler
 */
int libsiedler2::ArchivItem_Bitmap_Player::load(uint16_t width, uint16_t height, const std::vector<uint8_t>& image, const std::vector<uint16_t>& starts, bool absoluteStarts, const ArchivItem_Palette* palette)
{
    // Speicher anlegen
    tex_alloc(width, height, getTextureFormat());

    if(image.empty())
        return 0;

    // Einlesen
    for(uint16_t y = 0; y < height; ++y)
    {
        uint16_t x = 0;

        uint32_t position = starts[y];
        if(!absoluteStarts)
            position -= height * 2;

        if(position > image.size())
            return 1;

        // Solange Zeile einlesen, bis x voll ist
        while(x < width)
        {
            // Schalter einlesen
            uint8_t shift = image[position++];

            if(shift < 0x40)
            {
                // transparente Pixel setzen
                for(uint8_t i = 0; i < shift; ++i, ++x)
                    setPixel(x, y, TRANSPARENT_INDEX, palette);
            } else if(shift < 0x80)
            {
                shift -= 0x40;

                // farbige Pixel setzen
                for(uint8_t i = 0; i < shift; ++i, ++x)
                    setPixel(x, y, image[position++], palette);
            } else if(shift < 0xC0)
            {
                shift -= 0x80;

                // Spielerfarbe Pixel setzen
                for(uint8_t i = 0; i < shift; ++i, ++x)
                {
                    tex_pdata[y * getWidth() + x] = image[position];
                    setPixel(x, y, TRANSPARENT_INDEX, palette);
                }
                ++position;
            } else
            {
                shift -= 0xC0;

                // komprimierte Pixel setzen
                for(uint8_t i = 0; i < shift; ++i, ++x)
                    setPixel(x, y, image[position], palette);
                ++position;
            }
        }
    }

    return 0;
}

/**
 *  schreibt die Bilddaten in eine Datei.
 *
 *  @param[in] file    Dateihandle der Datei
 *  @param[in] palette Grundpalette
 *
 *  @return liefert Null bei Erfolg, ungleich Null bei Fehler
 *
 *  @todo unkomprimierte Pixel werden nicht geschrieben -> braucht viel Speicherplatz
 */
int libsiedler2::ArchivItem_Bitmap_Player::write(std::ostream& file, const ArchivItem_Palette* palette) const
{
    if(!file)
        return 1;
    if(palette == NULL)
        palette = getPalette();
    if(palette == NULL)
        return 2;

    libendian::EndianOStreamAdapter<false, std::ostream&> fs(file);
    char unknown[4] = { 0x00, 0x00, 0x00, 0x00 };
    char unknown2[2] = { 0x01, 0x00 };
    const uint16_t width = getWidth(), height = getHeight();

    fs << nx_ << ny_ << unknown << width << height << unknown2;

    // maximale größe von Player-Image: width*height*2 (sollte reichen :P)
    std::vector<uint8_t> image(width * height * 2);

    // Startadressen
    std::vector<uint16_t> starts(height);
    const unsigned pxlWidth = getWidth();

    uint16_t position = 0;
    for(uint16_t y = 0; y < height; ++y)
    {
        uint16_t x = 0;

        // Startadresse setzen
        starts[y] = position + height * 2;

        // Solange Zeile nicht voll
        while(x < width)
        {
            uint16_t target = position++;

            if(tex_pdata[y * pxlWidth + x] != TRANSPARENT_INDEX)
            {
                // spielerfarbe Pixel
                const uint8_t color = tex_pdata[y * pxlWidth + x];
                image[position++] = color;
                uint8_t count = 1;
                for(++x; x < width && count < 63; ++x, ++count)
                {
                    if(tex_pdata[y * pxlWidth + x] != color)
                        break;
                }

                image[target] = count + 0x80;
            } else
            {
                uint8_t color = getPixelClrIdx(x, y, palette);
                if(color == TRANSPARENT_INDEX)
                {
                    uint8_t count = 1;
                    for(++x; x < width && count < 63; ++x, ++count)
                    {
                        if(getPixelClrIdx(x, y, palette) != TRANSPARENT_INDEX || tex_pdata[y * pxlWidth + x] != TRANSPARENT_INDEX)
                            break;
                    }
                    image[target] = count;
                } else
                {
                    // komprimierte Pixel
                    image[position++] = color;
                    uint8_t count = 1;
                    for(++x; x < width && count < 63; ++x, ++count)
                    {
                        if(getPixelClrIdx(x, y, palette) != color || tex_pdata[y * pxlWidth + x] != TRANSPARENT_INDEX)
                            break;
                    }
                    image[target] = count + 0xC0;
                }
            }
        }
    }

    image.resize(position);
    uint32_t length = position + height * sizeof(uint16_t);

    // Länge schreiben
    fs << length;

    // Daten schreiben
    fs << starts << image;

    return (!fs) ? 99 : 0;
}

/**
 *  alloziert Bildspeicher für die gewünschte Größe.
 */
void libsiedler2::ArchivItem_Bitmap_Player::tex_alloc(int16_t width, int16_t height, TexturFormat format)
{
    ArchivItem_BitmapBase::tex_alloc(width, height, format);

    tex_pdata.resize(getWidth() * getHeight(), TRANSPARENT_INDEX);
}

/**
 *  räumt den Bildspeicher auf.
 */
void libsiedler2::ArchivItem_Bitmap_Player::tex_clear()
{
    ArchivItem_BitmapBase::tex_clear();
    tex_pdata.clear();
}

/**
 *  erzeugt ein Bitmap inkl. festgelegter Spielerfarbe aus einem Puffer.
 *
 *  Wichtig: @p color muss der hellste Ton von vier aufeinanderfolgenden Farbindezes sein,
 *  welche von @p color bis @p color+3 dunkler werden.
 *
 *  z.B. pal05.bbm: 128-131 (blau), 132-135 (gelb), 136-139 (rot), usw
 *  jeweils der erste Wert (128, 132, 136, usw) ist dann der Grundfarbindex.
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
 *  @param[in]     color         Grundfarbindex der benutzt werden soll
 *
 */
bool libsiedler2::ArchivItem_Bitmap_Player::create(uint16_t width,
        uint16_t height,
        const uint8_t* buffer,
        uint16_t buffer_width,
        uint16_t buffer_height,
        TexturFormat buffer_format,
        const ArchivItem_Palette* palette,
        uint8_t color)
{
    if(buffer_width > 0 && buffer_height > 0 && !buffer)
        return false;
    if(!palette)
        palette = getPalette();
    if(!palette)
        return false;

    // Texturspeicher anfordern
    tex_alloc(width, height, buffer_format);
    if(palette)
        setPalette(*palette);

    const unsigned bpp = getBBP(buffer_format);
    const unsigned texBpp = getBBP();
    const unsigned texWidth = getWidth();

    for(uint32_t y = 0, y2 = 0; y2 < buffer_height && y < height; ++y, ++y2)
    {
        for(uint32_t x = 0, x2 = 0; x2 < buffer_width && x < width; ++x, ++x2)
        {
            size_t posBuffer  = (y2 * buffer_width + x2) * bpp;
            size_t posPlayerTex = y * texWidth + x;
            size_t posTex = (y * texWidth + x) * texBpp;
            // und Pixel setzen
            switch(buffer_format)
            {
                case FORMAT_BGRA:
                {
                    ColorARGB clr = ColorARGB::fromBGRA(&buffer[posBuffer]);
                    if(clr.getAlpha() != 0)
                    {
                        uint8_t c = palette->lookup(clr);
                        if(c >= color && c <= color + 3) // Spielerfarbe
                            tex_pdata[posPlayerTex] = c - color;
                    }
                    clr.toBGRA(&getData()[posTex]);
                } break;
                case FORMAT_PALETTED:
                {
                    uint8_t c = buffer[posBuffer];
                    if(c >= color && c <= color + 3) // Spielerfarbe
                    {
                        tex_pdata[posPlayerTex] = c - color;
                        c = TRANSPARENT_INDEX;
                    }
                    setPixel(x, y, c, palette);
                } break;
                default:
                    break;
            }
        }
    }

    // Alles ok
    return true;
}

/**
 *  schreibt das Bitmap inkl. festgelegter Spielerfarbe in einen Puffer.
 *
 *  Wichtig: @p color muss der hellste Ton von vier aufeinanderfolgenden Farbindezes sein,
 *  welche von @p color bis @p color+3 dunkler werden.
 *
 *  z.B. pal05.bbm: 128-131 (blau), 132-135 (gelb), 136-139 (rot), usw
 *  jeweils der erste Wert (128, 132, 136, usw) ist dann der Grundfarbindex.
 *
 *  @param[in,out] buffer        Zielpuffer
 *  @param[in]     buffer_width  Breite des Puffers
 *  @param[in]     buffer_height Höhe des Puffers
 *  @param[in]     buffer_format Texturformat des Puffers
 *  @param[in]     palette       Grundpalette
 *  @param[in]     color         Grundfarbindex der benutzt werden soll
 *  @param[in]     to_x          Ziel-X-Koordinate
 *  @param[in]     to_y          Ziel-Y-Koordinate
 *  @param[in]     from_x        Quell-X-Koordinate
 *  @param[in]     from_y        Quell-Y-Koordinate
 *  @param[in]     from_w        zu kopierende Breite
 *  @param[in]     from_h        zu kopierende Höhe
 *  @param[in]     only_player   bei @p true wird nur die Playerschicht kopiert
 *
 *  @return Null falls Bitmap in Puffer geschrieben worden ist, ungleich Null bei Fehler
 */
bool libsiedler2::ArchivItem_Bitmap_Player::print(uint8_t* buffer,
        uint16_t buffer_width,
        uint16_t buffer_height,
        TexturFormat buffer_format,
        const ArchivItem_Palette* palette,
        uint8_t color,
        uint16_t to_x,
        uint16_t to_y,
        uint16_t from_x,
        uint16_t from_y,
        uint16_t from_w,
        uint16_t from_h,
        bool only_player) const
{
    if(buffer_width == 0 || buffer_height == 0)
        return true;
    if(!buffer)
        return false;
    if(palette == NULL)
        palette = getPalette();
    if(palette == NULL)
        return false;

    if(from_x >= getWidth() || from_y >= getHeight() || to_x >= buffer_width || to_y >= buffer_height)
        return true;

    if(from_w == 0 || from_x + from_w > getWidth())
        from_w = getWidth() - from_x;
    if(from_h == 0 || from_y + from_h > getHeight())
        from_h = getHeight() - from_y;

    const unsigned bpp = getBBP(buffer_format);
    const unsigned texBpp = getBBP();
    const unsigned texWidth = getWidth();
    const std::vector<uint8_t>& pxlData = getData();

    for(uint16_t y = from_y, y2 = to_y; y2 < buffer_height && y < from_y + from_h; ++y, ++y2)
    {
        for(uint16_t x = from_x, x2 = to_x; x2 < buffer_width && x < from_x + from_w; ++x, ++x2)
        {
            size_t posBuffer = (y2 * buffer_width + x2) * bpp;
            size_t posTexture = (y * texWidth + x) * texBpp;
            size_t posPlayerTex = (y * texWidth + x) * 1;
            switch(texBpp)
            {
                case 1: // Textur ist Paletted
                {
                    switch(bpp)
                    {
                        case 1:
                        {
                            // Ziel ist auch Paletted
                            if(tex_pdata[posPlayerTex] != TRANSPARENT_INDEX)
                            {
                                // Playerfarbe setzen
                                buffer[posBuffer] = tex_pdata[posPlayerTex] + color;
                            } else if(!only_player && pxlData[posTexture] != TRANSPARENT_INDEX)
                            {
                                buffer[posBuffer] = pxlData[posTexture];
                            }
                        } break;
                        case 4:
                        {
                            // Ziel ist RGB+A
                            if(tex_pdata[posPlayerTex] != TRANSPARENT_INDEX)
                            {
                                // Playerfarbe setzen
                                ColorARGB(palette->get(tex_pdata[posPlayerTex] + color)).toBGRA(&buffer[posBuffer]);
                            } else if(!only_player && pxlData[posTexture] != TRANSPARENT_INDEX)
                            {
                                // normale Pixel setzen
                                ColorARGB(palette->get(pxlData[posTexture])).toBGRA(&buffer[posBuffer]);
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
                            if(tex_pdata[posPlayerTex] != TRANSPARENT_INDEX)
                            {
                                // Playerfarbe setzen
                                buffer[posBuffer] = tex_pdata[posPlayerTex] + color;
                            } else if(!only_player && pxlData[posTexture + 3] != 0)
                            {
                                // normale Pixel setzen
                                buffer[posBuffer] = getPixelClrIdx(x, y, palette);
                            }
                        } break;
                        case 4:
                        {
                            // Ziel ist auch RGB+A
                            if(tex_pdata[posPlayerTex] != TRANSPARENT_INDEX)
                            {
                                // Playerfarbe setzen
                                ColorARGB(palette->get(tex_pdata[posPlayerTex] + color), pxlData[posTexture + 3]).toBGRA(&buffer[posBuffer]);
                            } else if(!only_player && pxlData[posTexture + 3] != 0)
                                *reinterpret_cast<ColorARGB*>(&buffer[posBuffer]) = *reinterpret_cast<const ColorARGB*>(&pxlData[posTexture]);
                        } break;
                    }
                } break;
            }
        }
    }

    // Alles ok
    return true;
}

void libsiedler2::ArchivItem_Bitmap_Player::getVisibleArea(int& vx, int& vy, int& vw, int& vh)
{
    int x, y, lx, ly;

    vx = vy = 0;
    lx = ly = -1;

    const int width = getWidth();
    const int height = getHeight();

    if((width == 0) || (height == 0))
        return;

    const unsigned texBpp = getBBP();
    const std::vector<uint8_t>& pxlData = getData();

    // find empty rows at left
    for(x = 0; x < width; ++x)
    {
        for(y = 0; y < height; ++y)
        {
            if((tex_pdata[width * y + x] != TRANSPARENT_INDEX) ||
                (texBpp == 1 && pxlData[width * y + x] != TRANSPARENT_INDEX) ||
                (texBpp == 4 && pxlData[((width * y + x) * 4) + 3] != 0x00))
            {
                vx = x;
                break;
            }
        }

        if(y != height)
            break;
    }

    // find empty rows at bottom
    for(x = width - 1; x >= 0; --x)
    {
        for(y = 0; y < height; ++y)
        {
            if((tex_pdata[width * y + x] != TRANSPARENT_INDEX) ||
                (texBpp == 1 && pxlData[width * y + x] != TRANSPARENT_INDEX) ||
                (texBpp == 4 && pxlData[((width * y + x) * 4) + 3] != 0x00))
            {
                lx = x;
                break;
            }
        }

        if(y != height)
            break;
    }

    // find empty rows at top
    for(y = 0; y < height; ++y)
    {
        for(x = 0; x < width; ++x)
        {
            if((tex_pdata[width * y + x] != TRANSPARENT_INDEX) ||
                (texBpp == 1 && pxlData[width * y + x] != TRANSPARENT_INDEX) ||
                (texBpp == 4 && pxlData[((width * y + x) * 4) + 3] != 0x00))
            {
                vy = y;
                break;
            }
        }

        if(x != width)
            break;
    }

    // find empty rows at bottom
    for(y = height - 1; y >= 0; --y)
    {
        for(x = 0; x < width; ++x)
        {
            if((tex_pdata[width * y + x] != TRANSPARENT_INDEX) ||
                (texBpp == 1 && pxlData[width * y + x] != TRANSPARENT_INDEX) ||
                (texBpp == 4 && pxlData[((width * y + x) * 4) + 3] != 0x00))
            {
                ly = y;
                break;
            }
        }

        if(x != width)
            break;
    }

    vw = lx + 1 - vx;
    vh = ly + 1 - vy;
}
