// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "ArchivItem_BitmapBase.h"
#include "GetFormat.h"
#include "enumTypes.h"
#include <cstdint>

namespace libsiedler2 {
class ArchivItem_Palette;
}

namespace libsiedler2 {
/**
 * Base class for regular bitmaps (single bitmaps, no player colored ones)
 */
class baseArchivItem_Bitmap : public virtual ArchivItem_BitmapBase
{
public:
    /// schreibt das Bitmap in einen Puffer.
    int print(uint8_t* buffer, uint16_t buffer_width, uint16_t buffer_height, TextureFormat buffer_format,
              const ArchivItem_Palette* dstPalette = nullptr, uint16_t to_x = 0, uint16_t to_y = 0, uint16_t from_x = 0,
              uint16_t from_y = 0, uint16_t from_w = 0, uint16_t from_h = 0) const;
    template<class T_PixelBuffer>
    int print(T_PixelBuffer& pixelBuffer, const ArchivItem_Palette* palette = nullptr, uint16_t to_x = 0,
              uint16_t to_y = 0, uint16_t from_x = 0, uint16_t from_y = 0, uint16_t from_w = 0,
              uint16_t from_h = 0) const;

    /// erzeugt ein Bitmap aus einem Puffer.
    int create(uint16_t width, uint16_t height, const uint8_t* buffer, uint16_t buffer_width, uint16_t buffer_height,
               TextureFormat buffer_format, const ArchivItem_Palette* palette = nullptr);
    int create(const uint8_t* buffer, uint16_t buffer_width, uint16_t buffer_height, TextureFormat buffer_format,
               const ArchivItem_Palette* palette = nullptr);
    /// Create a bitmap with the same data as the pixelBuffer
    template<class T_PixelBuffer>
    int create(const T_PixelBuffer& pixelBuffer, const ArchivItem_Palette* palette = nullptr);
    /// Create a bitmap with the given size and fill it with the data from the pixelBuffer
    template<class T_PixelBuffer>
    int create(uint16_t width, uint16_t height, const T_PixelBuffer& pixelBuffer,
               const ArchivItem_Palette* palette = nullptr);

    void flipVertical();
};

/// Basisklasse für Bitmapitems.
class ArchivItem_Bitmap : public virtual baseArchivItem_Bitmap
{};

template<class T_PixelBuffer>
inline int baseArchivItem_Bitmap::print(T_PixelBuffer& pixelBuffer, const ArchivItem_Palette* dstPalette, uint16_t to_x,
                                        uint16_t to_y, uint16_t from_x, uint16_t from_y, uint16_t from_w,
                                        uint16_t from_h) const
{
    return print(pixelBuffer.getPixelPtr(), pixelBuffer.getWidth(), pixelBuffer.getHeight(),
                 traits::GetFormat<T_PixelBuffer>::value, dstPalette, to_x, to_y, from_x, from_y, from_w, from_h);
}

inline int baseArchivItem_Bitmap::create(const uint8_t* buffer, uint16_t buffer_width, uint16_t buffer_height,
                                         TextureFormat buffer_format, const ArchivItem_Palette* palette)
{
    return create(buffer_width, buffer_height, buffer, buffer_width, buffer_height, buffer_format, palette);
}

template<class T_PixelBuffer>
inline int baseArchivItem_Bitmap::create(const T_PixelBuffer& pixelBuffer, const ArchivItem_Palette* palette)
{
    return create(pixelBuffer.getWidth(), pixelBuffer.getHeight(), pixelBuffer, palette);
}

template<class T_PixelBuffer>
inline int baseArchivItem_Bitmap::create(uint16_t width, uint16_t height, const T_PixelBuffer& pixelBuffer,
                                         const ArchivItem_Palette* palette)
{
    return create(width, height, pixelBuffer.getPixelPtr(), pixelBuffer.getWidth(), pixelBuffer.getHeight(),
                  traits::GetFormat<T_PixelBuffer>::value, palette);
}

template<class T_PixelBuffer>
void flipVertical(T_PixelBuffer&& buffer)
{
    if(buffer.getNumPixels() == 0)
        return;
    std::vector<uint8_t> tmp(buffer.getWidth() * sizeof(typename std::remove_reference_t<T_PixelBuffer>::PixelType));
    assert(buffer.getSizeInBytes() >= tmp.size());
    uint8_t* topIt = buffer.getPixelPtr();
    uint8_t* botIt = buffer.getPixelPtr() + buffer.getSizeInBytes() - tmp.size();
    for(unsigned y = 0; y < buffer.getHeight() / 2u; y++)
    {
        assert(topIt + tmp.size() <= botIt);
        // Top row to tmp
        std::copy(topIt, topIt + tmp.size(), tmp.begin());
        // Bottom to top
        std::copy(botIt, botIt + tmp.size(), topIt);
        // Tmp to bottom
        std::copy(tmp.begin(), tmp.end(), botIt);
        topIt += tmp.size();
        botIt -= tmp.size();
    }
}

} // namespace libsiedler2
