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
#include <stdint.h>

namespace libsiedler2
{

    class ArchivItem_Palette;
    struct ColorARGB;

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
        void setPixel(uint16_t x, uint16_t y, uint8_t colorIdx);
        void setPixel(uint16_t x, uint16_t y, const ColorARGB clr);

        /// Return the color index at the given position using the current palette for ARGB conversion
        uint8_t getPixelClrIdx(uint16_t x, uint16_t y) const;
        /// Return the color index at the given position using the given palette for ARGB conversion. Otherwise not required
        uint8_t getPixelClrIdx(uint16_t x, uint16_t y, const ArchivItem_Palette* palette) const;
        /// Return the color at the given position using the current palette for paletted bmps
        ColorARGB getPixel(uint16_t x, uint16_t y) const;

        /// lädt die Bilddaten aus einer Datei.
        virtual int load(std::istream& file, const ArchivItem_Palette* palette) = 0;

        /// schreibt die Bilddaten in eine Datei.
        virtual int write(std::ostream& file, const ArchivItem_Palette* palette) const = 0;

        /// liefert den Textur-Datenblock.
        const std::vector<uint8_t>& getData() const { return data_; }

        /// liefert den X-Nullpunkt.
        int16_t getNx() const;

        /// liefert den Y-Nullpunkt.
        int16_t getNy() const;

        /// liefert die Breite des Bildes.
        uint16_t getWidth() const { return width_; }

        /// liefert die Höhe des Bildes.
        uint16_t getHeight() const { return height_; }

        /// setzt den X-Nullpunkt.
        void setNx(int16_t nx);

        /// setzt den Y-Nullpunkt.
        void setNy(int16_t ny);

        /// alloziert Bildspeicher für die gewünschte Größe.
        virtual void tex_alloc(int16_t width, int16_t height, TexturFormat format);

        /// räumt den Bildspeicher auf.
        virtual void tex_clear();

        /// Return the currently used palette
        const ArchivItem_Palette* getPalette() const { return palette_; }
        /// Set the palette passing ownership
        void setPalette(ArchivItem_Palette* palette);
        /// Set the palette NOT passing ownership
        void setPalette(const ArchivItem_Palette& palette);
        /// Remove the currently used palette
        void removePalette();

        TexturFormat getFormat() const { return format_; }

        virtual void getVisibleArea(int& vx, int& vy, int& vw, int& vh);

        /// Return the bytes per pixel for a given format
        static uint32_t getBBP(TexturFormat format);
        uint32_t getBBP() const { return getBBP(getFormat()); }
    protected:
        std::vector<uint8_t>& getData() { return data_; }

        int16_t nx_;                   /// X-Nullpunkt.
        int16_t ny_;                   /// Y-Nullpunkt.
    private:
        uint16_t width_;       /// Breite des Bildes.
        uint16_t height_;      /// Höhe des Bildes.

        std::vector<uint8_t> data_;    /// Die Texturdaten.

        const ArchivItem_Palette* palette_; /// Die Palette.
        TexturFormat format_; /// Das Texturformat.
    };

    // Define inline in header to allow optimizations
    inline uint32_t ArchivItem_BitmapBase::getBBP(TexturFormat format)
    {
        return (format == FORMAT_PALETTED) ? 1 : 4;
    }

} // namespace libsiedler2

#endif // ARCHIVITEM_BITMAPBASE_H_INCLUDED
