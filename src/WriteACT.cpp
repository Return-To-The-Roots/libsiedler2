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
int libsiedler2::loader::WriteACT(const std::string& file, const Archiv& items)
{
    if(file.empty())
        return ErrorCode::INVALID_BUFFER;

    const auto* palette = dynamic_cast<const ArchivItem_Palette*>(items[0]);
    if(!palette)
        return ErrorCode::WRONG_ARCHIV;

    // Datei zum schreiben öffnen
    bnw::ofstream fs(file, std::ios_base::binary);

    // hat das geklappt?
    if(!fs)
        return ErrorCode::FILE_NOT_ACCESSIBLE;

    return palette->write(fs, false);
}
