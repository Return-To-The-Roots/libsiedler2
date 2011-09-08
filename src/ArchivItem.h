// $Id: ArchivItem.h 7521 2011-09-08 20:45:55Z FloSoft $
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
#ifndef ARCHIVITEM_H_INCLUDED
#define ARCHIVITEM_H_INCLUDED

#pragma once

namespace libsiedler2
{
	/// Basisklasse der Archivelemente.
	class ArchivItem
	{
	public:
		/// Konstruktor von @p ArchivItem.
		ArchivItem();

		/// Kopierkonstruktor von @p ArchivItem.
		ArchivItem(const ArchivItem *item);

		/// virtueller Destruktor von @p ArchivItem.
		virtual ~ArchivItem();

		/// setzt den Bobtype des Items.
		inline void setBobType(unsigned short bobtype)
		{
			this->bobtype = bobtype;
		}

		/// liefert den Bobtype des Items.
		inline unsigned short getBobType(void) const
		{
			return bobtype;
		}

		/// setzt den Namen des Items.
		inline void setName(const char *name)
		{
			strncpy(this->name, name, 63);
		}

		/// liefert den Namen des Items.
		inline const char *getName(void) const
		{
			return name;
		}

		/// lädt die Daten aus einer Datei.
		virtual int load(FILE *file);

		/// schreibt die Daten in eine Datei.
		virtual int write(FILE *file) const;

	protected:
		unsigned short bobtype; ///< Bobtype des Elements.
		char name[64];          ///< Name des Elements.
	};
}

#endif // !ARCHIVITEM_H_INCLUDED
