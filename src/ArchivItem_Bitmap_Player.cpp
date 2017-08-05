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
#include "ArchivItem_Bitmap_Player.h"
#include "ArchivItem_Palette.h"
#include "ColorARGB.h"
#include "ErrorCodes.h"
#include "libsiedler2.h"
#include "libendian/src/EndianIStreamAdapter.h"
#include "libendian/src/EndianOStreamAdapter.h"
#include <cstddef>
#include <cstring>
#include <iostream>
#include <vector>

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
    bobtype_ = BOBTYPE_BITMAP_PLAYER;
}

libsiedler2::ArchivItem_Bitmap_Player::~ArchivItem_Bitmap_Player()
{
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
        return ErrorCode::FILE_NOT_ACCESSIBLE;
    if(palette == NULL)
        return ErrorCode::PALETTE_MISSING;

    clear();
    libendian::EndianIStreamAdapter<false, std::istream&> fs(file);
    uint16_t width, height;
    uint32_t unknown1;
    uint16_t unknown2;
    uint32_t length;

    fs >> nx_ >> ny_ >> unknown1 >> width >> height >> unknown2 >> length;

    if(!fs)
        return ErrorCode::UNEXPECTED_EOF;

    if(unknown1 != 0 || unknown2 != 1)
        return ErrorCode::WRONG_HEADER;

    std::vector<uint16_t> starts;
    std::vector<uint8_t> data;
    // Daten einlesen
    if(length >= height * sizeof(uint16_t))
    {
        starts.resize(height);
        data.resize(length - height * sizeof(uint16_t));
        if(!(fs >> starts >> data))
            return ErrorCode::UNEXPECTED_EOF;
    } else
        return ErrorCode::WRONG_FORMAT;

    int ec = load(width, height, data, starts, false, palette);
    if(ec)
        return ec;
    // Remove external palette
    if(getFormat() == FORMAT_BGRA)
        removePalette();

    return (!fs) ? ErrorCode::UNEXPECTED_EOF : ErrorCode::NONE;
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
int libsiedler2::ArchivItem_Bitmap_Player::load(uint16_t width, uint16_t height, const std::vector<uint8_t>& image,
                                                const std::vector<uint16_t>& starts, bool absoluteStarts, const ArchivItem_Palette* palette)
{
    // Speicher anlegen
    init(width, height, getGlobalTextureFormat(), palette);

    if(image.empty())
        return ErrorCode::NONE;

    // Einlesen
    for(uint16_t y = 0; y < height; ++y)
    {
        uint16_t x = 0;

        uint32_t position = starts[y];
        if(!absoluteStarts)
            position -= height * 2;

        if(position > image.size())
            return ErrorCode::UNEXPECTED_EOF;

        // Solange Zeile einlesen, bis x voll ist
        while(x < width)
        {
            // Schalter einlesen
            uint8_t shift = image[position++];

            if(shift < 0x40)
            {
                // transparente Pixel setzen
                for(uint8_t i = 0; i < shift; ++i, ++x)
                    setPixel(x, y, TRANSPARENT_INDEX);
            } else if(shift < 0x80)
            {
                shift -= 0x40;

                // farbige Pixel setzen
                for(uint8_t i = 0; i < shift; ++i, ++x)
                    setPixel(x, y, image[position++]);
            } else if(shift < 0xC0)
            {
                shift -= 0x80;

                // Spielerfarbe Pixel setzen
                for(uint8_t i = 0; i < shift; ++i, ++x)
                {
                    tex_pdata.set(x, y, image[position]);
                    setPixel(x, y, image[position] + 128);
                }
                ++position;
            } else
            {
                shift -= 0xC0;

                // komprimierte Pixel setzen
                for(uint8_t i = 0; i < shift; ++i, ++x)
                    setPixel(x, y, image[position]);
                ++position;
            }
        }
    }

    return ErrorCode::NONE;
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
        return ErrorCode::FILE_NOT_ACCESSIBLE;
    if(palette == NULL)
        palette = getPalette();
    // Palette required for ARGB -> Palette conversion only
    if(palette == NULL && getFormat() == FORMAT_BGRA)
        return ErrorCode::PALETTE_MISSING;

    libendian::EndianOStreamAdapter<false, std::ostream&> fs(file);
    const uint16_t width = getWidth(), height = getHeight();

    fs << nx_ << ny_ << uint32_t(0) << width << height << uint16_t(1);

    // maximale größe von Player-Image: width*height*2 (sollte reichen :P)
    std::vector<uint8_t> image(width * height * 2);

    // Startadressen
    std::vector<uint16_t> starts(height);

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

            if(tex_pdata.get(x, y) != TRANSPARENT_INDEX)
            {
                // spielerfarbe Pixel
                const uint8_t color = tex_pdata.get(x, y);
                image[position++] = color;
                uint8_t count = 1;
                for(++x; x < width && count < 63; ++x, ++count)
                {
                    if(tex_pdata.get(x, y) != color)
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
                        if(getPixelClrIdx(x, y, palette) != TRANSPARENT_INDEX || tex_pdata.get(x, y) != TRANSPARENT_INDEX)
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
                        if(getPixelClrIdx(x, y, palette) != color || tex_pdata.get(x, y) != TRANSPARENT_INDEX)
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

    return (!fs) ? ErrorCode::UNEXPECTED_EOF : ErrorCode::NONE;
}

/**
 *  alloziert Bildspeicher für die gewünschte Größe.
 */
void libsiedler2::ArchivItem_Bitmap_Player::init(int16_t width, int16_t height, TextureFormat format)
{
    ArchivItem_BitmapBase::init(width, height, format);

    tex_pdata = PixelBufferPaletted(getWidth(), getHeight());
}

/**
 *  räumt den Bildspeicher auf.
 */
void libsiedler2::ArchivItem_Bitmap_Player::clear()
{
    ArchivItem_BitmapBase::clear();
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
int libsiedler2::ArchivItem_Bitmap_Player::create(uint16_t width, uint16_t height, const uint8_t* buffer, uint16_t buffer_width,
                                                  uint16_t buffer_height, TextureFormat buffer_format, const ArchivItem_Palette* palette,
                                                  uint8_t plClrStartIdx)
{
    if(buffer_width > 0 && buffer_height > 0 && !buffer)
        return ErrorCode::INVALID_BUFFER;
    if(!palette)
        return ErrorCode::PALETTE_MISSING;

    // Texturspeicher anfordern
    init(width, height, buffer_format, buffer_format == FORMAT_BGRA ? NULL : palette);

    const unsigned bpp = getBBP(buffer_format);

    for(uint32_t y = 0, y2 = 0; y2 < buffer_height && y < height; ++y, ++y2)
    {
        for(uint32_t x = 0, x2 = 0; x2 < buffer_width && x < width; ++x, ++x2)
        {
            size_t posBuffer = (y2 * buffer_width + x2) * bpp;
            // und Pixel setzen
            if(buffer_format == FORMAT_BGRA)
            {
                ColorARGB clr = ColorARGB::fromBGRA(&buffer[posBuffer]);
                if(clr.getAlpha() != 0)
                {
                    uint8_t c = palette->lookup(clr);
                    if(c >= plClrStartIdx && c <= plClrStartIdx + numPlayerClrs - 1) // Spielerfarbe
                        tex_pdata.set(x, y, c - plClrStartIdx);
                }
                clr.toBGRA(getPixelPtr(x, y));
            } else
            {
                uint8_t c = buffer[posBuffer];
                if(c >= plClrStartIdx && c <= plClrStartIdx + numPlayerClrs - 1) // Spielerfarbe
                {
                    tex_pdata.set(x, y, c - plClrStartIdx);
                    c = TRANSPARENT_INDEX;
                }
                *getPixelPtr(x, y) = c;
            }
        }
    }

    // Alles ok
    return ErrorCode::NONE;
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
int libsiedler2::ArchivItem_Bitmap_Player::print(uint8_t* buffer, uint16_t buffer_width, uint16_t buffer_height,
                                                 TextureFormat buffer_format, const ArchivItem_Palette* palette, uint8_t plClrStartIdx,
                                                 uint16_t to_x, uint16_t to_y, uint16_t from_x, uint16_t from_y, uint16_t from_w,
                                                 uint16_t from_h, bool only_player) const
{
    if(buffer_width == 0 || buffer_height == 0)
        return ErrorCode::NONE;
    if(!buffer)
        return ErrorCode::INVALID_BUFFER;
    if(palette == NULL)
        palette = getPalette();
    if(palette == NULL)
        return ErrorCode::PALETTE_MISSING;

    if(from_x >= getWidth() || from_y >= getHeight() || to_x >= buffer_width || to_y >= buffer_height)
        return ErrorCode::NONE;

    if(from_w == 0 || from_x + from_w > getWidth())
        from_w = getWidth() - from_x;
    if(from_h == 0 || from_y + from_h > getHeight())
        from_h = getHeight() - from_y;

    const unsigned bpp = getBBP(buffer_format);

    for(uint16_t y = from_y, y2 = to_y; y2 < buffer_height && y < from_y + from_h; ++y, ++y2)
    {
        for(uint16_t x = from_x, x2 = to_x; x2 < buffer_width && x < from_x + from_w; ++x, ++x2)
        {
            size_t posBuffer = (y2 * buffer_width + x2) * bpp;
            const uint8_t* pxlPtr = getPixelPtr(x, y);
            if(getFormat() == FORMAT_PALETTED)
            {
                if(buffer_format == FORMAT_PALETTED)
                {
                    if(tex_pdata.get(x, y) != TRANSPARENT_INDEX)
                    {
                        // Playerfarbe setzen
                        buffer[posBuffer] = tex_pdata.get(x, y) + plClrStartIdx;
                    } else if(!only_player && *pxlPtr != TRANSPARENT_INDEX)
                    {
                        buffer[posBuffer] = *pxlPtr;
                    }
                } else
                {
                    // Ziel ist RGB+A
                    if(tex_pdata.get(x, y) != TRANSPARENT_INDEX)
                    {
                        // Playerfarbe setzen
                        ColorARGB(palette->get(tex_pdata.get(x, y) + plClrStartIdx)).toBGRA(&buffer[posBuffer]);
                    } else if(!only_player && *pxlPtr != TRANSPARENT_INDEX)
                    {
                        // normale Pixel setzen
                        ColorARGB(palette->get(*pxlPtr)).toBGRA(&buffer[posBuffer]);
                    }
                }
            } else
            {
                if(buffer_format == FORMAT_PALETTED)
                {
                    // Ziel ist Paletted
                    if(tex_pdata.get(x, y) != TRANSPARENT_INDEX)
                    {
                        // Playerfarbe setzen
                        buffer[posBuffer] = tex_pdata.get(x, y) + plClrStartIdx;
                    } else if(!only_player && pxlPtr[3] != 0)
                    {
                        // normale Pixel setzen
                        buffer[posBuffer] = getPixelClrIdx(x, y, palette);
                    }
                } else
                {
                    // Ziel ist auch RGB+A
                    if(tex_pdata.get(x, y) != TRANSPARENT_INDEX)
                    {
                        // Playerfarbe setzen
                        ColorARGB(palette->get(tex_pdata.get(x, y) + plClrStartIdx), pxlPtr[3]).toBGRA(&buffer[posBuffer]);
                    } else if(!only_player && pxlPtr[3] != 0)
                        std::memcpy(&buffer[posBuffer], pxlPtr, 4);
                }
            }
        }
    }

    // Alles ok
    return ErrorCode::NONE;
}

void libsiedler2::ArchivItem_Bitmap_Player::getVisibleArea(int& vx, int& vy, unsigned& vw, unsigned& vh)
{
    int x, y, lx, ly;

    vx = vy = 0;
    lx = ly = -1;

    const int width = getWidth();
    const int height = getHeight();

    if((width == 0) || (height == 0))
        return;

    const unsigned texBpp = getBBP();

    // find empty rows at left
    for(x = 0; x < width; ++x)
    {
        for(y = 0; y < height; ++y)
        {
            if((tex_pdata.get(x, y) != TRANSPARENT_INDEX) || (texBpp == 1 && *getPixelPtr(x, y) != TRANSPARENT_INDEX)
               || (texBpp == 4 && getPixelPtr(x, y)[3] != 0x00))
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
            if((tex_pdata.get(x, y) != TRANSPARENT_INDEX) || (texBpp == 1 && *getPixelPtr(x, y) != TRANSPARENT_INDEX)
               || (texBpp == 4 && getPixelPtr(x, y)[3] != 0x00))
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
            if((tex_pdata.get(x, y) != TRANSPARENT_INDEX) || (texBpp == 1 && *getPixelPtr(x, y) != TRANSPARENT_INDEX)
               || (texBpp == 4 && getPixelPtr(x, y)[3] != 0x00))
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
            if((tex_pdata.get(x, y) != TRANSPARENT_INDEX) || (texBpp == 1 && *getPixelPtr(x, y) != TRANSPARENT_INDEX)
               || (texBpp == 4 && getPixelPtr(x, y)[3] != 0x00))
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
