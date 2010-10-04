// $Id: ArchivItem_Sound_Other.cpp 6581 2010-07-16 11:16:34Z FloSoft $
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

///////////////////////////////////////////////////////////////////////////////
// Header
#include "main.h"
#include "ArchivItem_Sound_Other.h"

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#if defined _WIN32 && defined _DEBUG && defined _MSC_VER
	#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////////
/** @class libsiedler2::baseArchivItem_Sound_Other
 *
 *  Klasse für Other-Sounds.
 *
 *  @author FloSoft
 */

///////////////////////////////////////////////////////////////////////////////
/**
 *  Konstruktor von @p baseArchivItem_Sound_Other.
 *
 *  @author FloSoft
 */
libsiedler2::baseArchivItem_Sound_Other::baseArchivItem_Sound_Other(void) : baseArchivItem_Sound()
{
	length = 0;
	data = NULL;

	setType(SOUNDTYPE_OTHER);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Kopierkonstruktor von @p baseArchivItem_Sound_Other.
 *
 *  @param[in] item Quellitem
 *
 *  @author FloSoft
 */
libsiedler2::baseArchivItem_Sound_Other::baseArchivItem_Sound_Other(const baseArchivItem_Sound_Other *item) : baseArchivItem_Sound( (baseArchivItem_Sound*)item )
{
	type = item->type;
	length = item->length;
	data = NULL;

	if(length != 0)
	{
		data = new unsigned char[length];
		memcpy(data, item->data, length);
	}
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Destruktor von @p baseArchivItem_Sound_Other.
 *
 *  @author FloSoft
 */
libsiedler2::baseArchivItem_Sound_Other::~baseArchivItem_Sound_Other(void)
{
	clear();
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  lädt die Daten aus einer Datei.
 *
 *  @param[in] file    Dateihandle der Datei
 *  @param[in] length  Länge der Daten
 *
 *	@return liefert Null bei Erfolg, ungleich Null bei Fehler
 *
 *  @author FloSoft
 */
int libsiedler2::baseArchivItem_Sound_Other::load(FILE *file, unsigned int length)
{
	if(file == NULL || length == 0)
		return 1;

	alloc(length);

	if(libendian::le_read_uc(data, length, file) != (int)length)
		return 2;

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  schreibt die Daten in eine Datei.
 *
 *  @param[in] file        Dateihandle der Datei
 *
 *	@return liefert Null bei Erfolg, ungleich Null bei Fehler
 *
 *  @author FloSoft
 */
int libsiedler2::baseArchivItem_Sound_Other::write(FILE *file) const
{
	if(file == NULL)
		return 1;

	if(libendian::le_write_ui(length, file) != 0)
		return 2;

	if(libendian::le_write_uc(data, length, file) != (int)length)
		return 3;

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  alloziert Soundspeicher für die gewünschte Größe.
 *
 *  @param[in] length Größe des Speichers
 *
 *  @author FloSoft
 */
void libsiedler2::baseArchivItem_Sound_Other::alloc(unsigned int length)
{
	clear();

	this->length = length;

	if(length != 0)
	{
		data = new unsigned char[length];
		memset(data, 0, length);
	}
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  räumt den Soundspeicher auf.
 *
 *  @author FloSoft
 */
void libsiedler2::baseArchivItem_Sound_Other::clear(void)
{
	delete[] data;

	length = 0;
	data = NULL;
}
