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
#ifndef PROTOTYP_H_INCLUDED
#define PROTOTYP_H_INCLUDED

#pragma once

#include "enumTypes.h"
#include <string>

namespace libsiedler2 {
// Fwd decl
class ArchivItem_Palette;
class ArchivItem;
class Archiv;

/// Die verschiedenen Lade-/Schreibfunktionen der Dateien
namespace loader {
    /// lädt eine spezifizierten Bobtype aus einer Datei in ein ArchivItem.
    int LoadType(BobType bobtype, std::istream& lst, ArchivItem*& item, const ArchivItem_Palette* palette = NULL);

    /// schreibt eine spezifizierten Bobtype aus einem ArchivItem in eine Datei.
    int WriteType(BobType bobtype, std::ostream& lst, const ArchivItem& item, const ArchivItem_Palette* palette = NULL);

    /// lädt eine LST-File in ein Archiv.
    int LoadLST(const std::string& file, Archiv& items, const ArchivItem_Palette* palette = NULL);

    /// schreibt ein Archiv eine LST-File.
    int WriteLST(const std::string& file, const Archiv& items, const ArchivItem_Palette* palette = NULL);

    /// lädt eine BBM-File in ein Archiv.
    int LoadBBM(const std::string& file, Archiv& items);

    /// schreibt ein Archiv in eine BBM-File.
    int WriteBBM(const std::string& file, const Archiv& items);

    /// lädt eine ACT-File in ein Archiv.
    int LoadACT(const std::string& file, Archiv& items);

    /// schreibt ein Archiv in eine ACT-File.
    int WriteACT(const std::string& file, const Archiv& items);

    /// lädt eine DAT/IDX-File in ein Archiv.
    int LoadDATIDX(const std::string& file, Archiv& items, const ArchivItem_Palette* palette = NULL);

    /// lädt eine BMP-File in ein Archiv.
    int LoadBMP(const std::string& file, Archiv& image, const ArchivItem_Palette* palette = NULL);

    /// schreibt ein Archiv in eine BMP-File.
    int WriteBMP(const std::string& file, const Archiv& items, const ArchivItem_Palette* palette = NULL);

    /// lädt eine GER/ENG-File in ein Archiv.
    int LoadTXT(const std::string& file, Archiv& items, bool conversion = true);

#define LoadGER LoadTXT
#define LoadENG LoadTXT

    /// schreibt eine GER/ENG-File aus einem Archiv.
    int WriteTXT(const std::string& file, const Archiv& items, bool conversion = true);

#define WriteGER WriteTXT
#define WriteENG WriteTXT

    /// lädt eine LBM-File in ein Archiv.
    int LoadLBM(const std::string& file, Archiv& items);

    /// schreibt ein Archiv in eine LBM-File.
    int WriteLBM(const std::string& file, const Archiv& items, const ArchivItem_Palette* palette = NULL);

    /// lädt eine SWD/WSD-File in ein Archiv.
    int LoadMAP(const std::string& file, Archiv& items, bool only_header = false);

#define LoadSWD LoadMAP
#define LoadWSD LoadMAP

    /// schreibt ein Archiv in eine SWD/WSD-File.
    int WriteMAP(const std::string& file, const Archiv& items);

#define WriteSWD WriteMAP
#define WriteWSD WriteMAP

    /// lädt eine BOB-File in ein Archiv.
    int LoadBOB(const std::string& file, Archiv& items, const ArchivItem_Palette* palette);

    int LoadSND(const std::string& file, Archiv& items);

#define LoadMID LoadSND
#define LoadXMID LoadSND
#define LoadWAV LoadSND

    int WriteSND(const std::string& file, const Archiv& items);

#define WriteWAV WriteSND

    /// lädt eine INI-File in ein Archiv.
    int LoadINI(const std::string& file, Archiv& items);
    int WriteINI(const std::string& file, const Archiv& items);

    int LoadPaletteAnim(const std::string& file, Archiv& items);
    int WritePaletteAnim(const std::string& file, const Archiv& items);

} // namespace loader
} // namespace libsiedler2

#endif // !PROTOTYP_H_INCLUDED
