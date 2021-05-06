// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "CopyPixelBuffer.h"
#include "ArchivItem_Palette.h"
#include "ColorBGRA.h"
#include <PixelBufferRef.h>
#include <algorithm>

namespace {
using namespace libsiedler2;
/// Adjust value and size so it fits [0, maxSize]
void clipValues(uint16_t& offset, uint16_t& size, const uint16_t maxSize)
{
    if(offset < maxSize)
    {
        size = std::min<uint16_t>(size, maxSize - offset);
    } else
    {
        offset = maxSize;
        size = 0;
    }
}

bool isTransparent(const PixelBufferBGRARef&, const uint32_t* pixelPtr)
{
    return ColorBGRA::fromBGRA(pixelPtr).getAlpha() == 0;
}
bool isTransparent(const PixelBufferPalettedRef& buffer, const uint8_t* pixelPtr)
{
    return buffer.getPalette().isTransparent(*pixelPtr);
}

template<class T_Buf, typename T>
void copyPixel(const T_Buf&, const T_Buf&, const T* srcPtr, T* dstPtr)
{
    *dstPtr = *srcPtr;
}
void copyPixel(const PixelBufferPalettedRef& src, const PixelBufferBGRARef&, const uint8_t* srcPtr, uint32_t* dstPtr)
{
    ColorBGRA(src.getPalette().get(*srcPtr)).toBGRA(dstPtr);
}
void copyPixel(const PixelBufferBGRARef&, const PixelBufferPalettedRef& dst, const uint32_t* srcPtr, uint8_t* dstPtr)
{
    *dstPtr = dst.getPalette().lookup(ColorBGRA::fromBGRA(srcPtr));
}
} // namespace

namespace libsiedler2 {

/// Move and resize the rect so it fits into [(0,0), (w,h)]
Rect clipRect(Rect rect, uint16_t w, uint16_t h)
{
    clipValues(rect.x, rect.w, w);
    clipValues(rect.y, rect.h, h);
    return rect;
}

template<class T_Src, class T_Dst>
void CopyPixelBuffer(const T_Src& src, T_Dst& dst, Rect srcRect, Rect dstRect)
{
    srcRect = clipRect(srcRect, src.getWidth(), src.getHeight());
    dstRect = clipRect(dstRect, dst.getWidth(), dst.getHeight());
    const uint16_t copyWidth = std::min(srcRect.w, dstRect.w);
    const uint16_t copyHeight = std::min(srcRect.h, dstRect.h);

    if(copyWidth == 0)
        return;

    for(uint16_t y = 0; y < copyHeight; ++y)
    {
        const auto* srcPtr = src.getPixelPtr(srcRect.x, y + srcRect.y);
        auto* dstPtr = dst.getPixelPtr(dstRect.x, y + dstRect.y);
        for(uint16_t x = 0; x < copyWidth; ++x)
        {
            // Don't change if transparent
            if(!isTransparent(src, srcPtr))
            {
                copyPixel(src, dst, srcPtr, dstPtr);
            }
            ++srcPtr;
            ++dstPtr;
        }
    }
}

template void CopyPixelBuffer(const PixelBufferPalettedRef&, PixelBufferPalettedRef&, Rect, Rect);
template void CopyPixelBuffer(const PixelBufferBGRARef&, PixelBufferPalettedRef&, Rect, Rect);
template void CopyPixelBuffer(const PixelBufferPalettedRef&, PixelBufferBGRARef&, Rect, Rect);
template void CopyPixelBuffer(const PixelBufferBGRARef&, PixelBufferBGRARef&, Rect, Rect);
} // namespace libsiedler2
