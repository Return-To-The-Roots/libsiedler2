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

#pragma once

#include "enumTypes.h"
#include <boost/filesystem/path.hpp>
#include <memory>

namespace libsiedler2 {
// Fwd decl
class ArchivItem_Palette;
class ArchivItem;
class Archiv;

/// Die verschiedenen Lade-/Schreibfunktionen der Dateien
namespace loader {
    /// lädt eine spezifizierten Bobtype aus einer Datei in ein ArchivItem.
    int LoadType(BobType bobtype, std::istream& lst, std::unique_ptr<ArchivItem>& item,
                 const ArchivItem_Palette* palette = nullptr);

    /// schreibt eine spezifizierten Bobtype aus einem ArchivItem in eine Datei.
    int WriteType(BobType bobtype, std::ostream& lst, const ArchivItem& item,
                  const ArchivItem_Palette* palette = nullptr);

    /// lädt eine LST-File in ein Archiv.
    int LoadLST(const boost::filesystem::path& filepath, Archiv& items, const ArchivItem_Palette* palette = nullptr);

    /// schreibt ein Archiv eine LST-File.
    int WriteLST(const boost::filesystem::path& filepath, const Archiv& items,
                 const ArchivItem_Palette* palette = nullptr);

    /// lädt eine BBM-File in ein Archiv.
    int LoadBBM(const boost::filesystem::path& filepath, Archiv& items);

    /// schreibt ein Archiv in eine BBM-File.
    int WriteBBM(const boost::filesystem::path& filepath, const Archiv& items);

    /// lädt eine ACT-File in ein Archiv.
    int LoadACT(const boost::filesystem::path& filepath, Archiv& items);

    /// schreibt ein Archiv in eine ACT-File.
    int WriteACT(const boost::filesystem::path& filepath, const Archiv& items);

    /// lädt eine DAT/IDX-File in ein Archiv.
    int LoadDATIDX(const boost::filesystem::path& filepath, Archiv& items, const ArchivItem_Palette* palette = nullptr);

    /// lädt eine BMP-File in ein Archiv.
    int LoadBMP(const boost::filesystem::path& filepath, Archiv& image, const ArchivItem_Palette* palette = nullptr);

    /// schreibt ein Archiv in eine BMP-File.
    int WriteBMP(const boost::filesystem::path& filepath, const Archiv& items,
                 const ArchivItem_Palette* palette = nullptr);

    /// load a GER/ENG-File into the archive.
    /// If conversion is true then OEM conversion and @@-replacement is done, otherwise line endings are normalized to
    /// \n
    int LoadTXT(const boost::filesystem::path& filepath, Archiv& items, bool conversion);

#define LoadGER LoadTXT
#define LoadENG LoadTXT

    /// write a GER/ENG-File from an archive
    /// If conversion is true then OEM conversion and @@-replacement is done
    int WriteTXT(const boost::filesystem::path& filepath, const Archiv& items, bool conversion);

#define WriteGER WriteTXT
#define WriteENG WriteTXT

    /// lädt eine LBM-File in ein Archiv.
    int LoadLBM(const boost::filesystem::path& filepath, Archiv& items);

    /// schreibt ein Archiv in eine LBM-File.
    int WriteLBM(const boost::filesystem::path& filepath, const Archiv& items,
                 const ArchivItem_Palette* palette = nullptr);

    /// lädt eine SWD/WSD-File in ein Archiv.
    int LoadMAP(const boost::filesystem::path& filepath, Archiv& items, bool only_header = false);

#define LoadSWD LoadMAP
#define LoadWSD LoadMAP

    /// schreibt ein Archiv in eine SWD/WSD-File.
    int WriteMAP(const boost::filesystem::path& filepath, const Archiv& items);

#define WriteSWD WriteMAP
#define WriteWSD WriteMAP

    /// lädt eine BOB-File in ein Archiv.
    int LoadBOB(const boost::filesystem::path& filepath, Archiv& items, const ArchivItem_Palette* palette);

    int LoadSND(const boost::filesystem::path& filepath, Archiv& items);

#define LoadMID LoadSND
#define LoadXMID LoadSND
#define LoadWAV LoadSND

    int WriteSND(const boost::filesystem::path& filepath, const Archiv& items);

#define WriteWAV WriteSND

    /// lädt eine INI-File in ein Archiv.
    int LoadINI(const boost::filesystem::path& filepath, Archiv& items);
    int WriteINI(const boost::filesystem::path& filepath, const Archiv& items);

    int LoadTxtPalette(const boost::filesystem::path& filepath, Archiv& items);
    int WriteTxtPalette(const boost::filesystem::path& filepath, const ArchivItem_Palette& palette);
    int LoadPaletteAnim(const boost::filesystem::path& filepath, Archiv& items);
    int WritePaletteAnim(const boost::filesystem::path& filepath, const Archiv& items);

} // namespace loader
} // namespace libsiedler2
