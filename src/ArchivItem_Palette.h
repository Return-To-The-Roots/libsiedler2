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
#ifndef ARCHIVITEM_PALETTE_H_INCLUDED
#define ARCHIVITEM_PALETTE_H_INCLUDED

#pragma once

#include "ArchivItem.h"
#include <boost/array.hpp>
#include <cstddef>
#include <iosfwd>
#include <stdint.h>

namespace libsiedler2
{
    /// Index des Transparenzwertes
    const uint8_t TRANSPARENT_INDEX = 254;
    struct Color
    {
        uint8_t r, g, b;
        Color(): r(0), g(0), b(0){}
        Color(uint8_t r, uint8_t g, uint8_t b): r(r), g(g), b(b){}

        bool operator==(const Color& obj) const
        {
            return (r == obj.r) && (g == obj.g) && (b == obj.b);
        }
    };

    /// Klasse für Paletten.
    class ArchivItem_Palette : public ArchivItem
    {
        public:
            ArchivItem_Palette();

            /// Konstruktor von @p ArchivItem_Palette mit Laden der Farbwerte aus einer Datei.
            ArchivItem_Palette(std::istream& file, bool skip = true);

            /// liest die Farbwerte aus einer Datei.
            int load(std::istream& file, bool skip = true);

            /// schreibt die Farbwerte in eine Datei.
            int write(std::ostream& file, bool skip = true) const;

            /// setzt einen Farbwert am entsprechenden Index.
            void set(uint8_t index, Color clr);

            /// liefert einen Farbwert am entsprechenden Index.
            void get(uint8_t index, uint8_t& r, uint8_t& g, uint8_t& b) const
            {
                const Color& clr = (*this)[index];
                r = clr.r;
                g = clr.g;
                b = clr.b;
            }

            Color get(uint8_t index) const
            {
                return (*this)[index];
            }

            /// liefert einen Index zum entsprechenden RGB-Wert.
            uint8_t lookup(const Color& clr) const;
            uint8_t lookup(uint8_t r, uint8_t g, uint8_t b) const
            {
                return lookup(Color(r, g, b));
            }

            /// Index-Operator von @p ArchivItem_Palette.
            const Color& operator[](int index) const;

            /// kopiert die Palette in einen Puffer (als RGBA)
            void copy(uint8_t* buffer, size_t bufSize) const;

        protected:
            boost::array<Color, 256> colors; //-V730_NOINIT
    };
}

#endif // !ARCHIVITEM_PALETTE_H_INCLUDED
