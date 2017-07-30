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
#include "ArchivItem_Sound_Wave.h"
#include "WAV_Header.h"
#include "libendian/src/EndianIStreamAdapter.h"
#include "libendian/src/EndianOStreamAdapter.h"
#include <iostream>
#include <cstring>

/** @class libsiedler2::baseArchivItem_Sound_Wave
 *
 *  Klasse für WAVE-Sounds.
 */

libsiedler2::baseArchivItem_Sound_Wave::baseArchivItem_Sound_Wave() : baseArchivItem_Sound()
{
    setType(SOUNDTYPE_WAVE);
}

libsiedler2::baseArchivItem_Sound_Wave::~baseArchivItem_Sound_Wave()
{}

/**
 *  lädt die Wave-Daten aus einer Datei.
 *
 *  @param[in] file    Dateihandle der Datei
 *  @param[in] length  Länge der Daten
 *
 *  @return liefert Null bei Erfolg, ungleich Null bei Fehler
 */
int libsiedler2::baseArchivItem_Sound_Wave::load(std::istream& file, uint32_t length)
{
    if(!file || length == 0)
        return 1;

    libendian::EndianIStreamAdapter<false, std::istream&> fs(file);

    // Header einlesen
    char headerId[4];
    fs >> headerId;
    fs.setPositionRel(-4);

    // ist es eine RIFF-File? (Header "FORM" bzw "RIFF")
    bool hasHeader = false;
    if(strncmp(headerId, "FORM", 4) == 0 || strncmp(headerId, "RIFF", 4) == 0)
    {
        if(!fs.readRaw(&header, 1))
            return 2;
        length -= sizeof(header);
    } else
    {
        strncpy(header.RIFF_ID, "RIFF", 4);
        strncpy(header.WAVE_ID, "WAVE", 4);
        strncpy(header.fmt_ID, "fmt ", 4);
        strncpy(header.data_ID, "data", 4);
        header.fmtTag = 1;
        header.numChannels = 1;
        header.samplesPerSec = 11025;
        header.bytesPerSec = 11025;
        header.frameSize = 1;
        header.bitsPerSample = 8;
        header.dataSize = length;
        header.fmtSize = 28;
        header.fileSize = length + sizeof(header);
    }

    data.resize(length);
    fs >> data;

    return (!fs) ? 99 : 0;
}

/**
 *  schreibt die Wave-Daten in eine Datei.
 *
 *  @param[in] file        Dateihandle der Datei
 *  @param[in] stripheader Soll der Wave-Header entfernt werden?
 *
 *  @return liefert Null bei Erfolg, ungleich Null bei Fehler
 */
int libsiedler2::baseArchivItem_Sound_Wave::write(std::ostream& file, bool stripheader) const
{
    if(!file)
        return 1;

    libendian::EndianOStreamAdapter<false, std::ostream&> fs(file);

    if(!stripheader)
        fs.writeRaw(&header, 1);

    fs << data;

    return (!fs) ? 99 : 0;
}

/**
 *  räumt den Soundspeicher auf.
 */
void libsiedler2::baseArchivItem_Sound_Wave::clear()
{
    data.clear();
}
