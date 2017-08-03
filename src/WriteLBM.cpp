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

#include "libSiedler2Defines.h" // IWYU pragma: keep
#include "ArchivItem_Bitmap.h"
#include "ArchivItem_Palette.h"
#include "ArchivInfo.h"
#include "prototypen.h"
#include "ErrorCodes.h"
#include "PixelBufferPaletted.h"
#include "libendian/src/EndianOStreamAdapter.h"
#include <boost/filesystem/fstream.hpp>

int libsiedler2::loader::WriteLBM(const std::string& file, const ArchivInfo& items, const ArchivItem_Palette* palette)
{
    if(file.empty())
        return ErrorCode::INVALID_BUFFER;

    const baseArchivItem_Bitmap* bmp = dynamic_cast<const baseArchivItem_Bitmap*>(items[0]);
    if(items.size() != 1u || !bmp)
        return ErrorCode::WRONG_ARCHIV;
    if(bmp->getPalette())
        palette = bmp->getPalette();
    if(!palette)
        return ErrorCode::PALETTE_MISSING;

    // Datei zum schreiben öffnen
    libendian::EndianOStreamAdapter<true, bfs::ofstream> fs(file, std::ios_base::binary);

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
    fs << uint32_t(20) << width << height
        << int16_t(0) << int16_t(0) // origin
        << uint8_t(8) << uint8_t(0) << uint8_t(256) // numPlanes, mask, no compression
        << uint8_t(0) << uint16_t(0) // pad and transClr
        << uint8_t(0) << uint8_t(0) // aspect ratio
        << int16_t(320) << uint16_t(200); // page size

    fs.write("CMAP", 4);
    fs << uint32_t(256 * 3); // len = numColors * RGB
    if(int ec = palette->write(fs.getStream(), false))
        return ec;

    fs.write("BODY", 4);
    PixelBufferPaletted pixels(width, height);
    if(int ec = bmp->print(pixels.getPixelPtr(), width, height, FORMAT_PALETTED, palette))
        return ec;
    fs << uint32_t(width * height) << pixels.getPixels();

    const long size = fs.getPosition();
    fs.setPosition(lenPos);
    fs << uint32_t(size - 4);

    return ErrorCode::NONE;
}
