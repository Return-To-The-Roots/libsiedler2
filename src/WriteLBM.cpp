// Copyright (c) 2005 - 2020 Settlers Freaks (sf-team at siedler25.org)
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

#include "Archiv.h"
#include "ArchivItem_Bitmap.h"
#include "ArchivItem_Palette.h"
#include "ArchivItem_PaletteAnimation.h"
#include "ErrorCodes.h"
#include "PixelBufferPaletted.h"
#include "prototypen.h"
#include "libendian/EndianOStreamAdapter.h"
#include <boost/nowide/fstream.hpp>

int libsiedler2::loader::WriteLBM(const boost::filesystem::path& filepath, const Archiv& items, const ArchivItem_Palette* palette)
{
    if(filepath.empty())
        return ErrorCode::INVALID_BUFFER;

    const auto* bmp = dynamic_cast<const baseArchivItem_Bitmap*>(items[0]);
    if(!bmp)
        return ErrorCode::WRONG_ARCHIV;
    if(bmp->getPalette())
        palette = bmp->getPalette();
    if(!palette)
        return ErrorCode::PALETTE_MISSING;

    // Open file for writing
    libendian::EndianOStreamAdapter<true, boost::nowide::ofstream> fs(filepath, std::ios_base::binary);

    // hat das geklappt?
    if(!fs)
        return ErrorCode::FILE_NOT_ACCESSIBLE;

    fs.write("FORM", 4);
    const long lenPos = fs.getPosition();
    fs << uint32_t(0);
    fs.write("PBM ", 4);

    const uint16_t width = bmp->getWidth();
    const uint16_t height = bmp->getHeight();
    fs.write("BMHD", 4);
    fs << uint32_t(20) << width << height << int16_t(0) << int16_t(0) // origin
       << uint8_t(8) << uint8_t(0) << uint8_t(0)                      // numPlanes, mask, no compression
       << uint8_t(0) << uint16_t(0)                                   // pad and transClr
       << uint8_t(0) << uint8_t(0)                                    // aspect ratio
       << int16_t(320) << uint16_t(200);                              // page size

    const uint8_t transparentIdx = palette->hasTransparency() ? palette->getTransparentIdx() : 0;
    fs.write("CMAP", 4);
    fs << uint32_t(256 * 3); // len = numColors * RGB
    // Hack due to S2 format: If the transparent index is not zero, make it zero
    if(transparentIdx != 0)
    {
        ArchivItem_Palette tmpPal{*palette};
        const auto clr0 = tmpPal[0];
        tmpPal.set(0, tmpPal[transparentIdx]);
        tmpPal.set(transparentIdx, clr0);
        if(int ec = tmpPal.write(fs.getStream(), false))
            return ec;
    } else if(int ec = palette->write(fs.getStream(), false))
        return ec;

    for(unsigned i = 1; i < items.size(); i++)
    {
        if(!items[i] || items[i]->getBobType() != BobType::PaletteAnim)
            return ErrorCode::WRONG_ARCHIV;
        fs.write("CRNG", 4);
        fs << uint32_t(8);
        if(int ec = static_cast<const ArchivItem_PaletteAnimation*>(items[i])->write(fs.getStream()))
            return ec;
    }

    fs.write("BODY", 4);
    PixelBufferPaletted pixels(width, height, transparentIdx);
    if(int ec = bmp->print(pixels, palette))
        return ec;
    // Hack for S2 compatibility: Swap transparent Idx
    if(transparentIdx != 0)
    {
        for(auto& c : pixels.getPixels())
        {
            if(c == transparentIdx)
                c = 0;
            else if(c == 0)
                c = transparentIdx;
        }
    }
    fs << uint32_t(width * height) << pixels.getPixels();

    const long size = fs.getPosition();
    fs.setPosition(lenPos);
    fs << uint32_t(size - 8); // - header and length

    return ErrorCode::NONE;
}
