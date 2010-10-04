// $Id: LoadSND.cpp 6581 2010-07-16 11:16:34Z FloSoft $
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
 *  lädt eine Sounddatei in ein ArchivInfo. (midi, xmidi, wave)
 *
 *  @param[in]  file    Dateiname der Sounddatei
 *  @param[out] items   ArchivInfo-Struktur, welche gefüllt wird
 *
 *  @return Null bei Erfolg, ein Wert ungleich Null bei Fehler
 *
 *  @author FloSoft
 *  @author OLiver
 */
int libsiedler2::loader::LoadSND(const char *file, ArchivInfo *items)
{
	FILE *snd;
	long size;

	if(file == NULL || items == NULL)
		return 1;

	// Datei zum lesen öffnen
	snd = fopen(file, "rb");

	// hat das geklappt?
	if(snd == NULL)
		return 2;

	fseek(snd, 0, SEEK_END);
	size = ftell(snd);
	fseek(snd, 0, SEEK_SET);

	baseArchivItem_Sound *sound = baseArchivItem_Sound::findSubType(snd);

	if(!sound)
		return 3;

	if(sound->load(snd, size) != 0)
		return 4;

	items->alloc(1);
	items->set(0, sound);

	fclose(snd);

	return 0;
}
