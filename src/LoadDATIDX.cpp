// $Id: LoadDATIDX.cpp 6581 2010-07-16 11:16:34Z FloSoft $
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

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#if defined _WIN32 && defined _DEBUG && defined _MSC_VER
	#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////////
/**
 *  lädt eine DAT/IDX-File in ein ArchivInfo.
 *
 *  @param[in]  file    Dateiname der DAT/IDX-File
 *  @param[in]  palette Grundpalette der DAT/IDX-File
 *  @param[out] items   ArchivInfo-Struktur, welche gefüllt wird
 *
 *  @return Null bei Erfolg, ein Wert ungleich Null bei Fehler
 *
 *  @author FloSoft
 *  @author OLiver
 */
int libsiedler2::loader::LoadDATIDX(const char *file, const ArchivItem_Palette *palette, ArchivInfo *items)
{
	if(file == NULL || items == NULL)
		return 1;

	size_t length = strlen(file) + 1;
	FILE *idx, *dat;
	char *datfile = new char[length];
	char *idxfile = new char[length];
	unsigned int count;

	memcpy(idxfile, file, length);
	memcpy(datfile, file, length);

	idxfile[length - 2] = 'X';
	idxfile[length - 3] = 'D';
	idxfile[length - 4] = 'I';

	datfile[length - 2] = 'T';
	datfile[length - 3] = 'A';
	datfile[length - 4] = 'D';

	// Datei zum lesen öffnen
	dat = fopen(datfile, "rb");

	// hat das geklappt?
	if(dat == NULL)
		return 2;

	// IDX-Datei zum lesen öffnen
	idx = fopen(idxfile, "rb");

	// hat das geklappt?
	if(idx == NULL)
		return 3;

	delete[] datfile;
	delete[] idxfile;

	// Anzahl einlesen
	if(libendian::le_read_ui(&count, idx) != 0)
		return 5;

	// Platz für items anlegen
	items->alloc(count);

	// items einlesen
	for(unsigned int i = 0; i < count; ++i)
	{
		char name[17];
		unsigned int offset;
		short idxbobtype;
		short bobtype;

		// Name einlesen
		if(libendian::le_read_c(name, 16, idx) != 16)
			return 6;

		// Offset einlesen
		if(libendian::le_read_ui(&offset, idx) != 0)
			return 7;

		// Unbekannte Daten überspringen
		fseek(idx, 6, SEEK_CUR);

		// BobType einlesen
		if(libendian::le_read_s(&idxbobtype, idx) != 0)
			return 8;

		// Zum Offset springen
		fseek(dat, offset, SEEK_SET);

		// BobType einlesen
		if(libendian::le_read_s(&bobtype, dat) != 0)
			return 9;

		if(idxbobtype != bobtype)
			continue;

		// Daten von Item auswerten
		if(LoadType(bobtype, dat, palette, items->getP(i)) != 0)
			return 10;

		// Name setzen
		ArchivItem *item = items->get(i);
		if(item)
			item->setName(name);
	}

	fclose(idx);
	fclose(dat);

	return 0;
}
