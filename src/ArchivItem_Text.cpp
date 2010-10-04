// $Id: ArchivItem_Text.cpp 6581 2010-07-16 11:16:34Z FloSoft $
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
#include "ArchivItem_Text.h"

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#if defined _WIN32 && defined _DEBUG && defined _MSC_VER
	#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////////
/** @class libsiedler2::ArchivItem_Text
 *
 *  Klasse für Texte.
 *
 *  @author FloSoft
 */

///////////////////////////////////////////////////////////////////////////////
/** @var libsiedler2::ArchivItem_Text::text
 *
 *  Der Textspeicher.
 *
 *  @author FloSoft
 */

///////////////////////////////////////////////////////////////////////////////
/** @var libsiedler2::ArchivItem_Text::length
 *
 *  Die Länge des Textes.
 *
 *  @author FloSoft
 */



/// Tabelle zum Konvertieren von ANSI nach OEM.
static const unsigned char gs_ansi2oemTab[] =
{
	/*0080:*/ 0x00,0x00,0x00,0x9F, 0x00,0x00,0x00,0xD8, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,
	/*0090:*/ 0x00,0x60,0x27,0x00, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,
	/*00A0:*/ 0xFF,0xAD,0x9B,0x9C, 0x0F,0x9D,0x7C,0x15, 0x22,0x63,0xA6,0xAE, 0xAA,0x2D,0x52,0x00,
	/*00B0:*/ 0xF8,0xF1,0xFD,0x33, 0x27,0xE6,0x14,0xFA, 0x2C,0x31,0xA7,0xAF, 0xAC,0xAB,0x00,0xA8,
	/*00C0:*/ 0x41,0x41,0x41,0x41, 0x8E,0x8F,0x92,0x80, 0x45,0x90,0x45,0x45, 0x49,0x49,0x49,0x49,
	/*00D0:*/ 0x44,0xA5,0x4F,0x4F, 0x4F,0x4F,0x99,0x78, 0x4F,0x55,0x55,0x55, 0x9A,0x59,0x00,0xE1,
	/*00E0:*/ 0x85,0xA0,0x83,0x61, 0x84,0x86,0x91,0x87, 0x8A,0x82,0x88,0x89, 0x8D,0xA1,0x8C,0x8B,
	/*00F0:*/ 0x64,0xA4,0x95,0xA2, 0x93,0x6F,0x94,0xF6, 0x6F,0x97,0xA3,0x96, 0x81,0x79,0x00,0x98,
};



///////////////////////////////////////////////////////////////////////////////
/**
 *  Konstruktor von @p ArchivItem_Text.
 *
 *  @author FloSoft
 */
