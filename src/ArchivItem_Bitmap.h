// $Id: ArchivItem_Bitmap.h 9359 2014-04-25 15:37:22Z FloSoft $
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
#ifndef ARCHIVITEM_BITMAP_H_INCLUDED
#define ARCHIVITEM_BITMAP_H_INCLUDED

#pragma once

#include "ArchivItem.h"
#include "ArchivItem_Palette.h"
#include "types.h"
#include <vector>

namespace libsiedler2
{
    /// Basis-Basisklasse für Bitmapitems.
    class baseArchivItem_Bitmap : public ArchivItem
    {
        public:
            /// Konstruktor von @p ArchivItem_Bitmap.
            baseArchivItem_Bitmap(void);

            /// Kopierkonstruktor von @p ArchivItem_Bitmap.
            baseArchivItem_Bitmap(const baseArchivItem_Bitmap& item);

            /// virtueller Destruktor von @p ArchivItem_Bitmap.
            virtual ~baseArchivItem_Bitmap(void);

            /// setzt einen Pixel auf einen bestimmten Wert.
            virtual void tex_setPixel(unsigned short x, unsigned short y, unsigned char color, const ArchivItem_Palette* palette);
            virtual void tex_setPixel(unsigned short x, unsigned short y, unsigned char r, unsigned char g, unsigned char b, unsigned char a);

            /// liefert einen Pixel an einem bestimmten Punkt.
            unsigned char tex_getPixel(unsigned short x, unsigned short y, const ArchivItem_Palette* palette) const;

            /// lädt die Bilddaten aus einer Datei.
            virtual int load(FILE* file, const ArchivItem_Palette* palette) = 0;

            /// schreibt die Bilddaten in eine Datei.
            virtual int write(FILE* file, const ArchivItem_Palette* palette) const = 0;

            /// liefert die Länge des Datenblocks.
            unsigned int getLength(void) const;

            /// liefert den Textur-Datenblock.
            const std::vector<unsigned char>& getTexData(void) const;

            /// liefert den X-Nullpunkt.
            short getNx(void) const;

            /// liefert den Y-Nullpunkt.
            short getNy(void) const;

            /// liefert die Breite des Bildes.
            unsigned short getWidth(void) const;

            /// liefert die Höhe des Bildes.
            unsigned short getHeight(void) const;

            /// gibt Palette zurück
            const ArchivItem_Palette* getPalette() const { return palette; }

            /// setzt die Länge des Datenblocks.
            void setLength(unsigned int length);

            /// setzt den X-Nullpunkt.
            void setNx(short nx);

            /// setzt den Y-Nullpunkt.
            void setNy(short ny);

            /// setzt die Breite des Bildes.
            void setWidth(unsigned short width);

            /// setzt die Höhe des Bildes.
            void setHeight(unsigned short height);

            /// alloziert Bildspeicher für die gewünschte Größe.
            void tex_alloc(void);

            /// räumt den Bildspeicher auf.
            void tex_clear(void);

            /// setzt die Grundpalette des Bildes.
            void setPalette(const ArchivItem_Palette* palette);

            /// setzt das Format des Bildes.
            void setFormat(TEXTURFORMAT format) { this->format = format; }

            virtual void getVisibleArea(int& vx, int& vy, int& vw, int& vh);

            /// schreibt das Bitmap in einen Puffer.
            virtual int print(unsigned char* buffer,
                              unsigned short buffer_width,
                              unsigned short buffer_height,
                              TEXTURFORMAT buffer_format,
                              const ArchivItem_Palette* palette,
                              unsigned short to_x = 0,
                              unsigned short to_y = 0,
                              unsigned short from_x = 0,
                              unsigned short from_y = 0,
                              unsigned short from_w = 0,
                              unsigned short from_h = 0) const;

            /// erzeugt ein Bitmap aus einem Puffer.
            virtual int create(unsigned short width,
                               unsigned short height,
                               const unsigned char* buffer,
                               unsigned short buffer_width,
                               unsigned short buffer_height,
                               TEXTURFORMAT buffer_format,
                               const ArchivItem_Palette* palette);

            /// liefert die nächste Quadratzahl zu einer Zahl.
            static unsigned short tex_pow2(unsigned short n);

        protected:
            unsigned short width;       ///< Breite des Bildes.
            unsigned short height;      ///< Höhe des Bildes.

            short nx;                   ///< X-Nullpunkt.
            short ny;                   ///< Y-Nullpunkt.

            unsigned int length;        ///< Länge der Bilddaten.

            unsigned short tex_width;   ///< Breite der Textur.
            unsigned short tex_height;  ///< Höhe der Textur.

            unsigned short tex_bpp;     ///< Bytebreite der Textur pro Pixel.
            std::vector<unsigned char> tex_data;    ///< Die Texturdaten.

            const ArchivItem_Palette* palette; ///< Die Palette.
            TEXTURFORMAT format; ///< Das Texturformat.
    };

    /// Basisklasse für Bitmapitems.
    class ArchivItem_Bitmap : public virtual baseArchivItem_Bitmap
    {
    };
}

#endif // !ARCHIVITEM_BITMAP_H_INCLUDED
