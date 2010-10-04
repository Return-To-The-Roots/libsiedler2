// $Id: ArchivItem_Bob.h 6581 2010-07-16 11:16:34Z FloSoft $
//
// Copyright (c) 2005 - 2010 Settlers Freaks (sf-team at siedler25.org)
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
#ifndef ARCHIVITEM_BOB_H_INCLUDED
#define ARCHIVITEM_BOB_H_INCLUDED

#pragma once

#include "ArchivItem.h"
#include "ArchivItem_Palette.h"

namespace libsiedler2
{
	/// Klasse für Bobfiles.
	class ArchivItem_Bob : public ArchivItem, public ArchivInfo
	{
	public:
		/// Konstruktor von @p ArchivItem_Bob.
		ArchivItem_Bob(void);

		/// Kopierkonstruktor von @p ArchivItem_Bob.
		ArchivItem_Bob(const ArchivItem_Bob *item);

		/// Konstruktor von @p ArchivItem_Bob mit Laden der Bob-Daten.
		ArchivItem_Bob(FILE *file, const ArchivItem_Palette *palette);

		/// Destruktor von @p ArchivItem_Bob.
		~ArchivItem_Bob(void);

		/// lädt die Bobdaten aus einer Datei.
		int load(FILE *file, const ArchivItem_Palette *palette);

		/// schreibt die Bobdaten in eine Datei.
		int write(FILE *file, const ArchivItem_Palette *palette) const;

	protected:
		unsigned short good_count; /// Warenanzahl
		unsigned short item_count; /// Bildanzahl
		unsigned short *links;     /// "Links" (Zugehörigkeiten der Bilder)
	};
}

#endif // !ARCHIVITEM_BOB_H_INCLUDED
