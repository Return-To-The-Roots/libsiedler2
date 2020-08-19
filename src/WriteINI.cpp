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
