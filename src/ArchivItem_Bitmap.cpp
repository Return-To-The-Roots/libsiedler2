// $Id: ArchivItem_Bitmap.cpp 9359 2014-04-25 15:37:22Z FloSoft $
//
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

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#if defined _WIN32 && defined _DEBUG && defined _MSC_VER
#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////////
/** @class libsiedler2::baseArchivItem_Bitmap
 *
 *  Basis-Basisklasse für Bitmapitems.
 *
 *  @author FloSoft
 */

///////////////////////////////////////////////////////////////////////////////
/** @class libsiedler2::ArchivItem_Bitmap
 *
 *  Basisklasse für Bitmapitems.
 *
 *  @author FloSoft
 */

///////////////////////////////////////////////////////////////////////////////
/** @var libsiedler2::baseArchivItem_Bitmap::width
 *
 *  Breite des Bildes.
 *
 *  @author FloSoft
 */

///////////////////////////////////////////////////////////////////////////////
/** @var libsiedler2::baseArchivItem_Bitmap::height
 *
 *  Höhe des Bildes.
 *
 *  @author FloSoft
 */

///////////////////////////////////////////////////////////////////////////////
/** @var libsiedler2::baseArchivItem_Bitmap::nx
 *
 *  X-Nullpunkt.
 *
 *  @author FloSoft
 */

///////////////////////////////////////////////////////////////////////////////
/** @var libsiedler2::baseArchivItem_Bitmap::ny
 *
 *  Y-Nullpunkt.
 *
 *  @author FloSoft
 */

///////////////////////////////////////////////////////////////////////////////
/** @var libsiedler2::baseArchivItem_Bitmap::length
 *
 *  Länge der Bilddaten.
 *
 *  @author FloSoft
 */

///////////////////////////////////////////////////////////////////////////////
/** @var libsiedler2::baseArchivItem_Bitmap::tex_width
 *
 *  Breite der Textur.
 *
 *  @author FloSoft
 */

///////////////////////////////////////////////////////////////////////////////
/** @var libsiedler2::baseArchivItem_Bitmap::tex_height
 *
 *  Höhe der Textur.
 *
 *  @author FloSoft
 */

///////////////////////////////////////////////////////////////////////////////
/** @var libsiedler2::baseArchivItem_Bitmap::tex_bpp
 *
 *  Bytebreite der Textur pro Pixel.
 *
 *  @author FloSoft
 */

///////////////////////////////////////////////////////////////////////////////
/** @var libsiedler2::baseArchivItem_Bitmap::tex_length
 *
 *  Länge der Texturdaten.
 *
 *  @author FloSoft
 */

///////////////////////////////////////////////////////////////////////////////
/** @var libsiedler2::baseArchivItem_Bitmap::tex_data
 *
 *  Die Texturdaten.
 *
 *  @author FloSoft
 */

///////////////////////////////////////////////////////////////////////////////
/**
 *  Konstruktor von @p baseArchivItem_Bitmap.
 *
 *  @author FloSoft
 */
