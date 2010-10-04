// $Id: ArchivItem_Bitmap_Shadow.cpp 6581 2010-07-16 11:16:34Z FloSoft $
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
#include "ArchivItem_Bitmap_Shadow.h"

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#if defined _WIN32 && defined _DEBUG && defined _MSC_VER
	#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////////
/** @class libsiedler2::baseArchivItem_Bitmap_Shadow
 *
 *  Basisklasse für Shadow-Bitmaps.
 *
 *  @author FloSoft
 */

///////////////////////////////////////////////////////////////////////////////
/** @class libsiedler2::ArchivItem_Bitmap_Shadow
 *
 *  Klasse für Shadow-Bitmaps.
 *
 *  @author FloSoft
 */

///////////////////////////////////////////////////////////////////////////////
/**
 *  Konstruktor von @p baseArchivItem_Bitmap_Shadow.
 *
 *  @author FloSoft
 */
libsiedler2::baseArchivItem_Bitmap_Shadow::baseArchivItem_Bitmap_Shadow(void) : baseArchivItem_Bitmap()
{
	setBobType(BOBTYPE_BITMAP_SHADOW);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Kopierkonstruktor von @p baseArchivItem_Bitmap_Shadow.
 *
 *  @param[in] item Quellitem
 *
 *  @author FloSoft
 */
libsiedler2::baseArchivItem_Bitmap_Shadow::baseArchivItem_Bitmap_Shadow(const baseArchivItem_Bitmap_Shadow *item) : baseArchivItem_Bitmap((baseArchivItem_Bitmap*)item)
{
	setBobType(BOBTYPE_BITMAP_SHADOW);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Konstruktor von @p baseArchivItem_Bitmap_Raw mit Laden der Bilddaten aus
 *  einer Datei.
 *
 *  @param[in] file    Dateihandle der Datei
 *  @param[in] palette Grundpalette
 *
 *  @author FloSoft
 */
libsiedler2::baseArchivItem_Bitmap_Shadow::baseArchivItem_Bitmap_Shadow(FILE *file, const ArchivItem_Palette *palette) : baseArchivItem_Bitmap()
{
	setBobType(BOBTYPE_BITMAP_SHADOW);
	load(file, palette);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Destruktor von @p baseArchivItem_Bitmap_Shadow.
 *
 *  @author FloSoft
 */
libsiedler2::baseArchivItem_Bitmap_Shadow::~baseArchivItem_Bitmap_Shadow(void)
{
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  lädt die Bilddaten aus einer Datei.
 *
 *  @param[in] file    Dateihandle der Datei
 *  @param[in] palette Grundpalette
 *
 *	@return liefert Null bei Erfolg, ungleich Null bei Fehler
 *
 *  @author FloSoft
 */
int libsiedler2::baseArchivItem_Bitmap_Shadow::load(FILE *file, const ArchivItem_Palette *palette)
{
	unsigned char *data = NULL;

	if(file == NULL)
		return 1;
	if(palette == NULL)
		palette = this->palette;
	if(palette == NULL)
		return 2;

	tex_clear();

	// Nullpunkt X einlesen
	if(libendian::le_read_s(&nx, file) != 0)
		return 2;

	// Nullpunkt Y einlesen
	if(libendian::le_read_s(&ny, file) != 0)
		return 3;

	// Unbekannte Daten überspringen
	fseek(file, 4, SEEK_CUR);

	// Breite einlesen
	if(libendian::le_read_us(&width, file) != 0)
		return 4;

	// Höhe einlesen
	if(libendian::le_read_us(&height, file) != 0)
		return 5;

	// Unbekannte Daten überspringen
	fseek(file, 2, SEEK_CUR);

	// Länge einlesen
	if(libendian::le_read_ui(&length, file) != 0)
		return 6;

	// Daten einlesen
	if(length != 0)
	{
		data = new unsigned char[length];
		if(libendian::le_read_uc(data, length, file) != (int)length)
			return 7;
	}

	// Speicher anlegen
	tex_alloc();

	unsigned char gray = palette->lookup(255,255,255);

	if(length != 0 && data)
	{
		unsigned char *image = &data[height*2];
		unsigned int position = 0;

		// Einlesen
		for(unsigned short y = 0; y < height; ++y)
		{
			unsigned short x = 0;

			// Solange Zeile einlesen, bis x voll ist
			while(x < width)
			{
				// graue Pixel setzen
				unsigned char count = image[position++];
				for(unsigned char i = 0; i < count; ++i, ++x)
					tex_setPixel(x, y, gray, palette);

				// transparente Pixel setzen
				count = image[position++];
				for(unsigned char i = 0; i < count; ++i, ++x)
					tex_setPixel(x, y, TRANSPARENT_INDEX, palette);
			}

			// FF überspringen
			++position;
		}

		// FF überspringen
		++position;

		if(position != length-(height*2) )
			return 8;

		delete[] data;
	}

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  schreibt die Bilddaten in eine Datei.
 *
 *  @param[in] file    Dateihandle der Datei
 *  @param[in] palette Grundpalette
 *
 *	@return liefert Null bei Erfolg, ungleich Null bei Fehler
 *
 *  @author FloSoft
 */
int libsiedler2::baseArchivItem_Bitmap_Shadow::write(FILE *file, const ArchivItem_Palette *palette) const
{
	unsigned char *data = NULL;

	if(file == NULL)
		return 1;
	if(palette == NULL)
		palette = this->palette;
	if(palette == NULL)
		return 2;

	if(width == 0 || height == 0)
		return 2;

	// Nullpunkt X schreiben
	if(libendian::le_write_s(nx, file) != 0)
		return 3;

	// Nullpunkt Y schreiben
	if(libendian::le_write_s(ny, file) != 0)
		return 4;

	// Unbekannte Daten schreiben
	char unknown[4] = {0x00, 0x00, 0x00, 0x00};
	if(libendian::le_write_c(unknown, 4, file) != 4)
		return 5;

	// Breite schreiben
	if(libendian::le_write_us(width, file) != 0)
		return 6;

	// Höhe einlesen
	if(libendian::le_write_us(height, file) != 0)
		return 7;

	// Unbekannte Daten schreiben
	char unknown2[2] = {0x01, 0x00};
	if(libendian::le_write_c(unknown2, 2, file) != 2)
		return 8;

	// maximale größe von RLE: width*height*2
	data = new unsigned char[height*4+width*height*2];
	memset(data, 0, width*height*2);

	// Startadressen
	unsigned char *image = &data[height*2];
	unsigned short *starts = (unsigned short*)&data[0];

	// Schattendaten kodieren
	unsigned short position = 0;
	for(unsigned short y = 0; y < height; ++y)
	{
		unsigned short x = 0;

		// Startadresse setzen
		starts[y] = position + height*2;

		// Solange Zeile nicht voll
		while(x < width)
		{
			// graue Pixel schreiben
			unsigned char count, color;
			for(count = 0; count < width-x; ++count)
			{
				color = tex_getPixel(x+count, y, palette);
				if(color == TRANSPARENT_INDEX)
					break;
			}
			image[position++] = count;

			x += count;

			// transparente Pixel schreiben
			for(count = 0; count < width-x; ++count)
			{
				color = tex_getPixel(x+count, y, palette);
				if(color != TRANSPARENT_INDEX)
					break;
			}
			image[position++] = count;

			x += count;
		}

		image[position++] = 0xFF;
	}
	image[position++] = 0xFF;

	unsigned int length = position + height*2;

	// Länge schreiben
	if(libendian::le_write_ui(length, file) != 0)
		return 9;

	// Daten schreiben
	if(libendian::le_write_uc(data, length, file) != (int)length)
		return 10;

	delete[] data;

	return 0;
}
