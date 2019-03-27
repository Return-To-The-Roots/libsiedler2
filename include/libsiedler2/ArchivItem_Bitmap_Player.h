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
#ifndef ARCHIVITEM_BITMAP_PLAYER_H_INCLUDED
#define ARCHIVITEM_BITMAP_PLAYER_H_INCLUDED

#pragma once

#include "ArchivItem_BitmapBase.h"
#include "GetFormat.h"
#include "PixelBufferPaletted.h"
#include "enumTypes.h"
#include <cstdint>
#include <iosfwd>
#include <vector>

namespace libsiedler2 {
class ArchivItem_Palette;
}

namespace libsiedler2 {
/// Basisklasse für Player-Bitmaps.
class ArchivItem_Bitmap_Player : public virtual ArchivItem_BitmapBase //-V690
{
public:
    static constexpr uint8_t numPlayerClrs = 4;
    /// Color index used for transparent colors in the player color buffer. We need a different one here as we either store transparent or
    /// an offset onto the actual player color index
    static constexpr uint8_t TRANSPARENT_PLAYER_CLR_IDX = 0xFF;

    ArchivItem_Bitmap_Player();

    ArchivItem_Bitmap_Player(const ArchivItem_Bitmap_Player& other) = default;

    ~ArchivItem_Bitmap_Player() override;

    RTTR_CLONEABLE(ArchivItem_Bitmap_Player)

    /// lädt die Bilddaten aus einer Datei.
    int load(std::istream& file, const ArchivItem_Palette* palette) override;

    /// lädt die Bilddaten aus einem Puffer.
    int load(uint16_t width, uint16_t height, const std::vector<uint8_t>& image, const std::vector<uint16_t>& starts, bool absoluteStarts,
             const ArchivItem_Palette* palette);

    /// schreibt die Bilddaten in eine Datei.
    int write(std::ostream& file, const ArchivItem_Palette* palette) const override;

    /// Creates a new texture and initializes it to transparent
    void init(int16_t width, int16_t height, TextureFormat format) override;
    using ArchivItem_BitmapBase::init;

    /// räumt den Bildspeicher auf.
    void clear() override;

    void getVisibleArea(int& vx, int& vy, unsigned& vw, unsigned& vh) override;

    /// schreibt das Bitmap inkl. festgelegter Spielerfarbe in einen Puffer.
    int print(uint8_t* buffer, uint16_t buffer_width, uint16_t buffer_height, TextureFormat buffer_format,
              const ArchivItem_Palette* palette = nullptr, uint8_t plClrStartIdx = 128, uint16_t to_x = 0, uint16_t to_y = 0,
              uint16_t from_x = 0, uint16_t from_y = 0, uint16_t from_w = 0, uint16_t from_h = 0, bool only_player = false) const;
    template<class T_PixelBuffer>
    int print(T_PixelBuffer& pixelBuffer, const ArchivItem_Palette* palette = nullptr, uint8_t plClrStartIdx = 128, uint16_t to_x = 0,
              uint16_t to_y = 0, uint16_t from_x = 0, uint16_t from_y = 0, uint16_t from_w = 0, uint16_t from_h = 0,
              bool only_player = false) const;

    /// Create a bitmap with player colors.
    /// All colors with palette index in [plClrStartIdx, plClrStartIdx + numPlayerClrs) are considered to be player colors
    int create(uint16_t width, uint16_t height, const uint8_t* buffer, uint16_t buffer_width, uint16_t buffer_height,
               TextureFormat buffer_format, const ArchivItem_Palette* palette, uint8_t plClrStartIdx = 128);
    int create(const uint8_t* buffer, uint16_t buffer_width, uint16_t buffer_height, TextureFormat buffer_format,
               const ArchivItem_Palette* palette, uint8_t plClrStartIdx = 128);
    template<class T_PixelBuffer>
    int create(uint16_t width, uint16_t height, const T_PixelBuffer& pixelBuffer, const ArchivItem_Palette* palette,
               uint8_t plClrStartIdx = 128);
    template<class T_PixelBuffer>
    int create(const T_PixelBuffer& pixelBuffer, const ArchivItem_Palette* palette, uint8_t plClrStartIdx = 128);

protected:
    PixelBufferPaletted tex_pdata; /// Die Spielerfarbedaten.
};

template<class T_PixelBuffer>
inline int ArchivItem_Bitmap_Player::print(T_PixelBuffer& pixelBuffer, const ArchivItem_Palette* palette, uint8_t plClrStartIdx,
                                           uint16_t to_x, uint16_t to_y, uint16_t from_x, uint16_t from_y, uint16_t from_w, uint16_t from_h,
                                           bool only_player) const
{
    return print(pixelBuffer.getPixelPtr(), pixelBuffer.getWidth(), pixelBuffer.getHeight(), traits::GetFormat<T_PixelBuffer>::value,
                 palette, plClrStartIdx, to_x, to_y, from_x, from_y, from_w, from_h, only_player);
}

inline int libsiedler2::ArchivItem_Bitmap_Player::create(const uint8_t* buffer, uint16_t buffer_width, uint16_t buffer_height,
                                                         TextureFormat buffer_format, const ArchivItem_Palette* palette,
                                                         uint8_t plClrStartIdx /*= 128*/)
{
    return create(buffer_width, buffer_height, buffer, buffer_width, buffer_height, buffer_format, palette, plClrStartIdx);
}

template<class T_PixelBuffer>
inline int ArchivItem_Bitmap_Player::create(uint16_t width, uint16_t height, const T_PixelBuffer& pixelBuffer,
                                            const ArchivItem_Palette* palette, uint8_t plClrStartIdx)
{
    return create(width, height, pixelBuffer.getPixelPtr(), pixelBuffer.getWidth(), pixelBuffer.getHeight(),
                  traits::GetFormat<T_PixelBuffer>::value, palette, plClrStartIdx);
}

template<class T_PixelBuffer>
inline int ArchivItem_Bitmap_Player::create(const T_PixelBuffer& pixelBuffer, const ArchivItem_Palette* palette, uint8_t plClrStartIdx)
{
    return create(pixelBuffer.getWidth(), pixelBuffer.getHeight(), pixelBuffer, palette, plClrStartIdx);
}

} // namespace libsiedler2

#endif // !ARCHIVITEM_BITMAP_PLAYER_H_INCLUDED
