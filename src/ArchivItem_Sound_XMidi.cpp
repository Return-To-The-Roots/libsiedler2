// $Id: ArchivItem_Sound_XMidi.cpp 9359 2014-04-25 15:37:22Z FloSoft $
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
#include "ArchivItem_Sound_XMidi.h"

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#if defined _WIN32 && defined _DEBUG && defined _MSC_VER
#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////////
/** @class libsiedler2::baseArchivItem_Sound_XMidi
 *
 *  Basisklasse für XMIDI-Sounds.
 *
 *  @author FloSoft
 */

///////////////////////////////////////////////////////////////////////////////
/**
 *  Konstruktor von @p baseArchivItem_Sound_XMidi.
 *
 *  @author FloSoft
 */
libsiedler2::baseArchivItem_Sound_XMidi::baseArchivItem_Sound_XMidi(void) : baseArchivItem_Sound()
{
    setType(SOUNDTYPE_XMIDI);

    tracks = 0;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Kopierkonstruktor von @p ArchivItem_Sound_XMidi.
 *
 *  @param[in] item Quellitem
 *
 *  @author FloSoft
 */
libsiedler2::baseArchivItem_Sound_XMidi::baseArchivItem_Sound_XMidi(const baseArchivItem_Sound_XMidi& item) : baseArchivItem_Sound( item )
{
    tracks = item.tracks;

    for(unsigned int i = 0; i < 256; ++i)
        tracklist[i] = item.tracklist[i];
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Destruktor von @p ArchivItem_Sound_XMidi.
 *
 *  @author FloSoft
 */
libsiedler2::baseArchivItem_Sound_XMidi::~baseArchivItem_Sound_XMidi(void)
{
}

int libsiedler2::baseArchivItem_Sound_XMidi::load(FILE* file, unsigned int length)
{
    if(file == NULL || length == 0)
        return 1;

    unsigned int item_length = length;
    long position = ftell(file);

    char header[4], subheader[4];
    unsigned int chunk;

    // Header einlesen
    if(libendian::le_read_c(header, 4, file) != 4)
        return 2;

    // ist es eine XMIDI-File? (Header "FORM")
    if(strncmp(header, "FORM", 4) != 0)
        return 3;

    // Länge einlesen
    if(libendian::be_read_ui(&length, file) != 0)
        return 4;

    // Typ einlesen
    if(libendian::le_read_c(subheader, 4, file) != 4)
        return 5;

    // ist es eine singleTrack-XMIDI-File? (Typ "XMID")
    if(strncmp(subheader, "XMID", 4) == 0)
    {
        tracks = 1;
    }
    else if(strncmp(subheader, "XDIR", 4) == 0)
    {
        long xdir_length = length;
        while(!feof(file) && ftell(file) - position < xdir_length)
        {
            // Chunk-Typ einlesen
            if(libendian::be_read_ui(&chunk, file) != 0)
                return 6;

            switch(chunk)
            {
                case 0x494E464F: // "INFO"
                {
                    // Länge einlesen
                    if(libendian::be_read_ui(&length, file) != 0)
                        return 7;

                    // Bei ungerader Zahl aufrunden
                    if(length & 1)
                        ++length;

                    if(length != 2)
                        return 8;

                    if(libendian::le_read_us(&tracks, file) != 0)
                        return 9;
                } break;
                case 0x43415420: // "CAT "
                {
                    // Länge einlesen
                    if(libendian::be_read_ui(&length, file) != 0)
                        return 10;

                    // Bei ungerader Zahl aufrunden
                    if(length & 1)
                        ++length;

                    // Typ einlesen
                    if(libendian::le_read_c(subheader, 4, file) != 4)
                        return 11;

                    if(strncmp(subheader, "XMID", 4) != 0)
                        return 12;
                } break;
            }
        }
    }
    else
        return 13;

    if(tracks == 0 || tracks > 256)
        return 14;

    unsigned short track_nr = 0;
    while(track_nr < tracks)
    {
        // Chunk-Typ einlesen
        if(libendian::be_read_ui(&chunk, file) != 0)
            return 15;

        switch(chunk)
        {
            case 0x464F524D: // "FORM"
            {
                fseek(file, 4, SEEK_CUR);
            } break;
            case 0x584D4944: // "XMID"
            {
            } break;
            case 0x54494D42: // "TIMB"
            {
                // Länge einlesen
                if(libendian::be_read_ui(&length, file) != 0)
                    return 16;

                // Bei ungerader Zahl aufrunden
                if(length & 1)
                    ++length;

                fseek(file, length, SEEK_CUR);
            } break;
            case 0x45564E54: // "EVNT"
            {
                // Länge einlesen
                if(libendian::be_read_ui(&length, file) != 0)
                    return 17;

                // Bei ungerader Zahl aufrunden
                if(length & 1)
                    ++length;

                if(tracklist[track_nr].readXMid(file, length) != 0)
                    return 18;

                if(tracklist[track_nr].XMid2Mid() != 0)
                    return 19;

                ++track_nr;
            } break;
        }
    }

    // auf jeden Fall kompletten Datensatz überspringen
    fseek(file, position + item_length, SEEK_SET);
    return 0;
}

int libsiedler2::baseArchivItem_Sound_XMidi::write(FILE* file) const
{
    if(!file)
        return 1;

    unsigned int length = 0;
    for(unsigned short i = 0; i < tracks; ++i)
        length += tracklist[i].getMidLength(false);

    // LST-Länge schreiben
    if(libendian::le_write_ui(length + 14, file) != 0)
        return 2;

    // Header schreiben
    if(libendian::le_write_c("MThd", 4, file) != 4)
        return 3;

    // Länge schreiben
    if(libendian::be_write_ui(length, file) != 0)
        return 4;

    // Typ schreiben
    if(libendian::be_write_us(0, file) != 0)
        return 5;

    // Tracksanzahl schreiben
    if(libendian::be_write_us(tracks, file) != 0)
        return 6;

    // PPQS schreiben
    if(libendian::be_write_us(96, file) != 0)
        return 7;

    for(unsigned short i = 0; i < tracks; ++i)
    {
        if(libendian::le_write_uc(tracklist[i].getMid(false), tracklist[i].getMidLength(false), file) != (int)tracklist[i].getMidLength(false))
            return 8;
    }

    return 0;
}
