// $Id: ArchivInfo.cpp 7521 2011-09-08 20:45:55Z FloSoft $
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
#include "ArchivInfo.h"

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#if defined _WIN32 && defined _DEBUG && defined _MSC_VER
	#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////////
/** @class libsiedler2::ArchivInfo
 *
 *  Klasse für Archivdateien.
 *
 *  @author FloSoft
 */

///////////////////////////////////////////////////////////////////////////////
/** @var libsiedler2::ArchivInfo::count
 * 
 *  Anzahl der Elemente.
 *
 *  @author FloSoft
 */

///////////////////////////////////////////////////////////////////////////////
/** @var libsiedler2::ArchivInfo::data
 * 
 *  die Elemente.
 *
 *  @author FloSoft
 */

///////////////////////////////////////////////////////////////////////////////
/** 
 *  Konstruktor von @p ArchivInfo.
 *
 *  @author FloSoft
 */
libsiedler2::ArchivInfo::ArchivInfo(void) : data(NULL), count(0)
{
}

///////////////////////////////////////////////////////////////////////////////
/** 
 *  Kopierkonstruktor von @p ArchivInfo.
 *
 *  @param[in] info Quellitem
 *
 *  @author FloSoft
 */
libsiedler2::ArchivInfo::ArchivInfo(const ArchivInfo &info) : data(NULL), count(0)
{
	set(&info);
}

///////////////////////////////////////////////////////////////////////////////
/** 
 *  Kopierkonstruktor von @p ArchivInfo.
 *
 *  @param[in] info Quellitem
 *
 *  @author FloSoft
 */
libsiedler2::ArchivInfo::ArchivInfo(const ArchivInfo *info) : data(NULL), count(0)
{
	set(info);
}

///////////////////////////////////////////////////////////////////////////////
/** 
 *  Destruktor von @p ArchivInfo, räumt automatisch auf.
 *
 *  @author FloSoft
 */
libsiedler2::ArchivInfo::~ArchivInfo(void)
{
	clear();
}

///////////////////////////////////////////////////////////////////////////////
/** 
 *  erstellt den Datensatz in einer bestimmten Größe.
 *
 *  @param[in] count Elementanzahl auf den der Datensatz angelegt werden soll
 *
 *  @author FloSoft
 */
void libsiedler2::ArchivInfo::alloc(unsigned long count)
{
	clear();

	this->count = count;
	data = new ArchivItem*[count];

	memset(data, 0, sizeof(ArchivItem*)*count);
}

///////////////////////////////////////////////////////////////////////////////
/** 
 *  vergrößert den Datensatz um eine bestimmten Größe.
 *
 *  @param[in] increment Elementanzahl um den der Datensatz vergrößert werden soll
 *
 *  @author FloSoft
 */
void libsiedler2::ArchivInfo::alloc_inc(unsigned long increment)
{
	ArchivItem **new_data = new ArchivItem*[count+increment];

	memset(new_data, 0, sizeof(ArchivItem*)*(count+increment));

	if(count > 0 && data)
	{
		for(unsigned long i = 0; i < count; ++i)
			new_data[i] = data[i];

		delete[] data;
	}

	data = new_data;
	count += increment;
}

///////////////////////////////////////////////////////////////////////////////
/** 
 *  gibt die angelegten Daten wieder frei.
 *
 *  @author FloSoft
 */
void libsiedler2::ArchivInfo::clear(void)
{
	if(data)
	{
		for(unsigned long c = 0; c < count; ++c)
		{
			delete data[c];
			data[c] = NULL;
		}
		delete[] data;
	}

	data = NULL;
	count = 0;
}

///////////////////////////////////////////////////////////////////////////////
/** 
 *  Setzt den Inhalt eines ArchivItems auf das des Übergebenen.
 *
 *  @param[in] index Index des zu setzenden Eintrags
 *  @param[in] item  Item mit dem zu setzenden Inhalt
 *
 *  @author FloSoft
 */

