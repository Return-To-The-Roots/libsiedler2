// $Id: ConvertOemToAnsi.cpp 6581 2010-07-16 11:16:34Z FloSoft $
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
#include "ConvertOemToAnsi.h"

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#if defined _WIN32 && defined _DEBUG && defined _MSC_VER
	#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

/// Oem --> Ansi konvertieren
static const unsigned char OEM_TO_ANSI[256] =
{
	/*       0    1    2    3     4    5    6    7     8    9    A    B     C    D    E    F  */
	/* 0 */ 0x00,0x01,0x02,0x03, 0x04,0x05,0x06,0x07, 0x08,0x09,0x0a,0x0b, 0x0c,0xd0,0x0e,0x0f,
	/* 1 */ 0x10,0x11,0x12,0x13, 0x14,0x15,0x16,0x17, 0x18,0x19,0x1a,0x1b, 0x1c,0x1d,0x1e,0x1f, 
	/* 2 */ 0x20,0x21,0x22,0x23, 0x24,0x25,0x26,0x27, 0x28,0x29,0x2a,0x2b, 0x2c,0x2d,0x2e,0x2f, 
	/* 3 */ 0x30,0x31,0x32,0x33, 0x34,0x35,0x36,0x37, 0x38,0x39,0x3a,0x3b, 0x3c,0x3d,0x3e,0x3f, 
	/* 4 */ 0x40,0x41,0x42,0x43, 0x44,0x45,0x46,0x47, 0x48,0x49,0x4a,0x4b, 0x4c,0x4d,0x4e,0x4f, 
	/* 5 */ 0x50,0x51,0x52,0x53, 0x54,0x55,0x56,0x57, 0x58,0x59,0x5a,0x5b, 0x5c,0x5d,0x5f,0x5e, 
	/* 6 */ 0x60,0x61,0x62,0x63, 0x64,0x65,0x66,0x67, 0x68,0x69,0x6a,0x6b, 0x6c,0x6d,0x6e,0x6f, 
	/* 7 */ 0x70,0x71,0x72,0x73, 0x74,0x75,0x76,0x77, 0x78,0x79,0x7a,0x7b, 0x7c,0x7d,0x7e,0x7f, 

	/* 8 */	 199, 252, 233, 226,  228, 224, 229, 231,  234, 235, 232, 239,  238, 236, 196, 197,
	/* 9 */	 201, 230, 198, 244,  246, 242, 251, 249,  255, 214, 220, 162,  163, 165,  80, 131,
	/* A */	 225, 237, 243, 250,  241, 209, 170, 186,  191, 172, 172, 189,  188, 161, 171, 187,
	/* B */	 166, 166, 166, 166,  166, 166, 166,  43,   43, 166, 166,  43,   43,  43,  43,  43,
	/* C */	  43,  45,  45,  43,   45,  43, 166, 166,   43,  43,  45,  45,  166,  45,  43,  45,
	/* D */	  45,  45,  45,  43,   43,  43,  43,  43,   43,  43,  43, 166,   95, 166, 166, 175,
	/* E */	  97, 223,  71, 112,   83, 115, 181, 116,   70,  84,  79, 100,   56, 102, 101, 110,
	/* F */	  61, 177,  61,  61,   40,  41, 247, 152,  176, 183, 183, 118,  110, 178, 166, 160
};

///////////////////////////////////////////////////////////////////////////////
/** 
 *  Konvertiert einen OEM- in einen ANSI-String.
 *
 *  @author OLiver
 */
void libsiedler2::ConvertOemToAnsi(char *oem)
{
	if(!oem)
		return;

	size_t length = strlen(oem);

	for(size_t i = 0; i < length; ++i)
		oem[i] = OEM_TO_ANSI[static_cast<unsigned char>(oem[i])];
}
