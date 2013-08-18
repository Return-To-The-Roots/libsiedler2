// $Id: WriteINI.cpp 8853 2013-08-18 19:23:49Z marcus $
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

///////////////////////////////////////////////////////////////////////////////
// Header
#include "main.h"

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#if defined _WIN32 && defined _DEBUG && defined _MSC_VER
	#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////////
/** 
 *  schreibt ein ArchivInfo in eine INI-File.
 *
 *  @param[in]  file    Dateiname der INI-File
 *  @param[in] items    ArchivInfo-Struktur, welche gefüllt wird
 *
 *  @return Null bei Erfolg, ein Wert ungleich Null bei Fehler
 *
 *  @author FloSoft
 */
int libsiedler2::loader::WriteINI(const char *file, const ArchivInfo *items)
{
	if(file == NULL || items == NULL)
		return 1;

	// Datei zum schreiben öffnen
	FILE *ini = fopen(file, "wb");
	if (ini == 0)
		return 2;

	for(unsigned long i = 0; i < items->getCount(); ++i)
	{
		const ArchivItem_Ini *item = dynamic_cast<const ArchivItem_Ini *>(items->get(i));

		if(item)
			item->write(ini);
	}

	// Datei schliessen
	fclose(ini);

	// alles ok
	return 0;
}
