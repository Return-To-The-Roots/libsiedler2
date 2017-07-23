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
#include "libsiedler2/src/libsiedler2.h"
#include "libsiedler2/src/ArchivInfo.h"
#include "libsiedler2/src/ArchivItem_Font.h"
#include "libsiedler2/src/ArchivItem_Bitmap_Player.h"
#include "libsiedler2/src/ArchivItem_Palette.h"
#include <cstdio>
#include <vector>

using namespace libsiedler2;

int main(int argc, char* argv[])
{
    ArchivInfo lst, bbm;

    Load("GFX/PALETTE/PAL5.BBM", bbm);

    ArchivItem_Palette* palette = (ArchivItem_Palette*)bbm.get(0);

    TEXTURFORMAT format = FORMAT_PALETTED;

    setTextureFormat(format);

    Load("DATA/RESOURCE.DAT", lst, palette);

    ArchivInfo to;
    for(unsigned i = 0; i < 4; ++i)
    {
        if(lst.get(i)->getBobType() == BOBTYPE_FONT)
        {
            ArchivItem_Font& font = dynamic_cast<ArchivItem_Font&>(*lst.get(i));

            // copy small font only
            if(i == 2)
            {
                to.pushC(font);
                continue;
            }

            ArchivItem_Font out;

            out.alloc(font.size());
            out.setDx(font.getDx() + 2);
            out.setDy(font.getDy() + 2);

            for(unsigned j = 0; j < font.size(); ++j)
            {
                ArchivItem_Bitmap_Player* c = dynamic_cast<ArchivItem_Bitmap_Player*>(font.get(j));
                ArchivItem_Bitmap_Player o;

                if(c == NULL)
                    continue;

                unsigned short w = c->getWidth() + 1, h = c->getHeight() + 1;

                std::vector<unsigned char> buffer(w * h, TRANSPARENT_INDEX);

                unsigned char color = 1;
                c->print(&buffer.front(), w, h, format, palette, color);
                c->print(&buffer.front(), w, h, format, palette, color, 2 );
                c->print(&buffer.front(), w, h, format, palette, color, 0, 2);
                c->print(&buffer.front(), w, h, format, palette, color, 2, 2 );

                for(unsigned short z = 0; z < w * h; ++z)
                {
                    if(buffer[z] >= color && buffer[z] <= color + 3)
                        buffer[z] = 0;
                }
                c->print(&buffer.front(), w, h, format, palette, color, 1, 1);

                o.create(w, h, &buffer.front(), w, h, format, palette, color);

                out.setC(j, o);
            }

            to.pushC(out);
        }
    }
    char file[512];
    snprintf(file, 512, "outline_fonts.lst");
    Write(file, to, palette);

    getchar();
    return 0;
}
