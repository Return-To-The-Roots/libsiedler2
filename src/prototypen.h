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
#ifndef PROTOTYP_H_INCLUDED
#define PROTOTYP_H_INCLUDED

#pragma once

#include "enumTypes.h"
#include <string>

namespace libsiedler2
{
    //Fwd decl
    class ArchivItem_Palette;
    class ArchivItem;
    class ArchivInfo;

	/// Die verschiedenen Lade-/Schreibfunktionen der Dateien
	namespace loader
	{
		/// lädt eine spezifizierten Bobtype aus einer Datei in ein ArchivItem.
		int LoadType(BobType bobtype, std::istream&lst, const ArchivItem_Palette* palette, ArchivItem *&item);

		/// schreibt eine spezifizierten Bobtype aus einem ArchivItem in eine Datei.
		int WriteType(BobType bobtype, std::ostream&lst, const ArchivItem_Palette* palette, const ArchivItem& item);

		/// lädt eine LST-File in ein ArchivInfo.
		int LoadLST(const std::string& file, const ArchivItem_Palette* palette, ArchivInfo& items);

		/// schreibt ein ArchivInfo eine LST-File.
		int WriteLST(const std::string& file, const ArchivItem_Palette* palette, const ArchivInfo& items);

		/// lädt eine BBM-File in ein ArchivInfo.
		int LoadBBM(const std::string& file, ArchivInfo& items);

		/// schreibt ein ArchivInfo in eine BBM-File.
		int WriteBBM(const std::string& file, const ArchivInfo& items);

		/// lädt eine ACT-File in ein ArchivInfo.
		int LoadACT(const std::string& file, ArchivInfo& items);

		/// schreibt ein ArchivInfo in eine ACT-File.
		int WriteACT(const std::string& file, const ArchivInfo& items);

		/// lädt eine DAT/IDX-File in ein ArchivInfo.
		int LoadDATIDX(const std::string& file, const ArchivItem_Palette *palette, ArchivInfo& items);

		/// schreibt ein ArchivInfo in eine DAT/IDX-File.
		int WriteDATIDX(const std::string& file, const ArchivItem_Palette *palette, const ArchivInfo& items);

		/// lädt eine BMP-File in ein ArchivInfo.
		int LoadBMP(const std::string& file, ArchivInfo& image);

		/// schreibt ein ArchivInfo in eine BMP-File.
		int WriteBMP(const std::string& file, const ArchivItem_Palette *palette, const ArchivInfo& items);

		/// lädt eine GER/ENG-File in ein ArchivInfo.
		int LoadTXT(const std::string& file, ArchivInfo& items, bool conversion = true);

#		define LoadGER LoadTXT
#		define LoadENG LoadTXT

		/// schreibt eine GER/ENG-File aus einem ArchivInfo.
		int WriteTXT(const std::string& file, const ArchivInfo& items, bool conversion = true);

#		define WriteGER WriteTXT
#		define WriteENG WriteTXT

		/// lädt eine LBM-File in ein ArchivInfo.
		int LoadLBM(const std::string& file, ArchivInfo& items);

		/// schreibt ein ArchivInfo in eine LBM-File.
		int WriteLBM(const std::string& file, const ArchivInfo& items);

		/// lädt eine SWD/WSD-File in ein ArchivInfo.
		int LoadMAP(const std::string& file, ArchivInfo& items, bool only_header = false);

#		define LoadSWD LoadMAP
#		define LoadWSD LoadMAP

		/// schreibt ein ArchivInfo in eine SWD/WSD-File.
		int WriteMAP(const std::string& file, const ArchivInfo& items);

#		define WriteSWD WriteMAP
#		define WriteWSD WriteMAP

		/// lädt eine BOB-File in ein ArchivInfo.
		int LoadBOB(const std::string& file, const ArchivItem_Palette* palette, ArchivInfo& items);

		/// schreibt ein ArchivInfo in eine BOB-File.
		int WriteBOB(const std::string& file, const ArchivItem_Palette* palette, const ArchivInfo& items);

		int LoadSND(const std::string& file, ArchivInfo& items);

#		define LoadMID LoadSND
#		define LoadXMID LoadSND
#		define LoadWAV LoadSND

		int WriteSND(const std::string& file, const ArchivInfo& items);

#		define WriteWAV WriteSND

		/// lädt eine INI-File in ein ArchivInfo.
		int LoadINI(const std::string& file, ArchivInfo& items);
		int WriteINI(const std::string& file, const ArchivInfo& items);
	}
}

#endif // !PROTOTYP_H_INCLUDED
