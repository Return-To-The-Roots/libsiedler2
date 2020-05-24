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

#include "ArchivItem_Bitmap_Player.h"
#include "ArchivItem_Palette.h"
#include "ColorBGRA.h"
#include "CopyPixelBuffer.h"
#include "ErrorCodes.h"
#include "libendian/EndianIStreamAdapter.h"
#include "libendian/EndianOStreamAdapter.h"
#include <algorithm>
#include <cstddef>
#include <iostream>
#include <vector>

/** @class ArchivItem_Bitmap_Player
 *
 *  Basisklasse für Player-Bitmaps.
 */

/** @class ArchivItem_Bitmap_Player
 *
 *  Klasse für Player-Bitmaps.
 */

/** @var ArchivItem_Bitmap_Player::tex_pdata
 *
 *  Die Spielerfarbedaten.
 */

/** @var ArchivItem_Bitmap_Player::tex_plength
 *
 *  Länge der Spielerfarbendaten.
 */

namespace libsiedler2 {

ArchivItem_Bitmap_Player::ArchivItem_Bitmap_Player()
{
    bobtype_ = BobType::BitmapPlayer;
}

ArchivItem_Bitmap_Player::~ArchivItem_Bitmap_Player() = default;

/**
 *  lädt die Bilddaten aus einer Datei.
 *
 *  @param[in] file    Dateihandle der Datei
 *  @param[in] palette Grundpalette
 *
 *  @return liefert Null bei Erfolg, ungleich Null bei Fehler
 */
int ArchivItem_Bitmap_Player::load(std::istream& file, const ArchivItem_Palette* palette)
{
    if(!file)
        return ErrorCode::FILE_NOT_ACCESSIBLE;
    if(palette == nullptr)
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
    if(getFormat() == TextureFormat::BGRA)
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
int ArchivItem_Bitmap_Player::load(uint16_t width, uint16_t height, const std::vector<uint8_t>& image, const std::vector<uint16_t>& starts,
                                   bool absoluteStarts, const ArchivItem_Palette* palette)
{
    if(!palette)
        return ErrorCode::PALETTE_MISSING;
    // Speicher anlegen
    init(width, height, getWantedFormat(TextureFormat::Paletted), palette);

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
                // Background is transparent, increase x
                x += shift;
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
int ArchivItem_Bitmap_Player::write(std::ostream& file, const ArchivItem_Palette* palette) const
{
    if(!file)
        return ErrorCode::FILE_NOT_ACCESSIBLE;
    if(palette == nullptr)
        palette = getPalette();
    // Palette required
    if(palette == nullptr)
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

            if(isPlayerColor(x, y))
            {
                // spielerfarbe Pixel
                const uint8_t color = getPlayerColorIdx(x, y);
                image[position++] = color;
                uint8_t count = 1;
                for(++x; x < width && count < 63; ++x, ++count)
                {
                    if(getPlayerColorIdx(x, y) != color)
                        break;
                }

                image[target] = count + 0x80;
            } else
            {
                uint8_t color = getPixelClrIdx(x, y, palette);
                if(palette->isTransparent(color))
                {
                    uint8_t count = 1;
                    for(++x; x < width && count < 63; ++x, ++count)
                    {
                        if(!palette->isTransparent(getPixelClrIdx(x, y, palette)) || isPlayerColor(x, y))
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
                        if(getPixelClrIdx(x, y, palette) != color || isPlayerColor(x, y))
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
void ArchivItem_Bitmap_Player::init(int16_t width, int16_t height, TextureFormat format)
{
    ArchivItem_BitmapBase::init(width, height, format);

    tex_pdata = PixelBufferPaletted(getWidth(), getHeight(), TRANSPARENT_PLAYER_CLR_IDX);
}

/**
 *  räumt den Bildspeicher auf.
 */
void ArchivItem_Bitmap_Player::clear()
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
int ArchivItem_Bitmap_Player::create(uint16_t width, uint16_t height, const uint8_t* buffer, uint16_t buffer_width, uint16_t buffer_height,
                                     TextureFormat buffer_format, const ArchivItem_Palette* palette, uint8_t plClrStartIdx)
{
    if(buffer_width > 0 && buffer_height > 0 && !buffer)
        return ErrorCode::INVALID_BUFFER;
    if(!palette)
        palette = getPalette();
    if(!palette)
        return ErrorCode::PALETTE_MISSING;

    // Texturspeicher anfordern
    init(width, height, buffer_format, buffer_format == TextureFormat::BGRA ? nullptr : palette);

    const unsigned bpp = getBBP(buffer_format);

    for(uint32_t y = 0, y2 = 0; y2 < buffer_height && y < height; ++y, ++y2)
    {
        for(uint32_t x = 0, x2 = 0; x2 < buffer_width && x < width; ++x, ++x2)
        {
            size_t posBuffer = (y2 * size_t(buffer_width) + x2) * bpp;
            // und Pixel setzen
            if(buffer_format == TextureFormat::BGRA)
            {
                ColorBGRA clr = ColorBGRA::fromBGRA(&buffer[posBuffer]); //-V522
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
                    c = palette->getTransparentIdx();
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
int ArchivItem_Bitmap_Player::print(uint8_t* buffer, uint16_t buffer_width, uint16_t buffer_height, TextureFormat buffer_format,
                                    const ArchivItem_Palette* palette, uint8_t plClrStartIdx, uint16_t to_x, uint16_t to_y, uint16_t from_x,
                                    uint16_t from_y, uint16_t from_w, uint16_t from_h, bool only_player) const
{
    if(buffer_width == 0 || buffer_height == 0)
        return ErrorCode::NONE;
    if(!buffer)
        return ErrorCode::INVALID_BUFFER;
    if(!palette)
    {
        palette = getPalette();
        if(!palette)
            return ErrorCode::PALETTE_MISSING;
    }

    if(from_w == 0 && from_x < getWidth())
        from_w = getWidth() - from_x;
    if(from_h == 0 && from_y < getHeight())
        from_h = getHeight() - from_y;

    const Rect fromRect = clipRect(Rect{from_x, from_y, from_w, from_h}, getWidth(), getHeight());
    const Rect toRect = clipRect(Rect{to_x, to_y, buffer_width, buffer_height}, buffer_width, buffer_height);

    if(!only_player)
    {
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
            PixelBufferPalettedRef dstBuf(buffer, buffer_width, buffer_height, *palette);
            doCall(dstBuf);
        } else
        {
            PixelBufferBGRARef dstBuf(reinterpret_cast<uint32_t*>(buffer), buffer_width, buffer_height);
            doCall(dstBuf);
        }
    }

    const uint16_t copyWidth = std::min(fromRect.w, toRect.w);
    const uint16_t copyHeight = std::min(fromRect.h, toRect.h);
    const size_t bufferBBP = getBBP(buffer_format);

    for(uint16_t y = 0; y < copyHeight; ++y)
    {
        size_t posBuffer = ((y + toRect.y) * buffer_width + toRect.x) * bufferBBP;
        for(uint16_t x = 0; x < copyWidth; ++x)
        {
            const uint8_t playerClr = getPlayerColorIdx(x + fromRect.x, y + fromRect.y);
            // Don't change if transparent
            if(playerClr != TRANSPARENT_PLAYER_CLR_IDX)
            {
                if(buffer_format == TextureFormat::Paletted)
                    buffer[posBuffer] = playerClr + plClrStartIdx;
                else
                {
                    const uint8_t srcAlpha =
                      (getFormat() == TextureFormat::Paletted) ? 255 : getPixelPtr(x + fromRect.x, y + fromRect.y)[3];
                    ColorBGRA(palette->get(playerClr + plClrStartIdx), srcAlpha).toBGRA(&buffer[posBuffer]);
                }
            }
            posBuffer += bufferBBP;
        }
    }

    // Alles ok
    return ErrorCode::NONE;
}

void ArchivItem_Bitmap_Player::getVisibleArea(int& vx, int& vy, unsigned& vw, unsigned& vh) const
{
    const auto width = getWidth();
    const auto height = getHeight();

    if(width == 0 || height == 0)
    {
        vx = vy = vw = vh = 0;
        return;
    }

    const auto texBpp = getBBP();
    const ArchivItem_Palette* palette = getPalette();
    if(texBpp == 1 && !palette->hasTransparency())
    {
        vx = vy = 0;
        vw = width;
        vh = height;
        return;
    }

    if(getBBP() == 1)
        doGetVisibleArea(vx, vy, vw, vh, [this, palette](auto x, auto y) {
            return !this->isPlayerColor(x, y) && palette->isTransparent(this->getPalettedPixel(x, y));
        });
    else
        doGetVisibleArea(vx, vy, vw, vh, [this](auto x, auto y) { return !this->isPlayerColor(x, y) && this->getPixelPtr(x, y)[3] == 0u; });
}
} // namespace libsiedler2
