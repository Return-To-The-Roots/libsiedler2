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
#include "IAllocator.h"
#include "OpenMemoryStream.h"
#include "fileFormatHelpers.h"
#include "libsiedler2.h"
#include "prototypen.h"
#include "libendian/EndianIStreamAdapter.h"
#include "s25util/strAlgos.h"
#include <boost/filesystem/path.hpp>
#include <iostream>
#include <memory>

/**
 *  lädt eine LBM-File in ein Archiv.
 *
 *  @param[in]  filepath    Dateiname der LBM-File
 *  @param[out] items   Archiv-Struktur, welche gefüllt wird
 *
 *  @return Null bei Erfolg, ein Wert ungleich Null bei Fehler
 */
int libsiedler2::loader::LoadLBM(const boost::filesystem::path& filepath, Archiv& items)
{
    MMStream mmapStream;
    if(int ec = openMemoryStream(filepath, mmapStream))
        return ec;
    libendian::EndianIStreamAdapter<true, MMStream&> lbm(mmapStream);

    std::array<char, 4> header, pbm;
    uint32_t length;
    // Header einlesen
    lbm >> header >> length >> pbm;

    // ist es eine LBM-File? (Header "FORM")
    if(!lbm || !isChunk(header, "FORM") || !isChunk(pbm, "PBM "))
        return ErrorCode::WRONG_HEADER;

    // (at least) 1 item
    items.alloc(1);

    auto bitmap = getAllocator().create<baseArchivItem_Bitmap>(BobType::Bitmap);

    uint16_t width = 0, height = 0, transClr = 0;
    uint8_t compression = 0, mask = 0;
    std::array<char, 4> chunk;
    bool headerRead = false;
    // Chunks einlesen
    while(lbm.read(chunk))
    {
        uint32_t chunkLen;
        lbm >> chunkLen;

        // Bei ungerader Zahl aufrunden
        if(chunkLen & 1)
            ++chunkLen;

        if(isChunk(chunk, "BMHD"))
        {
            if(headerRead || !bitmap)
                return ErrorCode::WRONG_FORMAT;
            uint16_t xOrig, yOrig, pageW, pageH;
            uint8_t numPlanes, pad, xAspect, yAspect;

            lbm >> width >> height >> xOrig >> yOrig >> numPlanes >> mask >> compression >> pad >> transClr >> xAspect
              >> yAspect >> pageW >> pageH;

            // Nur 256 Farben und nicht mehr!
            if(numPlanes != 8 || (mask != 0 && mask != 2))
                return ErrorCode::WRONG_FORMAT;

            // Keine bekannte Kompressionsart?
            if(compression > 1u)
                return ErrorCode::WRONG_FORMAT;

            headerRead = true;
        } else if(isChunk(chunk, "CRNG"))
        {
            if(!bitmap)
                return ErrorCode::WRONG_FORMAT;
            auto anim = getAllocator().create<ArchivItem_PaletteAnimation>(BobType::PaletteAnim);
            if(int ec = anim->load(lbm.getStream()))
                return ec;
            items.push(std::move(anim));
        } else if(isChunk(chunk, "CMAP"))
        {
            if(!bitmap)
                return ErrorCode::WRONG_FORMAT;
            // Ist Länge wirklich so groß wie Farbtabelle?
            if(chunkLen != 256 * 3)
                return ErrorCode::WRONG_FORMAT;

            auto palette = getAllocator().create<ArchivItem_Palette>(BobType::Palette);
            if(int ec = palette->load(lbm.getStream(), false))
                return ec;
            if(mask == 2 && transClr < 256)
                palette->setTransparentIdx(static_cast<uint8_t>(transClr));
            else
            {
                // For S2 *.LBMs the transparent index is always 0
                palette->setTransparentIdx(0);
            }
            bitmap->setPalette(std::move(palette));
        } else if(isChunk(chunk, "BODY"))
        {
            if(!headerRead || !bitmap)
                return ErrorCode::WRONG_FORMAT;
            // haben wir eine Palette erhalten?
            if(bitmap->getPalette() == nullptr)
                return ErrorCode::PALETTE_MISSING;

            bitmap->init(width, height, ArchivItem_BitmapBase::getWantedFormat(TextureFormat::Paletted));

            if(compression == 0) // unkomprimiert
            {
                if(chunkLen != static_cast<uint32_t>(width * height))
                    return ErrorCode::WRONG_FORMAT;
                for(int y = 0; y < height; ++y)
                {
                    for(int x = 0; x < width; ++x)
                    {
                        uint8_t color;
                        lbm >> color;
                        bitmap->setPixel(x, y, color);
                    }
                }
            } else // komprimiert (RLE?)
            {
                // Welcher Pixel ist dran?
                uint16_t x = 0, y = 0;

                // Solange einlesen, bis Block zuende bzw. Datei zuende ist
                while(chunkLen > 0 && !lbm.eof())
                {
                    // Typ lesen
                    int8_t ctype;
                    lbm >> ctype;
                    --chunkLen;
                    if(chunkLen == 0)
                        continue;

                    if(ctype > 0) // unkomprimierte Pixel
                    {
                        int16_t count = 1 + static_cast<int16_t>(ctype);

                        for(int16_t i = 0; i < count; ++i)
                        {
                            // Farbe auslesen
                            uint8_t color;
                            lbm >> color;
                            --chunkLen;

                            bitmap->setPixel(x++, y, color);
                            if(x >= width)
                            {
                                ++y;
                                x = 0;
                            }
                        }
                    } else // komprimierte Pixel
                    {
                        int16_t count = 1 - static_cast<int16_t>(ctype);

                        // Farbe auslesen
                        uint8_t color;
                        lbm >> color;
                        --chunkLen;

                        for(uint16_t i = 0; i < count; ++i)
                        {
                            bitmap->setPixel(x++, y, color);
                            if(x >= width)
                            {
                                ++y;
                                x = 0;
                            }
                        }
                    }
                }
            }
            items.set(0, std::move(bitmap));
            bitmap = nullptr; // Also marker that BODY was found
        } else
        {
            // Skip this chunk
            lbm.ignore(chunkLen);
        }
    }

    if(items.empty() || !items[0] || !lbm.eof() || bitmap)
        return ErrorCode::WRONG_FORMAT;

    return ErrorCode::NONE;
}
