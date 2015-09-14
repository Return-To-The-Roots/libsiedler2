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
#include "ArchivItem_Sound_Wave.h"
#include <fstream>
#include <EndianStream.h>
#include <cstring>

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#if defined _WIN32 && defined _DEBUG && defined _MSC_VER
#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////////
/** @class libsiedler2::baseArchivItem_Sound_Wave
 *
 *  Klasse für WAVE-Sounds.
 *
 *  @author FloSoft
 */

///////////////////////////////////////////////////////////////////////////////
/**
 *  Konstruktor von @p baseArchivItem_Sound_Wave.
 *
 *  @author FloSoft
 */
libsiedler2::baseArchivItem_Sound_Wave::baseArchivItem_Sound_Wave(void) : baseArchivItem_Sound()
{
    setType(SOUNDTYPE_WAVE);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Destruktor von @p baseArchivItem_Sound_Wave.
 *
 *  @author FloSoft
 */
libsiedler2::baseArchivItem_Sound_Wave::~baseArchivItem_Sound_Wave(void)
{}

///////////////////////////////////////////////////////////////////////////////
/**
 *  lädt die Wave-Daten aus einer Datei.
 *
 *  @param[in] file    Dateihandle der Datei
 *  @param[in] length  Länge der Daten
 *
 *  @return liefert Null bei Erfolg, ungleich Null bei Fehler
 *
 *  @author FloSoft
 */
int libsiedler2::baseArchivItem_Sound_Wave::load(std::istream& file, unsigned int length)
{
    if(!file || length == 0)
        return 1;

    char header[4];
    bool prependheader = true;
    libendian::LittleEndianIStreamRef fs(file);

    // Header einlesen
    fs >> header;

    // ist es eine RIFF-File? (Header "FORM" bzw "RIFF")
    if(strncmp(header, "FORM", 4) == 0 || strncmp(header, "RIFF", 4) == 0)
        prependheader = false;

    fs.setPositionRel(-4);

    if(prependheader)
    {
        data.resize(length + 44);

        fs.read(&data[44], length);

        //unsigned char header[] = {
        //   0 | 'R', 'I', 'F', 'F',
        //   4 | 0, 0, 0, 0, // file-size
        //   8 | 'W', 'A', 'V', 'E',
        //  12 | 'f', 'm', 't', ' ',
        //  16 | 0x10, 0x00, // fmt-length
        //  18 | 0x00, 0x00, // fmt-format, unkomprimiert
        //  20 | 0x01, 0x00, // Audio-Format (PCM)
        //  22 | 0x01, 0x00, // channels, mono
        //  24 | 0x11, 0x2B, 0x00, 0x00, // 11,025 kHz
        //  28 | 0x11, 0x2B, 0x00, 0x00, // average bytes, 11025
        //  32 | 0x01, 0x00, // block align, 1
        //  34 | 0x08, 0x00, // bits per sample, 8
        //  36 | 'd', 'a', 't', 'a',
        //  40 | 0, 0, 0, 0 // data-size
        //};

        unsigned char header[44] =
        {
            'R', 'I', 'F', 'F',
            0, 0, 0, 0, // file-size
            'W', 'A', 'V', 'E',
            'f', 'm', 't', ' ',
            0x10, 0x00, // fmt-length
            0x00, 0x00, // fmt-format, unkomprimiert
            0x01, 0x00, // Audio-Format (PCM)
            0x01, 0x00, // channels, mono
            0x44, 0xac, 0x00, 0x00, // 44100 kHz
            0x44, 0xac, 0x00, 0x00, // average bytes, 44100
            0x02, 0x00, // block align, 1
            0x10, 0x00, // bits per sample, 16
            'd', 'a', 't', 'a',
            0, 0, 0, 0 // data-size
        };

        uint32_t size = length + sizeof(header) - 8;
        uint32_t flength = length;

        boost::endian::native_to_little_inplace(size);
        boost::endian::native_to_little_inplace(flength);

        memcpy(&header[4],  &size, 4);
        memcpy(&header[40], &flength,   4);

        memcpy(&data[0], header, 44);
    }
    else
    {
        data.resize(length);
        fs >> data;
    }

    return 0;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  schreibt die Wave-Daten in eine Datei.
 *
 *  @param[in] file        Dateihandle der Datei
 *  @param[in] stripheader Soll der Wave-Header entfernt werden?
 *
 *  @return liefert Null bei Erfolg, ungleich Null bei Fehler
 *
 *  @author FloSoft
 */
int libsiedler2::baseArchivItem_Sound_Wave::write(std::ostream& file, bool stripheader) const
{
    if(!file)
        return 1;

    libendian::LittleEndianOStreamRef fs(file);

    const unsigned char* start = &data.front();
    unsigned int length = data.size();
    if(stripheader)
    {
        start = &data[44];
        length -= 44;
    }

    fs << length;

    fs.write(start, length);

    return 0;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  räumt den Soundspeicher auf.
 *
 *  @author FloSoft
 */
void libsiedler2::baseArchivItem_Sound_Wave::clear(void)
{
    data.clear();
}
