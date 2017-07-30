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
#include "ColorARGB.h"
#include "libsiedler2.h"
#include "IAllocator.h"
#include "BmpHeader.h"
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
                                    std::vector<uint8_t>& buffer,
                                    std::vector<uint8_t>& tmpBuffer)
{
    if(bbp == 1)
    {
        bmp.read(&buffer[y * width], width);
    } else
    {
        bmp >> tmpBuffer;
        for(uint16_t x = 0; x < width; ++x)
        {
            libsiedler2::ColorRGB clr = libsiedler2::ColorRGB::fromBGR(&tmpBuffer[x * 3]);
            libsiedler2::ColorARGB clrOut;
            if(clr != libsiedler2::TRANSPARENT_COLOR)
                clrOut = clr;
            clrOut.toBGRA(&buffer[(y * width + x) * 4]);
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
    libendian::EndianIStreamAdapter<false, MMStream& > bmpFs(mmapStream);

    // hat das geklappt?
    if(!bmpFs)
        return 2;

    BmpFileHeader bmhd;

    if(!bmpFs.readRaw(&bmhd, 1))
        return 3;

    if(bmhd.header[0] != 'B' || bmhd.header[1] != 'M')
        return 4;

    BitmapInfoHeader bmih;
    if(!bmpFs.readRaw(&bmih, 1))
        return 5;

    if(bmih.headerSize != sizeof(bmih))
        return 6;

    if(bmih.height >= 0)
        bottomup = true;
    else
        bmih.height = -bmih.height;

    // nur eine Ebene
    if(bmih.planes != 1)
        return 6;

    boost::interprocess::unique_ptr< baseArchivItem_Bitmap, Deleter<baseArchivItem_Bitmap> > bitmap(dynamic_cast<baseArchivItem_Bitmap*>(getAllocator().create(BOBTYPE_BITMAP_RAW)));
    bitmap->setName(file);

    // keine Kompression
    if(bmih.compression != 0)
        return 8;

    // Einträge in der Farbtabelle
    if(bmih.clrused == 0)
        bmih.clrused = (int)pow(2.0, bmih.bpp);

    ArchivItem_Palette* pal = NULL;
    if(bmih.bpp == 8)
    {
        // Farbpalette lesen
        uint8_t colors[256][4];
        if(!bmpFs.read(colors[0], bmih.clrused * 4))
            return 99;

        pal = dynamic_cast<ArchivItem_Palette*>(getAllocator().create(BOBTYPE_PALETTE));
        for(int i = 0; i < bmih.clrused; ++i)
            pal->set(i, ColorARGB::fromBGRA(&colors[i][0]));
    }

    uint8_t bbp = (bmih.bpp / 8);
    if(bbp != 1 && bbp != 3)
        return 10;
    std::vector<uint8_t> buffer(bmih.height * bmih.width * (bbp == 1 ? 1 : 4));
    std::vector<uint8_t> tmpBuffer(bmih.width * bbp);

    bmpFs.setPosition(bmhd.pixelOffset);

    // Bitmap Pixel einlesen
    if(bottomup)
    {
        // Bottom-Up, "von unten nach oben"
        for(int y = bmih.height - 1; y >= 0; --y)
            LoadBMP_ReadLine(bmpFs, y, bmih.width, bbp, buffer, tmpBuffer);
    }
    else
    {
        // Top-Down, "von oben nach unten"
        for(int y = 0; y < bmih.height; ++y)
            LoadBMP_ReadLine(bmpFs, y, bmih.width, bbp, buffer, tmpBuffer);
    }

    if(!bitmap->create(bmih.width, bmih.height, &buffer[0], bmih.width, bmih.height,
        (bbp == 1) ? FORMAT_PALETTED : FORMAT_BGRA, pal))
        return 22;

    // Bitmap zuweisen
    image.push(bitmap.release());

    // alles ok
    return (!bmpFs) ? 99 : 0;
}

