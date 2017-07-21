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
#include <fstream>
#include "EndianStream.h"
#include <vector>
#include <cstddef>

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
{
    tex_clear();

    delete palette_;
    palette_ = NULL;
}

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
        palette = this->palette_;
    if(palette == NULL)
        return 2;

    tex_clear();
    libendian::LittleEndianIStreamRef fs(file);
    // Nullpunkt X einlesen
    fs >> nx_;

    // Nullpunkt Y einlesen
    fs >> ny_;

    // Unbekannte Daten überspringen
    fs.ignore(4);

    // Breite einlesen
    fs >> width_;

    // Höhe einlesen
    fs >> height_;

    // Unbekannte Daten überspringen
    fs.ignore(2);

    unsigned length;
    // Länge einlesen
    fs >> length;

    std::vector<unsigned short> starts;
    std::vector<unsigned char> data;
    // Daten einlesen
    if(length >= height_ * sizeof(unsigned short))
    {
        starts.resize(height_);
        data.resize(length - height_ * sizeof(unsigned short));
        fs >> starts >> data;
    }

    if(load(width_, height_, data, starts, false, palette) != 0)
        return 9;

    return 0;
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
int libsiedler2::ArchivItem_Bitmap_Player::load(unsigned short width, unsigned short height, const std::vector<unsigned char>& image, const std::vector<unsigned short>& starts, bool absoluteStarts, const ArchivItem_Palette* palette)
{
    this->width_ = width;
    this->height_ = height;

    // Speicher anlegen
    tex_alloc();

    if(!image.empty())
    {
        // Einlesen
        for(unsigned short y = 0; y < height; ++y)
        {
            unsigned short x = 0;

            unsigned position = starts[y];
            if(!absoluteStarts)
                position -= height * 2;

            if (position > image.size())
                return 1;

            // Solange Zeile einlesen, bis x voll ist
            while(x < width)
            {
                // Schalter einlesen
                unsigned char shift = image[position++];

                if(shift < 0x40)
                {
                    // transparente Pixel setzen
                    for(unsigned char i = 0; i < shift; ++i, ++x)
                        tex_setPixel(x, y, TRANSPARENT_INDEX, palette);
                }
                else if (shift < 0x80)
                {
                    shift -= 0x40;

                    // farbige Pixel setzen
                    for(unsigned char i = 0; i < shift; ++i, ++x)
                        tex_setPixel(x, y, image[position++], palette);
                }
                else if (shift < 0xC0)
                {
                    shift -= 0x80;

                    // Spielerfarbe Pixel setzen
                    for(unsigned char i = 0; i < shift; ++i, ++x)
                    {
                        tex_pdata[y * width + x] = image[position];
                        tex_setPixel(x, y, TRANSPARENT_INDEX, palette);
                    }
                    ++position;
                }
                else
                {
                    shift -= 0xC0;

                    // komprimierte Pixel setzen
                    for(unsigned char i = 0; i < shift; ++i, ++x)
                        tex_setPixel(x, y, image[position], palette);
                    ++position;
                }
            }
        }

        /*if(position != length-(height*2) )
            return 1;*/
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
        palette = this->palette_;
    if(palette == NULL)
        return 2;

    if(width_ == 0 || height_ == 0)
        return 2;

    libendian::LittleEndianOStreamRef fs(file);
    // Nullpunkt X schreiben
    fs << nx_;

    // Nullpunkt Y schreiben
    fs << ny_;

    // Unbekannte Daten schreiben
    char unknown[4] = {0x00, 0x00, 0x00, 0x00};
    fs.write(unknown, sizeof(unknown));

    // Breite schreiben
    fs << width_;

    // Höhe schreiben
    fs << height_;

    // Unbekannte Daten schreiben
    char unknown2[2] = {0x01, 0x00};
    fs.write(unknown2, sizeof(unknown2));

    // maximale größe von Player-Image: width*height*2 (sollte reichen :P)
    std::vector<unsigned char> image(width_ * height_ * 2);

    // Startadressen
    std::vector<unsigned short> starts(height_);

    unsigned short position = 0;
    for(unsigned short y = 0; y < height_; ++y)
    {
        unsigned short x = 0;

        // Startadresse setzen
        starts[y] = position + height_ * 2;

        // Solange Zeile nicht voll
        while(x < width_)
        {
            unsigned char color, count;
            unsigned short target = position++;

            color = tex_getPixel(x, y, palette);
            if(color == TRANSPARENT_INDEX && tex_pdata[y * width_ + x] == TRANSPARENT_INDEX)
            {
                count = 0;

                // transparente Pixel
                while(color == TRANSPARENT_INDEX && tex_pdata[y * width_ + x + count] == TRANSPARENT_INDEX && count < 63 && x + count < width_)
                {
                    ++count;
                    color = tex_getPixel(x + count, y, palette);
                }
                x += count;

                image[target] = count;
            }
            else if( tex_pdata[y * width_ + x] != TRANSPARENT_INDEX )
            {
                unsigned char first = tex_pdata[y * width_ + x];
                count = 0;

                // spielerfarbe Pixel
                image[position++] = tex_pdata[y * width_ + x];
                while( tex_pdata[y * width_ + x + count] == first && count < 63 && x + count < width_)
                {
                    ++count;
                }
                x += count;

                image[target] = count + 0x80;
            }
            else
            {
                unsigned char first = color;
                count = 0;

                // komprimierte Pixel
                image[position++] = color;
                while( color == first && tex_pdata[y * width_ + x + count] == TRANSPARENT_INDEX && count < 63 && x + count < width_)
                {
                    ++count;
                    color = tex_getPixel(x + count, y, palette);
                }
                x += count;

                image[target] = count + 0xC0;
            }

        }
    }

    image.resize(position);
    unsigned length = position + height_ * sizeof(unsigned short);

    // Länge schreiben
    fs << length;

    // Daten schreiben
    fs << starts << image;

    return 0;
}

/**
 *  alloziert Bildspeicher für die gewünschte Größe.
 */
void libsiedler2::ArchivItem_Bitmap_Player::tex_alloc()
{
    tex_clear();

    ArchivItem_BitmapBase::tex_alloc();

    tex_pdata.resize(tex_width_ * tex_height_, TRANSPARENT_INDEX);
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
 *  @return Null falls Bitmap erfolgreich erstellt worden ist, ungleich Null bei Fehler
 */
int libsiedler2::ArchivItem_Bitmap_Player::create(unsigned short width,
        unsigned short height,
        const unsigned char* buffer,
        unsigned short buffer_width,
        unsigned short buffer_height,
        TEXTURFORMAT buffer_format,
        const ArchivItem_Palette* palette,
        unsigned char color)
{
    if(width == 0 || height == 0 || buffer == NULL || buffer_width == 0 || buffer_height == 0 || palette == NULL)
        return 1;

    this->width_ = width;
    this->height_ = height;
    setPalette(palette);
    setFormat(buffer_format);

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

    for(unsigned y = 0, y2 = 0; y2 < buffer_height && y < height; ++y, ++y2)
    {
        for(unsigned x = 0, x2 = 0; x2 < buffer_width && x < width; ++x, ++x2)
        {
            size_t posBuffer  = (y2 * buffer_width + x2) * bpp;
            size_t posPlayerTex = (y * width + x) * 1;
            // und Pixel setzen
            switch(buffer_format)
            {
                case FORMAT_RGBA:
                {
                    unsigned char c = palette->lookup(buffer[posBuffer + 2], buffer[posBuffer + 1], buffer[posBuffer + 0]);
                    if(buffer[posBuffer + 3] != 0x00)
                    {
                        if(c >= color && c <= color + 3) // Spielerfarbe
                        {
                            tex_pdata[posPlayerTex] = c - color;
                            c = 20;
                        }

                        tex_setPixel(x, y, c, palette);
                    }
                    else
                        tex_setPixel(x, y, TRANSPARENT_INDEX, palette);
                } break;
                case FORMAT_PALETTED:
                {
                    unsigned char c = buffer[posBuffer];
                    if(c >= color && c <= color + 3) // Spielerfarbe
                    {
                        tex_pdata[posPlayerTex] = c - color;
                        c = 20;
                    }
                    tex_setPixel(x, y, c, palette);
                } break;
                default:
                    break;
            }
        }
    }

    // Alles ok
    return 0;
}

void libsiedler2::ArchivItem_Bitmap_Player::getVisibleArea(int& vx, int& vy, int& vw, int& vh)
{
    int x, y, lx, ly;

    vx = vy = 0;
    lx = ly = -1;

    if ((tex_width_ == 0) || (tex_height_ == 0))
    {
        return;
    }

//fprintf(stderr, "BPP %u\n", tex_bpp);

    // find empty rows at left
    for (x = 0; x < tex_width_; ++x)
    {
        for (y = 0; y < tex_height_; ++y)
        {
            if ((tex_bpp_ == 1) && ((tex_data_[tex_width_ * y + x] != TRANSPARENT_INDEX) || (tex_pdata[tex_width_ * y + x] != TRANSPARENT_INDEX)))
            {
                vx = x;
                break;
            }
            else if ((tex_bpp_ == 4) && ((tex_data_[((tex_width_ * y + x) << 2) + 3] != 0x00) || (tex_pdata[tex_width_ * y + x] != TRANSPARENT_INDEX)))
            {
                vx = x;
                break;
            }
        }

        if (y != tex_height_)
        {
//fprintf(stderr, "--- %i\n", tex_pdata[tex_width * y +x]);
            break;
        }
    }

    // find empty rows at bottom
    for (x = tex_width_ - 1; x >= 0; --x)
    {
        for (y = 0; y < tex_height_; ++y)
        {
            if ((tex_bpp_ == 1) && ((tex_data_[tex_width_ * y + x] != TRANSPARENT_INDEX) || (tex_pdata[tex_width_ * y + x] != TRANSPARENT_INDEX)))
            {
                lx = x;
                break;
            }
            else if ((tex_bpp_ == 4) && ((tex_data_[((tex_width_ * y + x) << 2) + 3] != 0x00) || (tex_pdata[tex_width_ * y + x] != TRANSPARENT_INDEX)))
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
            if ((tex_bpp_ == 1) && ((tex_data_[tex_width_ * y + x] != TRANSPARENT_INDEX) || (tex_pdata[tex_width_ * y + x] != TRANSPARENT_INDEX)))
            {
                vy = y;
                break;
            }
            else if ((tex_bpp_ == 4) && ((tex_data_[((tex_width_ * y + x) << 2) + 3] != 0x00) || (tex_pdata[tex_width_ * y + x] != TRANSPARENT_INDEX)))
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
            if ((tex_bpp_ == 1) && ((tex_data_[tex_width_ * y + x] != TRANSPARENT_INDEX) || (tex_pdata[tex_width_ * y + x] != TRANSPARENT_INDEX)))
            {
                ly = y;
                break;
            }
            else if ((tex_bpp_ == 4) && ((tex_data_[((tex_width_ * y + x) << 2) + 3] != 0x00) || (tex_pdata[tex_width_ * y + x] != TRANSPARENT_INDEX)))
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

//  fprintf(stderr, "%ix%i -> %ix%i %i,%i-%i,%i\n", tex_width, tex_height, vw, vh, vx, vy, lx, ly);
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
int libsiedler2::ArchivItem_Bitmap_Player::print(unsigned char* buffer,
        unsigned short buffer_width,
        unsigned short buffer_height,
        TEXTURFORMAT buffer_format,
        const ArchivItem_Palette* palette,
        unsigned char color,
        unsigned short to_x,
        unsigned short to_y,
        unsigned short from_x,
        unsigned short from_y,
        unsigned short from_w,
        unsigned short from_h,
        bool only_player) const
{
    if(buffer == NULL || buffer_width == 0 || buffer_height == 0)
        return 1;
    if(palette == NULL)
        palette = this->palette_;
    if(palette == NULL)
        return 2;

    if(from_w == 0 || from_x + from_w > width_)
        from_w = width_ - from_x;
    if(from_h == 0 || from_y + from_h > height_)
        from_h = height_ - from_y;

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
            size_t posBuffer = (y2 * buffer_width + x2) * bpp;
            size_t posTexture = (y * tex_width_ + x) * tex_bpp_;
            size_t posPlayerTex = (y * width_ + x) * 1;
            switch(tex_bpp_)
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
                            }
                            if(tex_data_[posTexture] != TRANSPARENT_INDEX)  // bei Transparenz wird buffer nicht verändert
                            {
                                if(!only_player)
                                {
                                    // normale Pixel setzen
                                    buffer[posBuffer] = tex_data_[posTexture];
                                }
                            }
                        } break;
                        case 4:
                        {
                            // Ziel ist RGB+A
                            if(tex_pdata[posPlayerTex] != TRANSPARENT_INDEX)
                            {
                                // Playerfarbe setzen
                                buffer[posBuffer + 3] = 0xFF;
                                palette->get(tex_pdata[posPlayerTex] + color, buffer[posBuffer + 2], buffer[posBuffer + 1], buffer[posBuffer + 0]);
                            }
                            if(tex_data_[posTexture] != TRANSPARENT_INDEX) // bei Transparenz wird buffer nicht verändert
                            {
                                if(!only_player)
                                {
                                    // normale Pixel setzen
                                    buffer[posBuffer + 3] = 0xFF;
                                    palette->get(tex_data_[posTexture], buffer[posBuffer + 2], buffer[posBuffer + 1], buffer[posBuffer + 0]);
                                }
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
                            }
                            if(tex_data_[posTexture + 3] == 0xFF)  // bei Transparenz wird buffer nicht verändert
                            {
                                if(!only_player)
                                {
                                    // normale Pixel setzen
                                    buffer[posBuffer] = tex_getPixel(x, y, palette);
                                }
                            }
                        } break;
                        case 4:
                        {
                            // Ziel ist auch RGB+A
                            if(tex_pdata[posPlayerTex] != TRANSPARENT_INDEX)
                            {
                                // Playerfarbe setzen
                                palette->get(tex_pdata[posPlayerTex] + color, buffer[posBuffer + 2], buffer[posBuffer + 1], buffer[posBuffer + 0]);
                                buffer[posBuffer + 3] = 0xFF; // a
                            }
                            if(tex_data_[posTexture + 3] == 0xFF)  // bei Transparenz wird buffer nicht verändert
                            {
                                if(!only_player)
                                {
                                    // normale Pixel setzen
                                    buffer[posBuffer + 0] = tex_data_[posTexture + 0]; // b
                                    buffer[posBuffer + 1] = tex_data_[posTexture + 1]; // g
                                    buffer[posBuffer + 2] = tex_data_[posTexture + 2]; // r
                                    buffer[posBuffer + 3] = tex_data_[posTexture + 3]; // a
                                }
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