///////////////////////////////////////////////////////////////////////////////
/** 
 *  kopiert den Inhalt eines ArchivItems auf das des Übergebenen.
 *
 *  @param[in] index Index des zu setzenden Eintrags
 *  @param[in] item  Item mit dem zu kopierenden Inhalt
 *
 *  @author FloSoft
 */
void libsiedler2::ArchivInfo::setC(int index, const ArchivItem *item)
{
	if(!data)
		return;

	if( (unsigned long)index < count && index >= 0)
	{
		// ist das Item gültig?
		if(item == NULL)
			data[(unsigned long)index] = NULL;
		else
		{
			// ja, dann neues item erstellen
			data[(unsigned long)index] = (ArchivItem*)(*allocator)(0, 0, item);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
/** 
 *  fügt ein Element hinten an.
 *
 *  @param[in] item Item mit dem anzufügenden Inhalt
 *
 *  @author FloSoft
 */

///////////////////////////////////////////////////////////////////////////////
/** 
 *  fügt ein Element hinten an und kopiert die Daten von @p item.
 *
 *  @param[in] item Item mit dem zu kopierenden Inhalt
 *
 *  @author FloSoft
 */
void libsiedler2::ArchivInfo::pushC(const ArchivItem *item)
{
	alloc_inc(1);

	if(item)
		data[count-1] = (ArchivItem*)(*allocator)(0, 0, item);
	else
		data[count-1] = NULL;
}

///////////////////////////////////////////////////////////////////////////////
/** 
 *  liefert den Inhalt eines ArchivItems am entsprechenden Index.
 *
 *  @param[in] index Index des zu setzenden Eintrags
 *
 *  @return liefert NULL bei Fehler, ansonsten das entsprechende Item
 *
 *  @author FloSoft
 */

///////////////////////////////////////////////////////////////////////////////
/** 
 *  liefert den Inhalt eines ArchivItems am entsprechenden Index.
 *
 *  @param[in] index Index des zu setzenden Eintrags
 *
 *  @return liefert NULL bei Fehler, ansonsten das entsprechende Item
 *
 *  @author FloSoft
 */

///////////////////////////////////////////////////////////////////////////////
/** 
 *  liefert den Pointer eines ArchivItems am entsprechenden Index.
 *
 *  @param[in] index Index des zu setzenden Eintrags
 *
 *  @return liefert NULL bei Fehler, ansonsten das entsprechende Item
 *
 *  @author FloSoft
 */

///////////////////////////////////////////////////////////////////////////////
/** 
 *  liefert die Größe des Archivs.
 *
 *  @return liefert die Größe des Archivs.
 *
 *  @author FloSoft
 */

///////////////////////////////////////////////////////////////////////////////
/** 
 *  Index-Operator von @p ArchivInfo. 
 *
 *  @param[in] index Index des zu liefernden Eintrags
 *
 *  @return Bei Erfolg ArchivItem, ansonsten NULL
 *
 *  @author FloSoft
 */

///////////////////////////////////////////////////////////////////////////////
/** 
 *  Zuweisungsoperator von @p ArchivInfo.
 *
 *  @param[in] info Quellitem
 *
 *  @return Referenz auf this
 *
 *  @author FloSoft
 */

///////////////////////////////////////////////////////////////////////////////
/** 
 *  Kopierfunktion von @p ArchivInfo.
 *
 *  @param[in] to     Zielposition
 *  @param[in] from   Quellposition
 *  @param[in] count  Anzahl
 *  @param[in] source Quelle
 *
 *  @author FloSoft
 */
void libsiedler2::ArchivInfo::copy(unsigned int to, unsigned int from, unsigned int count, const ArchivInfo *source)
{
	if(to+count > this->count)
		alloc_inc(to+count-this->count);

	for(unsigned int f = from; f < from+count; ++to, ++f)
		setC(to, source->get(f));
}
