// $Id: ArchivItem_Text.h 6581 2010-07-16 11:16:34Z FloSoft $
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
#ifndef ARCHIVITEM_TEXT_H_INCLUDED
#define ARCHIVITEM_TEXT_H_INCLUDED

#pragma once

#include "ArchivItem.h"

namespace libsiedler2
{
	/// Klasse für Texte.
	class ArchivItem_Text : public ArchivItem
	{
	public:
		/// Konstruktor von @p ArchivItem_Text.
		ArchivItem_Text(void);

		/// Destruktor von @p ArchivItem_Text.
		~ArchivItem_Text(void);

		/// Kopierkonstruktor von @p ArchivItem_Text.
		ArchivItem_Text(const ArchivItem_Text *item);

		/// Konstruktor von @p ArchivItem_Text mit Laden des Textes aus einer Datei.
		ArchivItem_Text(FILE *file, bool conversion = true, unsigned int length = 0);

		/// liest den Text aus einer Datei.
		int load(FILE *file, bool conversion = true, unsigned int length = 0);

		/// schreibt den Text in eine Datei.
		int write(FILE *file, bool conversion = true) const;

		// liefert den Text.
		const char *getText(void) const;

		// liefert die Länge.
		unsigned int getLength(void) const;

		// setzt den Text.
		void setText(const char *text, bool conversion = true, unsigned int length = 0);

		/// Konvertiert einen String von ANSI-Charset nach OEM-Charset.
		static char *Ansi2Oem(char *string, unsigned int length);

		/// Konvertiert einen String von UTF-8-Charset nach OEM-Charset.
		static char *Utf82Oem(char *string, unsigned int length);

		/// Konvertiert einen String von OEM-Charset nach ANSI-Charset.
		static char *Oem2Ansi(char *string, unsigned int length);

	private:
		char *text;          ///< Der Textspeicher.
		unsigned int length; ///< Die Länge des Textes.
	};
}

#endif // !ARCHIVITEM_TEXT_H_INCLUDED
