// $Id: LoadLBM.cpp 9359 2014-04-25 15:37:22Z FloSoft $
//
// Copyright (c) 2005 - 2011 Settlers Freaks (sf-team at siedler25.org)
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

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#if defined _WIN32 && defined _DEBUG && defined _MSC_VER
#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////////
/**
 *  lädt eine LBM-File in ein ArchivInfo.
 *
 *  @param[in]  file    Dateiname der LBM-File
 *  @param[out] items   ArchivInfo-Struktur, welche gefüllt wird
 *
 *  @return Null bei Erfolg, ein Wert ungleich Null bei Fehler
 *
 *  @author FloSoft
 *  @author OLiver
 */
int libsiedler2::loader::LoadLBM(const char* file, ArchivInfo* items)
{
    FILE* lbm;
    char header[4], pbm[4];
    unsigned int chunk;
    unsigned int length;
    long size;

    if(file == NULL || items == NULL)
        return 1;

    // Datei zum lesen öffnen
    lbm = fopen(file, "rb");

    // hat das geklappt?
    if(lbm == NULL)
        return 2;

    fseek(lbm, 0, SEEK_END);
    size = ftell(lbm);
    fseek(lbm, 0, SEEK_SET);

    // Header einlesen
    if(libendian::le_read_c(header, 4, lbm) != 4)
        return 3;

    // ist es eine LBM-File? (Header "FORM")
    if(strncmp(header, "FORM", 4) != 0)
        return 4;

    // Länge einlesen
    if(libendian::le_read_ui(&length, lbm) != 0)
        return 5;

    // Typ einlesen
    if(libendian::le_read_c(pbm, 4, lbm) != 4)
        return 6;

    // ist es eine LBM-File? (Typ "PBM ")
    if(strncmp(pbm, "PBM ", 4) != 0)
        return 7;

    baseArchivItem_Bitmap* bitmap = dynamic_cast<baseArchivItem_Bitmap*>((*allocator)(BOBTYPE_BITMAP_RAW, 0, NULL));
    bitmap->setFormat(FORMAT_PALETTED);
    ArchivItem_Palette* palette = NULL;
    unsigned short compression;
    unsigned short width, height;
    unsigned short depth;

    // lbm has normally 2 items, palette (1) and image (0),
    // i'll change position of those items to be compatible with LoadBMP
    items->alloc(2);

    // Chunks einlesen
    while(!feof(lbm) && ftell(lbm) < size)
    {
        // Chunk-Typ einlesen
        if(libendian::be_read_ui(&chunk, lbm) != 0)
            return 8;

        switch(chunk)
        {
            case 0x424D4844: // "BHMD"
            {
                // Länge einlesen
                if(libendian::be_read_ui(&length, lbm) != 0)
                    return 9;

                // Bei ungerader Zahl aufrunden
                if(length & 1)
                    ++length;

                if(libendian::be_read_us(&width, lbm) != 0)
                    return 10;

                if(libendian::be_read_us(&height, lbm) != 0)
                    return 11;

                bitmap->setWidth(width);
                bitmap->setHeight(height);

                // Unbekannte Daten ( 4 Byte ) berspringen
                fseek(lbm, 4, SEEK_CUR);

                // Farbtiefe einlesen
                if(libendian::le_read_us(&depth, lbm) != 0)
                    return 12;

                // Nur 256 Farben und nicht mehr!
                if(depth != 8)
                    return 13;

                // Kompressionflag lesen
                if(libendian::le_read_us(&compression, lbm) != 0)
                    return 14;

                // Keine bekannte Kompressionsart?
                if(compression > 1)
                    return 15;

                length -= 12;

                // Rest überspringen
                fseek(lbm, length, SEEK_CUR);
            } break;
            case 0x434D4150: // "CMAP"
            {
                // Länge einlesen
                if(libendian::be_read_ui(&length, lbm) != 0)
                    return 16;

                // Bei ungerader Zahl aufrunden
                if(length & 1)
                    ++length;

                // Ist Länge wirklich so groß wie Farbtabelle?
                if(length != 256 * 3)
                    return 17;

                // Daten von Item auswerten
                palette = (ArchivItem_Palette*)(*allocator)(BOBTYPE_PALETTE, 0, NULL);
                items->set(1, palette);

                // Farbpalette lesen
                unsigned char colors[256][3];
                if(libendian::le_read_uc(colors[0], 256 * 3, lbm) != 256 * 3)
                    return 18;

                // Farbpalette zuweisen
                for(unsigned int k = 0; k < 256; ++k)
                    palette->set(k, colors[k][0], colors[k][1], colors[k][2]);
            } break;
            case 0x424F4459: // "BODY"
            {
                // Länge einlesen
                if(libendian::be_read_ui(&length, lbm) != 0)
                    return 19;

                // Bei ungerader Zahl aufrunden
                if(length & 1)
                    ++length;

                // haben wir eine Palette erhalten?
                if(palette == NULL)
                    return 20;

                bitmap->setPalette(palette);
                bitmap->tex_alloc();

                switch(compression)
                {
                    case 0: // unkomprimiert
                    {
                        /*assert(false);*/
                    } break;
                    case 1: // komprimiert (RLE?)
                    {
                        // Welcher Pixel ist dran?
                        unsigned short x = 0, y = 0;
                        unsigned char ctype;
                        unsigned char color;

                        // Solange einlesen, bis Block zuende bzw. Datei zuende ist
                        while(length >= 0 && !feof(lbm) && ftell(lbm) < size)
                        {
                            // Typ lesen
                            if(libendian::le_read_uc(&ctype, 1, lbm) != 1)
                                return 21;
                            --length;
                            if(length == 0)
                                continue;

                            if(ctype < 128) // unkomprimierte Pixel
                            {
                                unsigned short count = ctype + 1;

                                for(unsigned short i = 0; i < count; ++i)
                                {
                                    // Farbe auslesen
                                    if(libendian::le_read_uc(&color, 1, lbm) != 1)
                                        return 22;
                                    --length;

                                    bitmap->tex_setPixel(x++, y, color, palette);
                                    if(x >= width)
                                    {
                                        ++y;
                                        x = 0;
                                    }
                                }
                            }
                            else // komprimierte Pixel
                            {
                                unsigned short count = 0xFF - ctype + 2;

                                // Farbe auslesen
                                if(libendian::le_read_uc(&color, 1, lbm) != 1)
                                    return 23;
                                --length;

                                for(unsigned short i = 0; i < count; ++i)
                                {
                                    bitmap->tex_setPixel(x++, y, color, palette);
                                    if(x >= width)
                                    {
                                        ++y;
                                        x = 0;
                                    }
                                }
                            }
                        }
                    } break;
                }
                items->set(0, bitmap);
            } break;
            default:
            {
                // Länge einlesen
                if(libendian::be_read_ui(&length, lbm) != 0)
                    return 24;

                // Bei ungerader Zahl aufrunden
                if(length & 1)
                    ++length;

                // Rest überspringen
                fseek(lbm, length, SEEK_CUR);
            } break;
        }
    }

    if(items->getCount() == 0)
        return 25;

    // Datei schliessen
    fclose(lbm);

    return 0;
}
