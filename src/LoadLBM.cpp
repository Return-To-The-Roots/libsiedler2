// $Id: LoadLBM.cpp 9359 2014-04-25 15:37:22Z FloSoft $
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
#include "ArchivItem_Bitmap.h"
#include "ArchivInfo.h"
#include "prototypen.h"
#include <EndianStream.h>
#include <boost/iostreams/device/mapped_file.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/interprocess/smart_ptr/unique_ptr.hpp>
#include <cstring>

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
int libsiedler2::loader::LoadLBM(const std::string& file, ArchivInfo& items)
{
    if(file.empty())
        return 1;

    // Datei zum lesen öffnen
    boost::iostreams::mapped_file_source mmapFile(file);
    typedef boost::iostreams::stream<boost::iostreams::mapped_file_source> MMStream;
    MMStream mmapStream(mmapFile);
    libendian::EndianIStream<true, MMStream& > lbm(mmapStream);

    // hat das geklappt?
    if(!lbm)
        return 2;

    char header[4], pbm[4];
    // Header einlesen
    lbm >> header;

    // ist es eine LBM-File? (Header "FORM")
    if(strncmp(header, "FORM", 4) != 0)
        return 4;

    // Länge einlesen
    unsigned length;
    lbm >> length;

    // Typ einlesen
    lbm >> pbm;

    // ist es eine LBM-File? (Typ "PBM ")
    if(strncmp(pbm, "PBM ", 4) != 0)
        return 7;

    boost::interprocess::unique_ptr< baseArchivItem_Bitmap, Deleter<baseArchivItem_Bitmap> > bitmap(dynamic_cast<baseArchivItem_Bitmap*>(getAllocator().create(BOBTYPE_BITMAP_RAW)));
    bitmap->setFormat(FORMAT_PALETTED);
    ArchivItem_Palette* palette = NULL;

    // lbm has normally 2 items, palette (1) and image (0),
    // i'll change position of those items to be compatible with LoadBMP
    items.alloc(2);

    unsigned short width, height;
    unsigned short compression;
    unsigned chunk;
    // Chunks einlesen
    while(lbm.readNoExcept(chunk))
    {
        switch(chunk)
        {
            case 0x424D4844: // "BHMD"
            {

                // Länge einlesen
                lbm >> length;

                // Bei ungerader Zahl aufrunden
                if(length & 1)
                    ++length;

                lbm >> width >> height;

                bitmap->setWidth(width);
                bitmap->setHeight(height);

                // Unbekannte Daten ( 4 Byte ) berspringen
                lbm.ignore(4);

                // Farbtiefe einlesen
                unsigned short depth;
                lbm >> depth;

                // Nur 256 Farben und nicht mehr!
                if(depth != 256 * 8)
                    return 13;

                // Kompressionflag lesen
                lbm >> compression;

                // Keine bekannte Kompressionsart?
                if(compression != 0 && compression != 256)
                    return 15;

                length -= 12;

                // Rest überspringen
                lbm.ignore(length);
            } break;
            case 0x434D4150: // "CMAP"
            {
                // Länge einlesen
                lbm >> length;

                // Bei ungerader Zahl aufrunden
                if(length & 1)
                    ++length;

                // Ist Länge wirklich so groß wie Farbtabelle?
                if(length != 256 * 3)
                    return 17;

                // Daten von Item auswerten
                palette = (ArchivItem_Palette*)getAllocator().create(BOBTYPE_PALETTE);
                items.set(1, palette);
                palette->load(lbm.getStream(), false);
            } break;
            case 0x424F4459: // "BODY"
            {
                // Länge einlesen
                lbm >> length;

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
                        if(length != width * height)
                            return 222;
                        for(int y = 0; y<height; ++y)
                            for(int x = 0; x<width; ++x)
                            {
                                unsigned char color;
                                if(libendian::le_read_uc(&color, 1, lbm) != 1)
                                    return 22;
                                bitmap->tex_setPixel(x, y, color, palette);
                            }
                    } break;
                    case 256: // komprimiert (RLE?)
                    {
                        // Welcher Pixel ist dran?
                        unsigned short x = 0, y = 0;

                        // Solange einlesen, bis Block zuende bzw. Datei zuende ist
                        while(length > 0 && !lbm.isEOF())
                        {
                            // Typ lesen
                            signed char ctype;
                            lbm >> ctype;
                            --length;
                            if(length == 0)
                                continue;

                            if(ctype > 0) // unkomprimierte Pixel
                            {
                                short count = 1 + static_cast<short>(ctype);

                                for(short i = 0; i < count; ++i)
                                {
                                    // Farbe auslesen
                                    unsigned char color;
                                    lbm >> color;
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
                                short count = 1 - static_cast<short>(ctype);

                                // Farbe auslesen
                                unsigned char color;
                                lbm >> color;
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
                items.set(0, bitmap.release());
            } break;
            default:
            {
                // Länge einlesen
                lbm >> length;

                // Bei ungerader Zahl aufrunden
                if(length & 1)
                    ++length;

                // Rest überspringen
                lbm.ignore(length);
            } break;
        }
    }

    if(items.size() == 0 || !lbm.isEOF())
        return 25;

    return 0;
}
