// $Id: ArchivItem_Sound_Other.h 7521 2011-09-08 20:45:55Z FloSoft $
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
#ifndef ARCHIVITEM_SOUND_OTHER_H_INCLUDED
#define ARCHIVITEM_SOUND_OTHER_H_INCLUDED

#pragma once

#include "ArchivItem_Sound.h"

namespace libsiedler2
{
	/// Basisklasse für Other-Sounds.
	class baseArchivItem_Sound_Other : public virtual baseArchivItem_Sound
	{
	public:
		/// Konstruktor von @p baseArchivItem_Sound_Other.
		baseArchivItem_Sound_Other(void);

		/// Kopierkonstruktor von @p baseArchivItem_Sound_Other.
		baseArchivItem_Sound_Other(const baseArchivItem_Sound_Other *item);

		/// Destruktor von @p baseArchivItem_Sound_Other.
		virtual ~baseArchivItem_Sound_Other(void);

		/// lädt die Daten aus einer Datei.
		int load(FILE *file, unsigned int length);

		/// schreibt die Daten in eine Datei.
		int write(FILE *file) const;

		/// alloziert Soundspeicher für die gewünschte Größe.
		void alloc(unsigned int length);

		/// räumt den Soundspeicher auf.
		void clear(void);

		const unsigned char *getData() { return data; }
		unsigned int getLength() { return length; }

	protected:
		unsigned char *data;
		unsigned int length;
	};

	/// Klasse für Other-Sounds.
	class ArchivItem_Sound_Other : public virtual baseArchivItem_Sound_Other, public ArchivItem_Sound
	{
	public:
		/// Konstruktor von @p ArchivItem_Sound_Other.
		ArchivItem_Sound_Other(void) : baseArchivItem_Sound_Other() {}

		/// Kopierkonstruktor von @p ArchivItem_Sound_Other.
		ArchivItem_Sound_Other(const ArchivItem_Sound_Other *item) : baseArchivItem_Sound_Other(item) {}
	};
}

#endif // !ARCHIVITEM_SOUND_OTHER_H_INCLUDED
