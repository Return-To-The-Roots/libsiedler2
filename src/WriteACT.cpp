// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "Archiv.h"
#include "ArchivItem_Palette.h"
#include "ErrorCodes.h"
#include "prototypen.h"
#include <boost/nowide/fstream.hpp>

/**
 *  schreibt ein Archiv in eine ACT-File.
 *
 *  @param[in] file    Dateiname der ACT-File
 *  @param[in] items   Archiv-Struktur, von welcher gelesen wird
 *
 *  @return Null bei Erfolg, ein Wert ungleich Null bei Fehler
 */
int libsiedler2::loader::WriteACT(const boost::filesystem::path& filepath, const Archiv& items)
{
    if(filepath.empty())
        return ErrorCode::INVALID_BUFFER;

    const auto* palette = dynamic_cast<const ArchivItem_Palette*>(items[0]);
    if(!palette)
        return ErrorCode::WRONG_ARCHIV;

    // Datei zum schreiben öffnen
    boost::nowide::ofstream fs(filepath, std::ios_base::binary);

    // hat das geklappt?
    if(!fs)
        return ErrorCode::FILE_NOT_ACCESSIBLE;

    return palette->write(fs, false);
}
