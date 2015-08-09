// $Id: LoadBMP.cpp 9359 2014-04-25 15:37:22Z FloSoft $
//
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

///////////////////////////////////////////////////////////////////////////////
// Header
#include "main.h"
#include <boost/scoped_ptr.hpp>
#include <boost/interprocess/smart_ptr/unique_ptr.hpp>
#include <vector>

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#if defined _WIN32 && defined _DEBUG && defined _MSC_VER
#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////////
/**
 *  liest eine Bitmapzeile
 *
 *  @author OLiver
 */
static inline void LoadBMP_ReadLine(FILE* bmp,
                                    unsigned short y,
                                    unsigned int bmih_size,
                                    unsigned int size,
                                    unsigned int width,
                                    unsigned char bbp,
                                    libsiedler2::baseArchivItem_Bitmap* bitmap,
                                    unsigned char* buffer)
{
    libendian::le_read_uc(buffer, width * bbp, bmp);

    for(unsigned short x = 0; x < width; ++x)
    {
        switch(bbp)
        {
            case 1: // 256
            {
                bitmap->tex_setPixel(x, y, buffer[x * bbp], NULL);
            } break;
            case 3: // 24 bit
            {
                if(buffer[x * bbp + 2] == 0xFF && buffer[x * bbp + 1] == 0x00 && buffer[x * bbp + 0] == 0x8F) // transparenz? (color-key "rosa")
                    bitmap->tex_setPixel(x, y, 0, 0, 0, 0x00);
                else
                    bitmap->tex_setPixel(x, y, buffer[x * bbp + 2], buffer[x * bbp + 1], buffer[x * bbp + 0], 0xFF);
            } break;
        }
    }
    if(width * bbp % 4 > 0)
        fseek(bmp, 4 - (width * bbp % 4), SEEK_CUR);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  lädt eine BMP-File in ein ArchivInfo.
 *
 *  @param[in]  file    Dateiname der BMP-File
 *  @param[out] items   ArchivInfo-Struktur, welche gefüllt wird
 *
 *  @return Null bei Erfolg, ein Wert ungleich Null bei Fehler
 *
 *  @todo RGB Bitmaps (Farben > 8Bit) ebenfalls einlesen.
 *
 *  @author FloSoft
 */
int libsiedler2::loader::LoadBMP(const std::string& file, ArchivItem*& image, ArchivItem** palette)
{
    struct BMHD
    {
        unsigned short header; // 2
        unsigned int size; // 6
        unsigned int reserved; // 10
        unsigned int offset; // 14
    } bmhd;

    struct BMIH
    {
        unsigned int length; // 4
        int width; // 8
        int height; // 12
        short planes; // 14
        short bbp; // 16
        unsigned int compression; // 20
        unsigned int size; // 24
        int xppm; // 28
        int yppm; // 32
        int clrused; // 36
        int clrimp; // 40
    } bmih ;

    image = NULL;
    if(palette)
        *palette = NULL;

    bool bottomup = false;

    if(file.empty())
        return 1;

    // Datei zum lesen öffnen
    boost::scoped_ptr<FILE> bmp(fopen(file.c_str(), "rb"));

    // hat das geklappt?
    if(!bmp)
        return 2;

    // Bitmap-Header einlesen
    if(libendian::le_read_us(&bmhd.header, bmp.get()) != 0)
        return 3;
    if(libendian::le_read_ui(&bmhd.size, bmp.get()) != 0)
        return 3;
    if(libendian::le_read_ui(&bmhd.reserved, bmp.get()) != 0)
        return 3;
    if(libendian::le_read_ui(&bmhd.offset, bmp.get()) != 0)
        return 3;

    if(bmhd.header != 0x4D42)
        return 4;

    // Bitmap-Info-Header einlesen
    //if(libendian::le_read_c((char*)&bmih, 40, bmp.get()) != 40)
    //  return 5;
    if(libendian::le_read_ui(&bmih.length, bmp.get()) != 0)
        return 5;
    if(libendian::le_read_i(&bmih.width, bmp.get()) != 0)
        return 5;
    if(libendian::le_read_i(&bmih.height, bmp.get()) != 0)
        return 5;
    if(libendian::le_read_s(&bmih.planes, bmp.get()) != 0)
        return 5;
    if(libendian::le_read_s(&bmih.bbp, bmp.get()) != 0)
        return 5;
    if(libendian::le_read_ui(&bmih.compression, bmp.get()) != 0)
        return 5;
    if(libendian::le_read_ui(&bmih.size, bmp.get()) != 0)
        return 5;
    if(libendian::le_read_i(&bmih.xppm, bmp.get()) != 0)
        return 5;
    if(libendian::le_read_i(&bmih.yppm, bmp.get()) != 0)
        return 5;
    if(libendian::le_read_i(&bmih.clrused, bmp.get()) != 0)
        return 5;
    if(libendian::le_read_i(&bmih.clrimp, bmp.get()) != 0)
        return 5;

    if(bmih.height > 0)
        bottomup = true;

    bmih.height = (bottomup ? bmih.height : -bmih.height);

    // nur eine Ebene
    if(bmih.planes != 1)
        return 6;

    boost::interprocess::unique_ptr< baseArchivItem_Bitmap, Deleter<baseArchivItem_Bitmap> > bitmap(dynamic_cast<baseArchivItem_Bitmap*>(allocator->create(BOBTYPE_BITMAP_RAW, 0)));
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
    {
        return 8;
    }

    // Einträge in der Farbtabelle
    if(bmih.clrused == 0)
        bmih.clrused = (int)pow(2.0, bmih.bbp);

    //items->alloc(2);

    if(bmih.bbp == 8)
    {
        if(palette)
            *palette = (ArchivItem_Palette*)allocator->create(BOBTYPE_PALETTE, 0);
        //ArchivItem_Palette *palette = (ArchivItem_Palette*)allocator->create(BOBTYPE_PALETTE, 0, NULL);
        //items->set(0, palette);

        // Farbpalette lesen
        unsigned char colors[256][4];
        if(libendian::le_read_uc(colors[0], bmih.clrused * 4, bmp.get()) != bmih.clrused * 4){
            return 10;
        }

        // Farbpalette zuweisen
        if(palette)
        {
            for(int i = 0; i < bmih.clrused; ++i)
                dynamic_cast<ArchivItem_Palette*>(*palette)->set(i, colors[i][2], colors[i][1], colors[i][0]);

            bitmap->setPalette(dynamic_cast<ArchivItem_Palette*>(*palette));
        }
    }

    // Bitmapdaten setzen
    bitmap->setWidth(bmih.width);
    bitmap->setHeight(bmih.height);
    bitmap->setLength(bmih.width * bmih.height);
    bitmap->tex_alloc();

    unsigned char bbp = (bmih.bbp / 8);
    unsigned int size = bmih.width * bmih.height * bbp;
    std::vector<unsigned char> buffer(bmih.width * bbp);

    // Bitmap Pixel einlesen
    if(bottomup)
    {
        // Bottom-Up, "von unten nach oben"
        for(int y = bmih.height - 1; y >= 0; --y)
            LoadBMP_ReadLine(bmp.get(), y, bmih.size, size, bmih.width, bbp, bitmap.get(), &buffer.front());
    }
    else
    {
        // Top-Down, "von oben nach unten"
        for(int y = 0; y < bmih.height; ++y)
            LoadBMP_ReadLine(bmp.get(), y, bmih.size, size, bmih.width, bbp, bitmap.get(), &buffer.front());
    }
    if(ftell(bmp.get()) % 4 > 0)
        fseek(bmp.get(), 4 - (ftell(bmp.get()) % 4), SEEK_CUR);

    // Bitmap zuweisen
    image = bitmap.release();

    // alles ok
    return 0;
}

