// $Id: ArchivItem_Raw.h 7521 2011-09-08 20:45:55Z FloSoft $
//
// Copyright (c) 2005 - 2011 Settlers Freaks (sf-team at siedler25.org)
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
#ifndef ARCHIVITEM_RAW_H_INCLUDED
#define ARCHIVITEM_RAW_H_INCLUDED

#pragma once

#include "ArchivItem.h"

namespace libsiedler2
{
	/// Basisklasse für Rawdaten.
	class baseArchivItem_Raw : public ArchivItem
	{
	public:
		/// Konstruktor von @p baseArchivItem_Raw.
		baseArchivItem_Raw(void);
		/// Kopierkonstruktor von @p baseArchivItem_Raw.
		baseArchivItem_Raw(const baseArchivItem_Raw *item);
		/// Destruktor von @p baseArchivItem_Raw.
		~baseArchivItem_Raw(void);

		/// lädt die Rawdaten aus einer Datei.
		int load(FILE *file, unsigned int length = 0xFFFFFFFF);
		/// schreibt die Rawdaten in eine Datei.
		int write(FILE *file, bool with_length) const;

		/// liefert die Länge des Datenblocks.
		unsigned int getLength(void) const;

		/// liefert die Daten zurück (ro).
		const unsigned char *getData(void) const;
		/// liefert die Daten zurück (rw).
		unsigned char *getData(void);

		/// erzeugt den Datenblock.
		unsigned char *alloc(unsigned int length);
		/// löscht den Datenblock.
		void clear(void);

	private:
		unsigned char *data; ///< Die Daten.
		unsigned int length; ///< Die Länge der Daten.
	};

	/// Klasse für Rawdaten.
	class ArchivItem_Raw : public virtual baseArchivItem_Raw
	{
	public:
		/// Konstruktor von @p baseArchivItem_Raw.
		ArchivItem_Raw(void) : baseArchivItem_Raw() {}
		/// Kopierkonstruktor von @p baseArchivItem_Raw.
		ArchivItem_Raw(const ArchivItem_Raw *item) : baseArchivItem_Raw(item) {}
	};
}

#endif // !ARCHIVITEM_RAW_H_INCLUDED
