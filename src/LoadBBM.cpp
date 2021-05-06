// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

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
 *  @param[in]  filepath    Dateiname der BBM-File
 *  @param[out] items   Archiv-Struktur, welche gefüllt wird
 *
 *  @return Null bei Erfolg, ein Wert ungleich Null bei Fehler
 */
int libsiedler2::loader::LoadBBM(const boost::filesystem::path& filepath, Archiv& items)
{
    MMStream mmStream;
    if(int ec = openMemoryStream(filepath, mmStream))
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
            auto palette = getAllocator().create<ArchivItem_Palette>(BobType::Palette);

            if(filepath.has_filename())
            {
                std::stringstream rName;
                rName << filepath.filename().string() << "(" << i << ")";
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
