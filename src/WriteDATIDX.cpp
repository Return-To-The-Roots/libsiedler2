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

#include "main.h" // IWYU pragma: keep
#include "prototypen.h"

/**
 *  schreibt ein ArchivInfo in eine DAT/IDX-File.
 *
 *  @param[in] file    Dateiname der DAT/IDX-File
 *  @param[in] palette Farbpalette die zum Schreiben benutzt wird.
 *  @param[in] items   ArchivInfo-Struktur, von welcher gelesen wird
 *
 *  @return Null bei Erfolg, ein Wert ungleich Null bei Fehler
 */
int libsiedler2::loader::WriteDATIDX(const std::string& file, const ArchivItem_Palette* palette, const ArchivInfo&  /*items*/)
{
    if(file.empty() || palette == NULL)
        return 1;

    /// @todo DAT und IDX schreiben.

    return 254;
}
