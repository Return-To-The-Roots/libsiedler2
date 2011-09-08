// $Id: ArchivItem_Palette.h 7521 2011-09-08 20:45:55Z FloSoft $
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
#ifndef ARCHIVITEM_PALETTE_H_INCLUDED
#define ARCHIVITEM_PALETTE_H_INCLUDED

#pragma once

#include "ArchivItem.h"

namespace libsiedler2
{
	/// Index des Transparenzwertes
	const unsigned char TRANSPARENT_INDEX = 254;

	/// Klasse für Paletten.
	class ArchivItem_Palette : public ArchivItem
	{
	public:
		/// Konstruktor von @p ArchivItem_Palette.
		ArchivItem_Palette(void);

		/// Kopierkonstruktor von @p ArchivItem_Palette.
		ArchivItem_Palette(const ArchivItem_Palette *item);

		/// Konstruktor von @p ArchivItem_Palette mit Laden der Farbwerte aus einer Datei.
		ArchivItem_Palette(FILE *file, bool skip = true);

		/// liest die Farbwerte aus einer Datei.
		int load(FILE *file, bool skip = true);

		/// schreibt die Farbwerte in eine Datei.
		int write(FILE *file, bool skip = true) const;

		/// setzt einen Farbwert am entsprechenden Index.
		void set(unsigned char index, unsigned char r, unsigned char g, unsigned char b);

		/// liefert einen Farbwert am entsprechenden Index.
		void get(unsigned char index, unsigned char *r, unsigned char *g, unsigned char *b) const;

		/// liefert einen Index zum entsprechenden RGB-Wert.
		unsigned char lookup(unsigned char r, unsigned char g, unsigned char b) const;

		/// Index-Operator von @p ArchivItem_Palette. 
		const unsigned char *const operator[](int index);

		/// kopiert die Palette in einen Puffer (als RGBA)
		void copy(unsigned char *buffer) const;

	protected:
		unsigned char colors[256][3];
	};
}

#endif // !ARCHIVITEM_PALETTE_H_INCLUDED
