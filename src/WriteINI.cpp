// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "Archiv.h"
#include "ArchivItem_Ini.h"
#include "ErrorCodes.h"
#include "prototypen.h"
#include <boost/nowide/fstream.hpp>

/**
 *  schreibt ein Archiv in eine INI-File.
 *
 *  @param[in]  filepath    Dateiname der INI-File
 *  @param[in] items    Archiv-Struktur, welche gefüllt wird
 *
 *  @return Null bei Erfolg, ein Wert ungleich Null bei Fehler
 */
int libsiedler2::loader::WriteINI(const boost::filesystem::path& filepath, const Archiv& items)
{
    if(filepath.empty())
        return ErrorCode::INVALID_BUFFER;

    // Datei zum schreiben öffnen
    boost::nowide::ofstream fs(filepath, std::ios_base::binary);
    if(!fs)
        return ErrorCode::FILE_NOT_ACCESSIBLE;

    bool first = true;
    for(size_t i = 0; i < items.size(); ++i)
    {
        const auto* item = dynamic_cast<const ArchivItem_Ini*>(items.get(i));
        if(!item)
            return ErrorCode::WRONG_ARCHIV;
        if(!first)
            fs << "\r\n";
        else
            first = false;
        if(int ec = item->write(fs))
            return ec;
    }

    return ErrorCode::NONE;
}
