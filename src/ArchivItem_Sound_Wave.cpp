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
#include "ArchivItem_Sound_Wave.h"
#include "ErrorCodes.h"
#include "WAV_Header.h"
#include "fileFormatHelpers.h"
#include "libendian/EndianIStreamAdapter.h"
#include "libendian/EndianOStreamAdapter.h"
#include <iostream>

namespace libsiedler2 {
/** @class baseArchivItem_Sound_Wave
 *
 *  Klasse für WAVE-Sounds.
 */

ArchivItem_Sound_Wave::ArchivItem_Sound_Wave() : ArchivItem_Sound(SOUNDTYPE_WAVE) {}

/**
 *  lädt die Wave-Daten aus einer Datei.
 *
 *  @param[in] file    Dateihandle der Datei
 *  @param[in] length  Länge der Daten
 *
 *  @return liefert Null bei Erfolg, ungleich Null bei Fehler
 */
int ArchivItem_Sound_Wave::load(std::istream& file, uint32_t length)
{
    if(!file)
        return ErrorCode::FILE_NOT_ACCESSIBLE;

    libendian::EndianIStreamAdapter<false, std::istream&> fs(file);

    bool hasHeader = false;
    if(length >= sizeof(header))
    {
        char headerId[4];
        if(!(fs >> headerId))
            return ErrorCode::UNEXPECTED_EOF;
        fs.setPositionRel(-4);
        if(isChunk(headerId, "FORM") || isChunk(headerId, "RIFF"))
            hasHeader = true;
    }

    // ist es eine RIFF-File? (Header "FORM" bzw "RIFF")
    if(hasHeader)
    {
        if(!fs.readRaw(&header, 1))
            return ErrorCode::WRONG_HEADER;
        length -= sizeof(header);
    } else
    {
        setChunkId(header.RIFF_ID, "RIFF");
        header.fileSize = length + sizeof(header);
        setChunkId(header.WAVE_ID, "WAVE");
        setChunkId(header.fmt_ID, "fmt ");
        // Hard coded guesses for the format
        header.fmtSize = 16;
        header.fmtTag = 1;
        header.numChannels = 1;
        header.samplesPerSec = 11025;
        header.bytesPerSec = 11025;
        header.frameSize = 1;
        header.bitsPerSample = 8;
        setChunkId(header.data_ID, "data");
        header.dataSize = length;
    }

    data.resize(length);
    fs >> data;

    return (!file) ? ErrorCode::UNEXPECTED_EOF : ErrorCode::NONE;
}

/**
 *  schreibt die Wave-Daten in eine Datei.
 *
 *  @param[in] file        Dateihandle der Datei
 *  @param[in] stripheader Soll der Wave-Header entfernt werden?
 *
 *  @return liefert Null bei Erfolg, ungleich Null bei Fehler
 */
int ArchivItem_Sound_Wave::write(std::ostream& file, bool stripheader) const
{
    if(!file)
        return ErrorCode::FILE_NOT_ACCESSIBLE;

    libendian::EndianOStreamAdapter<false, std::ostream&> fs(file);

    if(!stripheader)
        fs.writeRaw(&header, 1);

    fs << data;

    return (!file) ? ErrorCode::UNEXPECTED_EOF : ErrorCode::NONE;
}

/**
 *  räumt den Soundspeicher auf.
 */
void ArchivItem_Sound_Wave::clear()
{
    data.clear();
}
} // namespace libsiedler2
