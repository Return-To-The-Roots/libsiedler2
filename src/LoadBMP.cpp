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

#include "libSiedler2Defines.h" // IWYU pragma: keep
#include "ArchivInfo.h"
#include "ArchivItem_Bitmap.h"
#include "ArchivItem_Palette.h"
#include "prototypen.h"
#include "libsiedler2.h"
#include "IAllocator.h"
#include "libendian/src/EndianIStreamAdapter.h"
#include <boost/interprocess/smart_ptr/unique_ptr.hpp>
#include <boost/iostreams/device/mapped_file.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/filesystem/path.hpp> // For UTF8 support
#include <vector>
#include <cmath>
#include <iostream>

/**
 *  liest eine Bitmapzeile
 */
template<class T_FStream>
static inline void LoadBMP_ReadLine(T_FStream& bmp,
                                    uint16_t y,
                                    uint32_t width,
                                    uint8_t bbp,
                                    libsiedler2::baseArchivItem_Bitmap& bitmap,
                                    std::vector<uint8_t>& buffer)
{
    bmp >> buffer;

    for(uint16_t x = 0; x < width; ++x)
    {
        switch(bbp)
        {
            case 1: // 256
            {
                bitmap.tex_setPixel(x, y, buffer[x * bbp], NULL);
            } break;
            case 3: // 24 bit
            {
                if(libsiedler2::Color(buffer[x * bbp + 2], buffer[x * bbp + 1], buffer[x * bbp + 0]) == libsiedler2::TRANSPARENT_COLOR) // transparenz? (color-key "rosa")
                    bitmap.tex_setPixel(x, y, 0, 0, 0, 0x00);
                else
                    bitmap.tex_setPixel(x, y, buffer[x * bbp + 2], buffer[x * bbp + 1], buffer[x * bbp + 0], 0xFF);
            } break;
        }
    }
    if(width * bbp % 4 > 0)
        bmp.ignore(4 - (width * bbp % 4));
}

/**
 *  lädt eine BMP-File in ein ArchivInfo.
 *
 *  @param[in]  file    Dateiname der BMP-File
 *  @param[out] items   ArchivInfo-Struktur, welche gefüllt wird
 *
 *  @return Null bei Erfolg, ein Wert ungleich Null bei Fehler
 *
 *  @todo RGB Bitmaps (Farben > 8Bit) ebenfalls einlesen.
 */
int libsiedler2::loader::LoadBMP(const std::string& file, ArchivInfo& image)
{
    struct BMHD
    {
        uint16_t header; // 2
        uint32_t size; // 6
        uint32_t reserved; // 10
        uint32_t offset; // 14
    } bmhd;

    struct BMIH
    {
        uint32_t length; // 4
        int width; // 8
        int height; // 12
        int16_t planes; // 14
        int16_t bbp; // 16
        uint32_t compression; // 20
        uint32_t size; // 24
        int xppm; // 28
        int yppm; // 32
        int clrused; // 36
        int clrimp; // 40
    } bmih ;

    bool bottomup = false;

    if(file.empty())
        return 1;

    // Datei zum lesen öffnen
    boost::iostreams::mapped_file_source mmapFile;
    try{
        mmapFile.open(bfs::path(file));
    }catch(std::exception& e){
        std::cerr << "Could not open '" << file << "': " << e.what() << std::endl;
        return 2;
    }
    typedef boost::iostreams::stream<boost::iostreams::mapped_file_source> MMStream;
    MMStream mmapStream(mmapFile);
    libendian::EndianIStreamAdapter<false, MMStream& > bmp(mmapStream);

    // hat das geklappt?
    if(!bmp)
        return 2;

    // Bitmap-Header einlesen
    bmp >> bmhd.header;
    bmp >> bmhd.size;
    bmp >> bmhd.reserved;
    bmp >> bmhd.offset;

    if(bmhd.header != 0x4D42)
        return 4;

    // Bitmap-Info-Header einlesen
    //bmp >> bmih;
    bmp >> bmih.length;
    bmp >> bmih.width;
    bmp >> bmih.height;
    bmp >> bmih.planes;
    bmp >> bmih.bbp;
    bmp >> bmih.compression;
    bmp >> bmih.size;
    bmp >> bmih.xppm;
    bmp >> bmih.yppm;
    bmp >> bmih.clrused;
    bmp >> bmih.clrimp;

    if(!bmp)
        return 99;

    if(bmih.height > 0)
        bottomup = true;

    bmih.height = (bottomup ? bmih.height : -bmih.height);

    // nur eine Ebene
    if(bmih.planes != 1)
        return 6;

    boost::interprocess::unique_ptr< baseArchivItem_Bitmap, Deleter<baseArchivItem_Bitmap> > bitmap(dynamic_cast<baseArchivItem_Bitmap*>(getAllocator().create(BOBTYPE_BITMAP_RAW)));
    bitmap->setName(file);

    switch(bmih.bbp)
    {
        case 8: // nur 8 Bit
        {
            bitmap->setFormat(libsiedler2::FORMAT_PALETTED);
        } break;
        case 24: // oder 24 Bit
        {
            bitmap->setFormat(libsiedler2::FORMAT_RGBA);
        } break;
        default:
        {
            fprintf(stderr, "unknown bitmap depth: %d ", bmih.bbp);
            return 7;
        } break;
    }

    // keine Kompression
    if(bmih.compression != 0)
        return 8;

    // Einträge in der Farbtabelle
    if(bmih.clrused == 0)
        bmih.clrused = (int)pow(2.0, bmih.bbp);

    if(bmih.bbp == 8)
    {
        // Farbpalette lesen
        uint8_t colors[256][4];
        if(!bmp.read(colors[0], bmih.clrused * 4))
            return 99;

            ArchivItem_Palette* pal = dynamic_cast<ArchivItem_Palette*>(getAllocator().create(BOBTYPE_PALETTE));
            for(int i = 0; i < bmih.clrused; ++i)
                pal->set(i, Color(colors[i][2], colors[i][1], colors[i][0]));

            bitmap->setPalette(pal);
    }

    // Bitmapdaten setzen
    bitmap->setWidth(bmih.width);
    bitmap->setHeight(bmih.height);
    bitmap->tex_alloc();

    uint8_t bbp = (bmih.bbp / 8);
    std::vector<uint8_t> buffer(bmih.width * bbp);

    // Bitmap Pixel einlesen
    if(bottomup)
    {
        // Bottom-Up, "von unten nach oben"
        for(int y = bmih.height - 1; y >= 0; --y)
            LoadBMP_ReadLine(bmp, y, bmih.width, bbp, *bitmap, buffer);
    }
    else
    {
        // Top-Down, "von oben nach unten"
        for(int y = 0; y < bmih.height; ++y)
            LoadBMP_ReadLine(bmp, y, bmih.width, bbp, *bitmap, buffer);
    }

    // Bitmap zuweisen
    image.push(bitmap.release());

    // alles ok
    return (!bmp) ? 99 : 0;
}

