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


#ifndef types_h__
#define types_h__

namespace libsiedler2
{
    /**
     *  @brief Texturformat-Aufzählungstyp.
     *
     *  Enthält alle unterstützten Texturformate als Aufzählungswerte.
     *
     *  @author FloSoft
     */
    enum TEXTURFORMAT
    {
        FORMAT_UNKNOWN = 0,
        FORMAT_PALETTED = 1,
        FORMAT_RGBA = 4
    };

    /// Setzt das verwendete Texturausgabeformat.
    TEXTURFORMAT setTextureFormat(TEXTURFORMAT format);

    /// liefert das verwendete Texturausgabeformat.
    TEXTURFORMAT getTextureFormat(void);

    /**
     *  @brief Bobtype-Aufzählungstyp.
     *
     *  Enthält alle unterstützten Bobtypes als Aufzählungswerte.
     *
     *  @author FloSoft
     */
    enum BOBTYPES
    {
        BOBTYPE_SOUND = 1,
        BOBTYPE_BITMAP_RLE = 2,
        BOBTYPE_FONT = 3,
        BOBTYPE_BITMAP_PLAYER = 4,
        BOBTYPE_PALETTE = 5,
        BOBTYPE_BOB = 6,
        BOBTYPE_BITMAP_SHADOW = 7,
        BOBTYPE_MAP = 8,
        BOBTYPE_TEXT = 9,
        BOBTYPE_RAW = 10,
        BOBTYPE_MAP_HEADER = 11,
        BOBTYPE_INI = 12,
        BOBTYPE_BITMAP_RAW = 14
    };

    /**
     *  @brief Abstract ArchivItem factory
     */
    class IAllocator{
    public:
        virtual ~IAllocator(){}
        virtual ArchivItem* create(unsigned short type, unsigned short subtype) const = 0;
        virtual ArchivItem* clone(const ArchivItem& item) const = 0;
    };

    class StandardAllocator: public IAllocator{
    public:
        virtual ArchivItem* create(unsigned short type, unsigned short subtype) const;
        virtual ArchivItem* clone(const ArchivItem& item) const;
    };

    /// Setzt den Item-Allocator.
    void setAllocator(IAllocator* newAllocator);

    /// Lädt die Datei im Format ihrer Endung.
    int Load(const std::string& file, ArchivInfo& items, const ArchivItem_Palette* palette = NULL);

    /// Schreibt die Datei im Format ihrer Endung.
    int Write(const std::string& file, const ArchivInfo& items, const ArchivItem_Palette* palette = NULL);
}

#endif // types_h__