libsiedler2::ArchivItem_Text::ArchivItem_Text() : ArchivItem()
{
	text = NULL;
	length = 0;
	setBobType(BOBTYPE_TEXT);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Destruktor von @p ArchivItem_Text.
 *
 *  @author FloSoft
 */
libsiedler2::ArchivItem_Text::~ArchivItem_Text()
{
	delete[] text;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Kopierkonstruktor von @p ArchivItem_Text.
 *
 *  @param[in] item Quellitem
 *
 *  @author FloSoft
 */
libsiedler2::ArchivItem_Text::ArchivItem_Text(const ArchivItem_Text *item) : ArchivItem( (ArchivItem*)item )
{
	text = NULL;
	setBobType(BOBTYPE_TEXT);

	setText(item->text, false, item->length);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Konstruktor von @p ArchivItem_Text mit Laden des Textes aus
 *  einer Datei.
 *
 *  @param[in] file       Dateihandle aus denen die Farbwerte geladen werden sollen
 *  @param[in] conversion Soll ggf. OEM-Charset in ANSI umgewandelt werden?
 *  @param[in] length     Länge des Blocks (Wieviel Bytes sollen eingelesen werden?)
 *
 *  @author FloSoft
 */
libsiedler2::ArchivItem_Text::ArchivItem_Text(FILE *file, bool conversion, unsigned int length) : ArchivItem()
{
	text = NULL;
	setBobType(BOBTYPE_TEXT);

	load(file, conversion, length);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  liest den Text aus einer Datei.
 *
 *  @param[in] file       Dateihandle aus denen der Text geladen werden sollen
 *  @param[in] conversion Soll ggf. OEM-Charset in ANSI umgewandelt werden?
 *  @param[in] length     Länge des Blocks (Wieviel Bytes sollen eingelesen werden?)
 *
 *	@return liefert Null bei Erfolg, ungleich Null bei Fehler
 *
 *  @todo Hmm nur temporärer Fix! ist dieses doofe Escape-zeichen am Ende der Files
 *
 *  @author FloSoft
 */
int libsiedler2::ArchivItem_Text::load(FILE *file, bool conversion, unsigned int length)
{
	if(file == NULL)
		return 1;

	// Aktuelle Position bestimmen
	long pos = ftell(file);

	this->length = length;
	if(this->length == 0)
	{
		// Länge der Datei bestimmen
		fseek(file, 0, SEEK_END);
		this->length = ftell(file) - pos;
		fseek(file, pos, SEEK_SET);
	}

	// Alten Text ggf. löschen
	delete[] text;

	text = new char[this->length+1];
	memset(text, 0, this->length+1);

	// Einlesen
	if(libendian::le_read_c(text, this->length, file) != (int)this->length)
		return 2;

	/// TODO: Hmm nur temporärer Fix! ist dieses doofe Escape-zeichen am Ende der Files
	if(text[this->length-1] == 26)
		text[this->length-1] = 0;

	if(conversion)
		Oem2Ansi(text, this->length);

	for(unsigned int i = 0; i < this->length; ++i)
	{
		if(text[i] == '@' && text[i+1] == '@')
		{
			text[i] = '\r';
			text[i+1] = '\n';
		}
	}

	// Text setzen
	setName(text);

	// Alles OK
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  schreibt dem Text in eine Datei.
 *
 *  @param[in] file       Dateihandle in die der Text geschrieben werden soll
 *  @param[in] conversion Soll ggf. ANSI-Charset in OEM umgewandelt werden?
 *
 *	@return liefert Null bei Erfolg, ungleich Null bei Fehler
 *
 *  @author FloSoft
 */
int libsiedler2::ArchivItem_Text::write(FILE *file, bool conversion) const
{
	if(file == NULL)
		return 1;

	// Wenn Länge 0, nix schreiben, ist ja kein Fehler!
	if(length == 0)
		return 0;

	unsigned int length = this->length;
	char *text = new char[length*2+1];
	memset(text, 0, length*2+1);

	for(unsigned int i = 0, j = 0; i < length; ++i)
	{
		if(this->text[i] == '\n')
		{
			text[j++] = '@';
			text[j++] = '@';
		}
		else if(this->text[i] == '\r')
			continue;
		else
			text[j++] = this->text[i];
	}

	//memcpy(text, this->text, length);

	// ggf umwandeln
	if(conversion)
		Ansi2Oem(text, length);

	// Schreiben
	if(libendian::le_write_c(text, length+1, file) != (int)length+1)
		return 2;

	delete[] text;

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  liefert den Text.
 *
 *	@return liefert einen konstanten Zeiger auf das Textelement, NULL bei leerem Text
 *
 *  @author FloSoft
 */
const char *libsiedler2::ArchivItem_Text::getText(void) const
{
	return text;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  liefert die Länge.
 *
 *	@return liefert die Länge des Textes
 *
 *  @author FloSoft
 */
unsigned int libsiedler2::ArchivItem_Text::getLength(void) const
{
	return length;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  setzt den Text.
 *
 *  @param[in] text       Der Text der gesetzt werden soll, falls @p NULL, wird
 *                        evtl vorhandener Text gelöscht
 *  @param[in] conversion Soll ggf. ANSI-Charset in OEM umgewandelt werden?
 *  @param[in] length     Länge des Textes, bei @p 0 wird @p strlen verwendet
 *
 *  @author FloSoft
 */
void libsiedler2::ArchivItem_Text::setText(const char *text, bool conversion, unsigned int length)
{
	// alten Text löschen
	delete[] this->text;
	this->length = 0;

	// ist der neue Text leer?
	if(text == NULL)
		return;

	// ggf. Länge bestimmen
	if(length == 0)
		length = (unsigned int)strlen(text);

	// neuen Textspeicher erzeugen
	this->text = new char[length+1];
	memset(this->text, 0, length+1);

	// text kopieren
	memcpy(this->text, text, length);

	this->length = length;

	if(conversion)
		Ansi2Oem(this->text, this->length);

	// Name setzen
	if(strlen(getName()) == 0)
		setName(this->text);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Konvertiert einen String von ANSI-Charset nach OEM-Charset.
 *
 *  @param[in] string Der Text, der konvertiert werden soll
 *  @param[in] length Länge des Textes, bei @p 0 wird @p strlen verwendet
 *
 *  @return liefert @p string zurück
 *
 *  @author FloSoft
 */
char *libsiedler2::ArchivItem_Text::Ansi2Oem(char *string, unsigned int length)
{
	if (string == NULL) return NULL;

	for (unsigned int i = 0; (string[i] != 0) && (i < length); ++i) {

		unsigned char c = (unsigned char) string[i];

		// Ab char 128 noetig
		if (c > 128) string[i] = (signed char) gs_ansi2oemTab[c & 0x7F];
	}

	return string;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Konvertiert einen String von UTF-8-Charset nach OEM-Charset.
 *
 *  @param[in] string Der Text, der konvertiert werden soll
 *  @param[in] length Länge des Textes, bei @p 0 wird @p strlen verwendet
 *
 *  @return liefert @p string zurück
 *
 *  @author sicherha
 */
char *libsiedler2::ArchivItem_Text::Utf82Oem(char *string, unsigned int length)
{
	unsigned int getPos = 0;
	unsigned int putPos = 0;

	if (!string) return NULL;

	while ((string[getPos] != 0) && (getPos < length)) {

		unsigned char c = string[getPos++];

		// Escape-Zeichen interpretieren
		if (c == 0xc3) c = string[getPos++] + 0x40;

		// In OEM umwandeln
		if (c > 0x80) c = gs_ansi2oemTab[c & 0x7f];
		string[putPos++] = c;
	}

	// String neu terminieren, da er kuerzer geworden sein koennte
	string[putPos] = '\0';

	return string;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Konvertiert einen String von OEM-Charset nach ANSI-Charset.
 *
 *  @param[in] string Der Text der konvertiert werden soll
 *  @param[in] length Länge des Textes, bei @p 0 wird @p strlen verwendet
 *
 *  @return liefert @p string zurück
 *
 *  @author FloSoft
 */
char *libsiedler2::ArchivItem_Text::Oem2Ansi(char *string, unsigned int length)
{
  static unsigned char oem2ansi_tab[256] = {
    /*0000:*/ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
    /*0010:*/ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
    /*0020:*/ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
    /*0030:*/ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
    /*0040:*/ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
    /*0050:*/ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
    /*0060:*/ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
    /*0070:*/ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
    /*0080:*/ 0x00,0x00,0x00,0x9F, 0x00,0x00,0x00,0xD8, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,
    /*0090:*/ 0x00,0x60,0x27,0x00, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,
    /*00A0:*/ 0xFF,0xAD,0x9B,0x9C, 0x0F,0x9D,0x7C,0x15, 0x22,0x63,0xA6,0xAE, 0xAA,0x2D,0x52,0x00,
    /*00B0:*/ 0xF8,0xF1,0xFD,0x33, 0x27,0xE6,0x14,0xFA, 0x2C,0x31,0xA7,0xAF, 0xAC,0xAB,0x00,0xA8,
    /*00C0:*/ 0x41,0x41,0x41,0x41, 0x8E,0x8F,0x92,0x80, 0x45,0x90,0x45,0x45, 0x49,0x49,0x49,0x49,
    /*00D0:*/ 0x44,0xA5,0x4F,0x4F, 0x4F,0x4F,0x99,0x78, 0x4F,0x55,0x55,0x55, 0x9A,0x59,0x00,0xE1,
    /*00E0:*/ 0x85,0xA0,0x83,0x61, 0x84,0x86,0x91,0x87, 0x8A,0x82,0x88,0x89, 0x8D,0xA1,0x8C,0x8B,
    /*00F0:*/ 0x64,0xA4,0x95,0xA2, 0x93,0x6F,0x94,0xF6, 0x6F,0x97,0xA3,0x96, 0x81,0x79,0x00,0x98,
  };

  if(string == NULL)
	  return NULL;

  for(unsigned int x = 0; x < length; ++x)
  {
    if((unsigned char)string[x] > 128)
    {
      for(int i = 0; i < 256; ++i)
      {
        if((unsigned char)string[x] == oem2ansi_tab[i])
        {
          string[x] = (char)i;
          break;
        }
      }
    }
  }
  return string;
}
