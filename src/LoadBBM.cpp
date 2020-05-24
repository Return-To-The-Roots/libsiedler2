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
#include "ArchivItem_Palette.h"
#include "ErrorCodes.h"
#include "IAllocator.h"
#include "OpenMemoryStream.h"
#include "fileFormatHelpers.h"
#include "libsiedler2.h"
#include "prototypen.h"
#include "libendian/EndianIStreamAdapter.h"
#include <boost/filesystem/path.hpp>
#include <sstream>

/**
 *  lädt eine BBM-File in ein Archiv.
 *
 *  @param[in]  file    Dateiname der BBM-File
 *  @param[out] items   Archiv-Struktur, welche gefüllt wird
 *
 *  @return Null bei Erfolg, ein Wert ungleich Null bei Fehler
 */
int libsiedler2::loader::LoadBBM(const std::string& file, Archiv& items)
{
    MMStream mmStream;
    if(int ec = openMemoryStream(file, mmStream))
        return ec;

    std::array<char, 4> header, pbm, chunk;
    uint32_t i = 0;

    libendian::EndianIStreamAdapter<true, MMStream&> fs(mmStream);
    // Header einlesen
    fs >> header;

    // ist es eine BBM-File? (Header "FORM")
    if(!fs || !isChunk(header, "FORM"))
        return ErrorCode::WRONG_HEADER;

    // Länge einlesen
    uint32_t length;
    fs >> length;

    // Typ einlesen
    fs >> pbm;

    // ist es eine BBM-File? (Typ "PBM ")
    if(!fs || !isChunk(pbm, "PBM "))
        return ErrorCode::WRONG_HEADER;

    // Chunks einlesen
    while(!!(fs >> chunk))
    {
        if(isChunk(chunk, "CMAP"))
        {
            fs >> length;

            // Bei ungerader Zahl aufrunden
            if(length & 1)
                ++length;

            // Ist Länge wirklich so groß wie Farbtabelle?
            if(length != 256 * 3)
                return ErrorCode::WRONG_FORMAT;

            // Daten von Item auswerten
            auto palette = getAllocator().create<ArchivItem_Palette>(BOBTYPE_PALETTE);

            boost::filesystem::path filePath(file);
            if(filePath.has_filename())
            {
                std::stringstream rName;
                rName << filePath.filename().string() << "(" << i << ")";
                palette->setName(rName.str());
            }

            if(int ec = palette->load(fs.getStream(), false))
                return ec;
            items.push(std::move(palette));

            ++i;
        } else
        {
            fs >> length;

            // Bei ungerader Zahl aufrunden
            if(length & 1)
                ++length;

            fs.ignore(length);
        }
    }

    if(items.empty() || !fs.eof())
        return ErrorCode::UNEXPECTED_EOF;

    return ErrorCode::NONE;
}
