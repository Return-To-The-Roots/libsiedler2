// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

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
    int loadType(BobType bobtype, std::istream& lst, std::unique_ptr<ArchivItem>& item,
                 const ArchivItem_Palette* palette = nullptr);

    /// schreibt eine spezifizierten Bobtype aus einem ArchivItem in eine Datei.
    int writeType(BobType bobtype, std::ostream& lst, const ArchivItem& item,
                  const ArchivItem_Palette* palette = nullptr);

    /// lädt eine LST-File in ein Archiv.
    int loadLST(const boost::filesystem::path& filepath, Archiv& items, const ArchivItem_Palette* palette = nullptr);

    /// schreibt ein Archiv eine LST-File.
    int writeLST(const boost::filesystem::path& filepath, const Archiv& items,
                 const ArchivItem_Palette* palette = nullptr);

    /// lädt eine BBM-File in ein Archiv.
    int loadBBM(const boost::filesystem::path& filepath, Archiv& items);

    /// schreibt ein Archiv in eine BBM-File.
    int writeBBM(const boost::filesystem::path& filepath, const Archiv& items);

    /// lädt eine ACT-File in ein Archiv.
    int loadACT(const boost::filesystem::path& filepath, Archiv& items);

    /// schreibt ein Archiv in eine ACT-File.
    int writeACT(const boost::filesystem::path& filepath, const Archiv& items);

    /// lädt eine DAT/IDX-File in ein Archiv.
    int loadDATIDX(const boost::filesystem::path& filepath, Archiv& items, const ArchivItem_Palette* palette = nullptr);

    /// lädt eine BMP-File in ein Archiv.
    int loadBMP(const boost::filesystem::path& filepath, Archiv& image, const ArchivItem_Palette* palette = nullptr);

    /// schreibt ein Archiv in eine BMP-File.
    int writeBMP(const boost::filesystem::path& filepath, const Archiv& items,
                 const ArchivItem_Palette* palette = nullptr);

    /// load a GER/ENG-File into the archive.
    /// If conversion is true then OEM conversion and @@-replacement is done, otherwise line endings are normalized to
    /// \n
    int loadTXT(const boost::filesystem::path& filepath, Archiv& items, bool conversion);

#define loadGER loadTXT
#define loadENG loadTXT

    /// write a GER/ENG-File from an archive
    /// If conversion is true then OEM conversion and @@-replacement is done
    int writeTXT(const boost::filesystem::path& filepath, const Archiv& items, bool conversion);

#define writeGER writeTXT
#define writeENG writeTXT

    /// lädt eine LBM-File in ein Archiv.
    int loadLBM(const boost::filesystem::path& filepath, Archiv& items);

    /// schreibt ein Archiv in eine LBM-File.
    int writeLBM(const boost::filesystem::path& filepath, const Archiv& items,
                 const ArchivItem_Palette* palette = nullptr);

    /// lädt eine SWD/WSD-File in ein Archiv.
    int loadMAP(const boost::filesystem::path& filepath, Archiv& items, bool only_header = false);

#define loadSWD loadMAP
#define loadWSD loadMAP

    /// schreibt ein Archiv in eine SWD/WSD-File.
    int writeMAP(const boost::filesystem::path& filepath, const Archiv& items);

#define writeSWD writeMAP
#define writeWSD writeMAP

    /// lädt eine BOB-File in ein Archiv.
    int loadBOB(const boost::filesystem::path& filepath, Archiv& items, const ArchivItem_Palette* palette);

    int loadSND(const boost::filesystem::path& filepath, Archiv& items);

#define loadMID loadSND
#define loadXMID loadSND
#define loadWAV loadSND

    int writeSND(const boost::filesystem::path& filepath, const Archiv& items);

#define writeWAV writeSND

    /// lädt eine INI-File in ein Archiv.
    int loadINI(const boost::filesystem::path& filepath, Archiv& items);
    int writeINI(const boost::filesystem::path& filepath, const Archiv& items);

    int loadTxtPalette(const boost::filesystem::path& filepath, Archiv& items);
    int writeTxtPalette(const boost::filesystem::path& filepath, const ArchivItem_Palette& palette);
    int loadPaletteAnim(const boost::filesystem::path& filepath, Archiv& items);
    int writePaletteAnim(const boost::filesystem::path& filepath, const Archiv& items);

} // namespace loader
} // namespace libsiedler2
