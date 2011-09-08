// $Id: ArchivInfo.h 7521 2011-09-08 20:45:55Z FloSoft $
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
#ifndef ARCHIVINFO_H_INCLUDED
#define ARCHIVINFO_H_INCLUDED

#pragma once

#include "ArchivItem.h"

namespace libsiedler2
{
	/// Klasse für Archivdateien.
	class ArchivInfo 
	{
	public:
		/// Konstruktor von @p ArchivInfo.
		ArchivInfo(void);

		/// Kopierkonstruktor von @p ArchivInfo.
		ArchivInfo(const ArchivInfo &info);
		ArchivInfo(const ArchivInfo *info);

		/// Destruktor von @p ArchivInfo, räumt automatisch auf.
		virtual ~ArchivInfo(void);

		/// erstellt den Datensatz in einer bestimmten Größe.
		void alloc(unsigned long count);

		/// vergrößert den Datensatz um eine bestimmten Größe.
		void alloc_inc(unsigned long increment);

		/// gibt die angelegten Daten wieder frei.
		void clear(void);

		/// setzt den Inhalt auf das übergebene ArchivInfo
		inline void set(const ArchivInfo *info)
		{
			alloc(info->count);
			for(unsigned long i = 0; i < count; ++i)
				setC(i, info->get(i));
		}

		/// Setzt den Inhalt eines ArchivItems auf das des Übergebenen.
		inline void set(int index, ArchivItem *item)
		{
			if(!data)
				return;

			if( (unsigned long)index < count && index >= 0)
				data[(unsigned long)index] = item;
		}

		/// kopiert den Inhalt eines ArchivItems auf das des Übergebenen.
		void setC(int index, const ArchivItem *item);

		/// fügt ein Element hinten an.
		inline void push(ArchivItem *item)
		{
			alloc_inc(1);

			data[count-1] = item;
		}

		/// fügt ein Element hinten an und kopiert die Daten von @p item.
		void pushC(const ArchivItem *item);

		/// liefert den Inhalt eines ArchivItems am entsprechenden Index.
		inline ArchivItem *get(int index)
		{
			if(!data)
				return NULL;

			if( (unsigned long)index < count && index >= 0)
				return data[(unsigned long)index];

			return NULL;
		}

		/// liefert den Inhalt eines ArchivItems am entsprechenden Index.
		inline const ArchivItem *get(int index) const
		{
			if(!data)
				return NULL;

			if( (unsigned long)index < count && index >= 0)
				return data[(unsigned long)index];

			return NULL;
		}

		/// liefert den Pointer eines ArchivItems am entsprechenden Index.
		inline ArchivItem **getP(int index)
		{
			if(!data)
				return NULL;

			if( (unsigned long)index < count && index >= 0)
				return &data[(unsigned long)index];

			return NULL;
		}

		/// liefert das erste Item mit entsprechenden Namen
		inline ArchivItem *find(const char *name)
		{
			for(unsigned long i = 0; i < getCount(); ++i)
			{
				if(get(i) && strcmp(get(i)->getName(), name) == 0)
					return get(i);
			}

			return NULL;
		}

		/// liefert das erste Item mit entsprechenden Namen
		inline const ArchivItem *find(const char *name) const
		{
			for(unsigned long i = 0; i < getCount(); ++i)
			{
				if(get(i) && strcmp(get(i)->getName(), name) == 0)
					return get(i);
			}

			return NULL;
		}

		/// liefert die Größe des Archivs.
		inline unsigned long getCount(void) const
		{
			return count;
		}

		/// Index-Operator von @p ArchivInfo. 
		inline const ArchivItem *operator[](int index) const
		{
			return get(index);
		}

		/// Index-Operator von @p ArchivInfo. 
		inline ArchivItem *operator[](int index)
		{
			return get(index);
		}

		/// Zuweisungsoperator von @p ArchivInfo.
		inline ArchivInfo& operator=(const ArchivInfo &info)
		{
			alloc(info.count);

			for(unsigned long i = 0; i < count; ++i)
				setC(i, info.get(i));

			return *this;
		}

		/// Kopierfunktion von @p ArchivInfo.
		void copy(unsigned int to, unsigned int from, unsigned int count, const ArchivInfo *source);

	protected:
		ArchivItem **data;   ///< die Elemente.
		unsigned long count; ///< Anzahl der Elemente.
	};
}

#endif // ARCHIVINFO_H_INCLUDED