libsiedler2::baseArchivItem_Bitmap::baseArchivItem_Bitmap(void) : ArchivItem()
{
    width = 0;
    height = 0;

    nx = 0;
    ny = 0;

    length = 0;

    tex_width = 0;
    tex_height = 0;

    tex_bpp = 0;

    tex_length = 0;
    tex_data = NULL;

    palette = NULL;
    format = FORMAT_UNKNOWN;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Kopierkonstruktor von @p baseArchivItem_Bitmap.
 *
 *  @param[in] item Quellitem
 *
 *  @author FloSoft
 */
libsiedler2::baseArchivItem_Bitmap::baseArchivItem_Bitmap(const baseArchivItem_Bitmap* item) : ArchivItem( (ArchivItem*)item )
{
    width = item->width;
    height = item->height;

    nx = item->nx;
    ny = item->ny;

    length = item->length;

    tex_width = item->tex_width;
    tex_height = item->tex_height;

    tex_bpp = item->tex_bpp;

    tex_length = item->tex_length;
    tex_data = NULL;

    if(tex_length != 0)
    {
        tex_data = new unsigned char[tex_length];
        memcpy(tex_data, item->tex_data, tex_length);
    }

    palette = NULL;
    setPalette(item->palette);
    setFormat(item->format);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  virtueller Destruktor von @p baseArchivItem_Bitmap.
 *
 *  @author FloSoft
 */
libsiedler2::baseArchivItem_Bitmap::~baseArchivItem_Bitmap(void)
{
    tex_clear();

    delete palette;
    palette = NULL;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  setzt einen Pixel auf einen bestimmten Wert.
 *
 *  @param[in] x       X Koordinate des Pixels
 *  @param[in] y       Y Koordinate des Pixels
 *  @param[in] color   Farbe des Pixels
 *  @param[in] palette Grundpalette
 *
 *  @author FloSoft
 */
void libsiedler2::baseArchivItem_Bitmap::tex_setPixel(unsigned short x,
        unsigned short y,
        unsigned char color,
        const ArchivItem_Palette* palette)
{
    if(tex_data == NULL)
        return;
    if(palette == NULL)
        palette = this->palette;
    if(palette == NULL)
        return;

    if(x < tex_width && y < tex_height)
    {
        unsigned int position = (y * tex_width + x) * tex_bpp;
        switch(tex_bpp)
        {
            case 1:
            {
                // Palettenindex setzen
                tex_data[position] = color;
            } break;
            case 4:
            {
                // RGB+A setzen
                if(color == TRANSPARENT_INDEX) // Transparenz
                    tex_data[position + 3] = 0x00;
                else
                {
                    palette->get(color, &tex_data[position + 2], &tex_data[position + 1], &tex_data[position + 0]);
                    tex_data[position + 3] = 0xFF;
                }
            } break;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  setzt einen Pixel auf einen bestimmten Wert.
 *
 *  @param[in] x X Koordinate des Pixels
 *  @param[in] y Y Koordinate des Pixels
 *  @param[in] r Roter Wert
 *  @param[in] g Grüner Wert
 *  @param[in] b Blauer Wert
 *  @param[in] a Alpha Wert (bei paletted nur 0xFF/0x00 unterstützt)
 *
 *  @author FloSoft
 */
void libsiedler2::baseArchivItem_Bitmap::tex_setPixel(unsigned short x,
        unsigned short y,
        unsigned char r,
        unsigned char g,
        unsigned char b,
        unsigned char a)
{
    if(tex_bpp == 1 && palette == NULL)
        return;

    if(x < tex_width && y < tex_height)
    {
        unsigned int position = (y * tex_width + x) * tex_bpp;
        switch(tex_bpp)
        {
            case 1:
            {
                // Palettenindex setzen
                if(a == 0x00)
                    tex_data[position] = TRANSPARENT_INDEX;
                else
                    tex_data[position] = palette->lookup(r, g, b);
            } break;
            case 4:
            {
                // RGBA setzen
                tex_data[position + 0] = b;
                tex_data[position + 1] = g;
                tex_data[position + 2] = r;
                tex_data[position + 3] = a;
            } break;
        }
    }

}


///////////////////////////////////////////////////////////////////////////////
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
 *
 *  @author FloSoft
 */
unsigned char libsiedler2::baseArchivItem_Bitmap::tex_getPixel(unsigned short x,
        unsigned short y,
        const ArchivItem_Palette* palette) const
{
    if(tex_data == NULL)
        return 0;
    if(palette == NULL)
        palette = this->palette;
    if(palette == NULL)
        return 0;

    if(x < tex_width && y < tex_height)
    {
        unsigned int position = (y * tex_width + x) * tex_bpp;
        switch(tex_bpp)
        {
            case 1:
            {
                // Palettenindex liefern
                return tex_data[position];
            } break;
            case 4:
            {
                // Index von RGB+A liefern
                if(tex_data[position + 3] == 0x00) // Transparenz
                    return TRANSPARENT_INDEX;
                else
                    return palette->lookup(tex_data[position + 2], tex_data[position + 1], tex_data[position + 0]);
            } break;
        }
    }
    return 0;
}

///////////////////////////////////////////////////////////////////////////////
/** @function int libsiedler2::baseArchivItem_Bitmap::load(FILE *file, const ArchivItem_Palette *palette)
 *
 *  lädt die Bilddaten aus einer Datei.
 *
 *  @param[in] file    Dateihandle der Datei
 *  @param[in] palette Grundpalette
 *
 *  @return liefert Null bei Erfolg, ungleich Null bei Fehler
 *
 *  @author FloSoft
 */

///////////////////////////////////////////////////////////////////////////////
/** @function int libsiedler2::baseArchivItem_Bitmap::write(FILE *file, const ArchivItem_Palette *palette)
 *  schreibt die Bilddaten in eine Datei.
 *
 *  @param[in] file    Dateihandle der Datei
 *  @param[in] palette Grundpalette
 *
 *  @return liefert Null bei Erfolg, ungleich Null bei Fehler
 *
 *  @author FloSoft
 */

///////////////////////////////////////////////////////////////////////////////
/**
 *  alloziert Bildspeicher für die gewünschte Größe.
 *
 *  @author FloSoft
 */
void libsiedler2::baseArchivItem_Bitmap::tex_alloc(void)
{
    tex_clear();

    tex_width = tex_pow2(width);
    tex_height = tex_pow2(height);

    if(format == FORMAT_UNKNOWN)
        format = texturformat;

    switch(format)
    {
        case FORMAT_RGBA:
        {
            tex_bpp = 4;
        } break;
        case FORMAT_PALETTED:
        {
            tex_bpp = 1;
        } break;
    }

    tex_length = tex_width * tex_height * tex_bpp;

    if(tex_length != 0)
    {
        tex_data = new unsigned char[tex_length];

        unsigned char clear = 0x7F;
        switch(format)
        {
            case FORMAT_RGBA:
            {
                clear = 0x00;
            } break;
            case FORMAT_PALETTED:
            {
                clear = TRANSPARENT_INDEX;
            } break;
        }
        memset(tex_data, clear, tex_length);
    }
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  räumt den Bildspeicher auf.
 *
 *  @author FloSoft
 */
void libsiedler2::baseArchivItem_Bitmap::tex_clear(void)
{
    delete[] tex_data;

    tex_width = 0;
    tex_height = 0;

    tex_bpp = 0;

    tex_length = 0;
    tex_data = NULL;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  liefert die nächste 2er Potenz von einer Zahl.
 *
 *  @param[in] n Zahl zu der die nächste 2er Potenz gesucht werden soll
 *
 *  @return nächste 2er Potenz
 *
 *  @author FloSoft
 */
unsigned short libsiedler2::baseArchivItem_Bitmap::tex_pow2(unsigned short n)
{
    unsigned short t = 2;
    while(true)
    {
        if(t >= n)
            return t;
        t *= 2;
    }
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  liefert die Länge des Datenblocks.
 *
 *  @return Länge des Datenblocks
 *
 *  @author FloSoft
 */
unsigned int libsiedler2::baseArchivItem_Bitmap::getLength(void) const
{
    return length;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  liefert den Textur-Datenblock.
 *
 *  @return Der Textur-Datenblock
 *
 *  @author FloSoft
 */
const unsigned char* libsiedler2::baseArchivItem_Bitmap::getTexData(void) const
{
    return tex_data;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  liefert den X-Nullpunkt.
 *
 *  @return X-Nullpunkt
 *
 *  @author FloSoft
 */
short libsiedler2::baseArchivItem_Bitmap::getNx(void) const
{
    return nx;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  liefert den Y-Nullpunkt.
 *
 *  @return Y-Nullpunkt
 *
 *  @author FloSoft
 */
short libsiedler2::baseArchivItem_Bitmap::getNy(void) const
{
    return ny;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  liefert die Breite des Bildes.
 *
 *  @return Breite des Bildes
 *
 *  @author FloSoft
 */
unsigned short libsiedler2::baseArchivItem_Bitmap::getWidth(void) const
{
    return width;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  liefert die Höhe des Bildes.
 *
 *  @return Höhe des Bildes
 *
 *  @author FloSoft
 */
unsigned short libsiedler2::baseArchivItem_Bitmap::getHeight(void) const
{
    return height;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  setzt die Länge des Datenblocks.
 *
 *  @param[in] length Länge des Datenblocks
 *
 *  @author FloSoft
 */
void libsiedler2::baseArchivItem_Bitmap::setLength(unsigned int length)
{
    this->length = length;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  setzt den X-Nullpunkt.
 *
 *  @param[in] nx X-Nullpunkt
 *
 *  @author FloSoft
 */
void libsiedler2::baseArchivItem_Bitmap::setNx(short nx)
{
    this->nx = nx;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  setzt den Y-Nullpunkt.
 *
 *  @param[in] ny Y-Nullpunkt
 *
 *  @author FloSoft
 */
void libsiedler2::baseArchivItem_Bitmap::setNy(short ny)
{
    this->ny = ny;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  setzt die Breite des Bildes.
 *
 *  @param[in] width Breite des Bildes
 *
 *  @author FloSoft
 */
void libsiedler2::baseArchivItem_Bitmap::setWidth(unsigned short width)
{
    this->width = width;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  setzt die Höhe des Bildes.
 *
 *  @param[in] height Höhe des Bildes
 *
 *  @author FloSoft
 */
void libsiedler2::baseArchivItem_Bitmap::setHeight(unsigned short height)
{
    this->height = height;
}

void libsiedler2::baseArchivItem_Bitmap::getVisibleArea(int& vx, int& vy, int& vw, int& vh)
{
    int x, y, lx, ly;

    vx = vy = 0;
    lx = ly = -1;

    if ((tex_width == 0) || (tex_height == 0))
    {
        return;
    }

    // find empty rows at left
    for (x = 0; x < tex_width; ++x)
    {
        for (y = 0; y < tex_height; ++y)
        {
            if ((tex_bpp == 1) && (tex_data[tex_width * y + x] != TRANSPARENT_INDEX))
            {
                vx = x;
                break;
            }
            else if ((tex_bpp == 4) && (tex_data[((tex_width * y + x) << 2) + 3] != 0x00))
            {
                vx = x;
                break;
            }
        }

        if (y != tex_height)
        {
            break;
        }
    }

    // find empty rows at bottom
    for (x = tex_width - 1; x >= 0; --x)
    {
        for (y = 0; y < tex_height; ++y)
        {
            if ((tex_bpp == 1) && (tex_data[tex_width * y + x] != TRANSPARENT_INDEX))
            {
                lx = x;
                break;
            }
            else if ((tex_bpp == 4) && (tex_data[((tex_width * y + x) << 2) + 3] != 0x00))
            {
                lx = x;
                break;
            }
        }

        if (y != tex_height)
        {
            break;
        }
    }

    // find empty rows at top
    for (y = 0; y < tex_height; ++y)
    {
        for (x = 0; x < tex_width; ++x)
        {
            if ((tex_bpp == 1) && (tex_data[tex_width * y + x] != TRANSPARENT_INDEX))
            {
                vy = y;
                break;
            }
            else if ((tex_bpp == 4) && (tex_data[((tex_width * y + x) << 2) + 3] != 0x00))
            {
                vy = y;
                break;
            }
        }

        if (x != tex_width)
        {
            break;
        }
    }

    // find empty rows at bottom
    for (y = tex_height - 1; y >= 0; --y)
    {
        for (x = 0; x < tex_width; ++x)
        {
            if ((tex_bpp == 1) && (tex_data[tex_width * y + x] != TRANSPARENT_INDEX))
            {
                ly = y;
                break;
            }
            else if ((tex_bpp == 4) && (tex_data[((tex_width * y + x) << 2) + 3] != 0x00))
            {
                ly = y;
                break;
            }
        }

        if (x != tex_width)
        {
            break;
        }
    }

    vw = lx + 1 - vx;
    vh = ly + 1 - vy;
}

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
int libsiedler2::baseArchivItem_Bitmap::print(unsigned char* buffer,
        unsigned short buffer_width,
        unsigned short buffer_height,
        int buffer_format,
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
        palette = this->palette;
    if(palette == NULL)
        return 2;

    if(from_w == 0 || from_x + from_w > tex_width)
        from_w = tex_width - from_x;
    if(from_h == 0 || from_y + from_h > tex_height)
        from_h = tex_height - from_y;

    unsigned short bpp = 0;
    switch(buffer_format)
    {
        case FORMAT_RGBA:
        {
            bpp = 4;
        } break;
        case FORMAT_PALETTED:
        {
            bpp = 1;
        } break;
    }

    for(unsigned short y = from_y, y2 = to_y; y2 < buffer_height && y < from_y + from_h; ++y, ++y2)
    {
        for(unsigned short x = from_x, x2 = to_x; x2 < buffer_width && x < from_x + from_w; ++x, ++x2)
        {
            unsigned int position = (y2 * buffer_width + x2) * bpp;
            unsigned int position2 = (y * tex_width + x) * tex_bpp;
            switch(tex_bpp)
            {
                case 1: // Textur ist Paletted
                {
                    switch(bpp)
                    {
                        case 1:
                        {
                            // Ziel ist auch Paletted
                            if(tex_data[position2] != TRANSPARENT_INDEX)  // bei Transparenz wird buffer nicht verändert
                                buffer[position] = tex_data[position2];
                        } break;
                        case 4:
                        {
                            // Ziel ist RGB+A
                            if(tex_data[position2] != TRANSPARENT_INDEX) // bei Transparenz wird buffer nicht verändert
                            {
                                palette->get(tex_data[position2], &buffer[position + 2], &buffer[position + 1], &buffer[position + 0]);
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
                            if(tex_data[position2] != TRANSPARENT_INDEX)  // bei Transparenz wird buffer nicht verändert
                                buffer[position] = tex_getPixel(x, y, palette);
                        } break;
                        case 4:
                        {
                            // Ziel ist auch RGB+A
                            if(tex_data[position2 + 3] == 0xFF)  // bei Transparenz wird buffer nicht verändert
                            {
                                buffer[position + 0] = tex_data[position2 + 0]; // b
                                buffer[position + 1] = tex_data[position2 + 1]; // g
                                buffer[position + 2] = tex_data[position2 + 2]; // r
                                buffer[position + 3] = tex_data[position2 + 3]; // a
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
int libsiedler2::baseArchivItem_Bitmap::create(unsigned short width,
        unsigned short height,
        const unsigned char* buffer,
        unsigned short buffer_width,
        unsigned short buffer_height,
        int buffer_format,
        const ArchivItem_Palette* palette)
{
    if(width == 0 || height == 0 || buffer == NULL || buffer_width == 0 || buffer_height == 0)
        return 1;
    if(palette == NULL)
        palette = this->palette;
    if(palette == NULL)
        return 2;

    this->width = width;
    this->height = height;
    this->length = width * height;
    this->format = buffer_format;

    // Texturspeicher anfordern
    tex_alloc();

    unsigned short bpp = 0;
    switch(buffer_format)
    {
        case FORMAT_RGBA:
        {
            bpp = 4;
        } break;
        case FORMAT_PALETTED:
        {
            bpp = 1;
        } break;
    }

    for(unsigned int y = 0, y2 = 0; y2 < buffer_height && y < height; ++y, ++y2)
    {
        for(unsigned int x = 0, x2 = 0; x2 < buffer_width && x < width; ++x, ++x2)
        {
            unsigned int position = (y2 * buffer_width + x2) * bpp;
            // und Pixel setzen
            switch(buffer_format)
            {
                case FORMAT_RGBA:
                {
                    if(buffer[position + 3] != 0x00)
                        tex_setPixel(x, y, buffer[position + 2], buffer[position + 1], buffer[position], buffer[position + 3]);
                    else
                        tex_setPixel(x, y, TRANSPARENT_INDEX, palette);
                } break;
                case FORMAT_PALETTED:
                {
                    tex_setPixel(x, y, buffer[position], palette);
                } break;
            }
        }
    }

    // Alles ok
    return 0;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  setzt die Grundpalette.
 *
 *  @param[in] palette Palette die zukünftig verwendet werden soll.
 *
 *  @author FloSoft
 */
void libsiedler2::baseArchivItem_Bitmap::setPalette(const ArchivItem_Palette* palette)
{
    // alte Palette freigeben
    delete this->palette;

    if(palette)
        this->palette = dynamic_cast<ArchivItem_Palette*>((*allocator)(BOBTYPE_PALETTE, 0, palette));
    else
        this->palette = NULL;
}
