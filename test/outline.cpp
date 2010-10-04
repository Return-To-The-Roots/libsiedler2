// $Id: outline.cpp 6460 2010-05-31 11:42:38Z FloSoft $
//
// Copyright (c) 2005-2009 Settlers Freaks (sf-team at siedler25.org)
//
// This file is part of Siedler II.5 RTTR.
//
// Siedler II.5 RTTR is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// Siedler II.5 RTTR is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Siedler II.5 RTTR. If not, see <http://www.gnu.org/licenses/>.

///////////////////////////////////////////////////////////////////////////////
// Header
#include <stdio.h>
#include <windows.h>

#define snprintf _snprintf

#include "../src/libsiedler2.h"

using namespace libsiedler2;
using namespace loader;

int main(int argc, char* argv[])
{
	ArchivInfo lst, bbm;

	LoadBBM("GFX/PALETTE/PAL5.BBM", &bbm);

	ArchivItem_Palette* palette = (ArchivItem_Palette*)bbm.get(0);

	int format = FORMAT_PALETTED;

	setTextureFormat((TEXTURFORMAT)format);

	LoadDATIDX("DATA/RESOURCE.DAT", palette, &lst);

	ArchivInfo to;
	unsigned int v = 0;
	for(unsigned int i = 0; i < lst.getCount(); ++i)
	{
		if(lst.get(i)->getBobType() == BOBTYPE_FONT)
		{
			ArchivItem_Font *font = (ArchivItem_Font *)lst.get(i);
			ArchivItem_Font out;

			out.alloc(font->getCount());
			out.setDx(font->getDx()+2);
			out.setDy(font->getDy()+2);

			for(unsigned int j = 0; j < font->getCount(); ++j)
			{
				ArchivItem_Bitmap_Player *c = (ArchivItem_Bitmap_Player *)font->get(j);
				ArchivItem_Bitmap_Player o;

				if(c == NULL)
					continue;

				unsigned short w = c->getWidth()+1, h = c->getHeight()+1;

				unsigned char *buffer = new unsigned char[w*h];
				memset(buffer, TRANSPARENT_INDEX, w*h);

				unsigned char color = 1;
				c->print(buffer, w, h, format, palette, color);
				c->print(buffer, w, h, format, palette, color, 2 );
				c->print(buffer, w, h, format, palette, color, 0, 2);
				c->print(buffer, w, h, format, palette, color, 2, 2 );

				for(unsigned short z = 0; z < w*h; ++z)
				{
					if(buffer[z] >= color && buffer[z] <= color+3)
						buffer[z] = 0;
				}
				c->print(buffer, w, h, format, palette, color, 1, 1);

				o.create(w, h, buffer, w, h, format, palette, color);

				out.setC(j, &o);

				delete[] buffer;
			}

			to.alloc_inc(1);
			to.setC(v++, &out);
		}
	}
	char file[512];
	snprintf(file, 512, "outline_fonts.lst");
	WriteLST(file, palette, &to);

	getchar();
	return 0;
}
