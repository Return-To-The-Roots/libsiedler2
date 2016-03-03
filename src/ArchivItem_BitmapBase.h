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

#ifndef ARCHIVITEM_BITMAPBASE_H_INCLUDED
#define ARCHIVITEM_BITMAPBASE_H_INCLUDED

#include "ArchivItem.h"
#include "enumTypes.h"

#include <iosfwd>
#include <vector>

namespace libsiedler2
{

    class ArchivItem_Palette;

    /**
     * Base class for all bitmaps (regular and player bitmaps)
     */
    class ArchivItem_BitmapBase: public ArchivItem
    {
    public:
        ArchivItem_BitmapBase();
        ArchivItem_BitmapBase(const ArchivItem_BitmapBase&);
        ~ArchivItem_BitmapBase() override;

        ArchivItem_BitmapBase& operator=(const ArchivItem_BitmapBase& item);

        /// setzt einen Pixel auf einen bestimmten Wert.
        virtual void tex_setPixel(unsigned short x, unsigned short y, unsigned char color, const ArchivItem_Palette* palette);
        virtual void tex_setPixel(unsigned short x, unsigned short y, unsigned char r, unsigned char g, unsigned char b, unsigned char a);

        /// liefert einen Pixel an einem bestimmten Punkt.
        unsigned char tex_getPixel(unsigned short x, unsigned short y, const ArchivItem_Palette* palette) const;

        /// lädt die Bilddaten aus einer Datei.
        virtual int load(std::istream& file, const ArchivItem_Palette* palette) = 0;

        /// schreibt die Bilddaten in eine Datei.
        virtual int write(std::ostream& file, const ArchivItem_Palette* palette) const = 0;

        /// liefert den Textur-Datenblock.
        const std::vector<unsigned char>& getTexData() const;

        /// liefert den X-Nullpunkt.
        short getNx() const;

        /// liefert den Y-Nullpunkt.
        short getNy() const;

        /// liefert die Breite des Bildes.
        unsigned short getWidth() const;

        /// liefert die Höhe des Bildes.
        unsigned short getHeight() const;

        /// gibt Palette zurück
        const ArchivItem_Palette* getPalette() const { return palette_; }

        /// setzt den X-Nullpunkt.
        void setNx(short nx);

        /// setzt den Y-Nullpunkt.
        void setNy(short ny);

        /// setzt die Breite des Bildes.
        void setWidth(unsigned short width);

        /// setzt die Höhe des Bildes.
        void setHeight(unsigned short height);

        /// alloziert Bildspeicher für die gewünschte Größe.
        virtual void tex_alloc();

        /// räumt den Bildspeicher auf.
        virtual void tex_clear();

        /// setzt die Grundpalette des Bildes.
        void setPalette(const ArchivItem_Palette* palette);

        /// setzt das Format des Bildes.
        void setFormat(TEXTURFORMAT format) { this->format_ = format; }

        virtual void getVisibleArea(int& vx, int& vy, int& vw, int& vh);

        /// liefert die nächste Quadratzahl zu einer Zahl.
        static unsigned short tex_pow2(unsigned short n);

    protected:
        unsigned short width_;       ///< Breite des Bildes.
        unsigned short height_;      ///< Höhe des Bildes.

        short nx_;                   ///< X-Nullpunkt.
        short ny_;                   ///< Y-Nullpunkt.

        unsigned short tex_width_;   ///< Breite der Textur.
        unsigned short tex_height_;  ///< Höhe der Textur.

        unsigned short tex_bpp_;     ///< Bytebreite der Textur pro Pixel.
        std::vector<unsigned char> tex_data_;    ///< Die Texturdaten.

        const ArchivItem_Palette* palette_; ///< Die Palette.
        TEXTURFORMAT format_; ///< Das Texturformat.
    };

} // namespace libsiedler2

#endif // ARCHIVITEM_BITMAPBASE_H_INCLUDED
