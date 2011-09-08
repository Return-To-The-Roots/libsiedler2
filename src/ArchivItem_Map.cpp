// $Id: ArchivItem_Map.cpp 7521 2011-09-08 20:45:55Z FloSoft $
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
#include "ArchivItem_Map.h"

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#if defined _WIN32 && defined _DEBUG && defined _MSC_VER
	#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////////
/** @class libsiedler2::ArchivItem_Map
 *
 *  Klasse für eine Mapfile.
 *
 *  @author FloSoft
 */

///////////////////////////////////////////////////////////////////////////////
/**
 *  Konstruktor von @p ArchivItem_Map.
 *
 *  @author FloSoft
 */
libsiedler2::ArchivItem_Map::ArchivItem_Map(void) : ArchivItem(), ArchivInfo()
{
	setBobType(BOBTYPE_MAP);

	alloc(16);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Kopierkonstruktor von @p ArchivItem_Map.
 *
 *  @param[in] item Quellitem
 *
 *  @author FloSoft
 */
libsiedler2::ArchivItem_Map::ArchivItem_Map(const ArchivItem_Map *item) : ArchivItem(item), ArchivInfo(item)
{
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Destruktor von @p ArchivItem_Map.
 *
 *  @author FloSoft
 */
libsiedler2::ArchivItem_Map::~ArchivItem_Map(void)
{
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  lädt die Mapdaten aus einer Datei.
 *
 *  @param[in] file Dateihandle der Datei
 *  @param[in] only_header Soll nur der Header gelesen werden?
 *
 *	@return liefert Null bei Erfolg, ungleich Null bei Fehler
 *
 *  @author FloSoft
 */
int libsiedler2::ArchivItem_Map::load(FILE *file, bool only_header)
{
	return loadHelper(file, only_header);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  lädt die Mapdaten aus einer Datei.
 *
 *  @param[in] file        Dateihandle der Datei
 *  @param[in] only_header Soll nur der Header gelesen werden?
 *
 *	@return liefert Null bei Erfolg, ungleich Null bei Fehler
 *
 *  @author FloSoft
 */
int libsiedler2::ArchivItem_Map::loadHelper(FILE *file, bool only_header )
{
	if(file == NULL)
		return 1;

	ArchivItem_Map_Header *header = dynamic_cast<ArchivItem_Map_Header*>((*allocator)(BOBTYPE_MAP_HEADER, 0, NULL));

	if(header->load(file) != 0)
		return 2;

	set(0, header);

	// nur der Header?
	if(only_header)
		return 0;

	// unbekannte Daten einlesen
	ArchivItem_Raw *layer = dynamic_cast<ArchivItem_Raw*>((*allocator)(BOBTYPE_RAW, 0, NULL));
	if(layer->load(file, 2296) != 0)
		return 3;
	set(1, layer);

	unsigned short width, height;
	for(unsigned int i = 0; i < 14; ++i)
	{
		// 6 Unbekannte Daten überspringen
		fseek(file, 6, SEEK_CUR);

		// ggf paar bytes weiterspringen
		int seek = -4;
		do {
			seek += 2;
			if(libendian::le_read_us(&width, file) != 0)
				return 4;
		} while(width == 0);

		// ggf paar bytes weiterspringen
		do {
			seek += 2;
			if(libendian::le_read_us(&height, file) != 0)
				return 5;
		} while(height == 0);

		header->setWidth(width);
		header->setHeight(height);

		// 6 Unbekannte Daten überspringen
		fseek(file, 6 - seek, SEEK_CUR);

		ArchivItem_Raw *layer = dynamic_cast<ArchivItem_Raw*>((*allocator)(BOBTYPE_RAW, 0, NULL));
		if(layer->load(file, header->getWidth() * header->getHeight()) != 0)
			return 6;
		set(i+2, layer);
	}

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  schreibt die Mapdaten in eine Datei.
 *
 *  @param[in] file Dateihandle der Datei
 *
 *	@return liefert Null bei Erfolg, ungleich Null bei Fehler
 *
 *  @author FloSoft
 */
int libsiedler2::ArchivItem_Map::write(FILE *file) const
{
	if(file == NULL)
		return 1;

	/// @todo Schreiben der Mapdaten.

	return 256;
}
