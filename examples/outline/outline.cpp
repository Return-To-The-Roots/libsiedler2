// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "libsiedler2/Archiv.h"
#include "libsiedler2/ArchivItem_Bitmap_Player.h"
#include "libsiedler2/ArchivItem_Font.h"
#include "libsiedler2/ArchivItem_Palette.h"
#include "libsiedler2/libsiedler2.h"

#include <cstdio>
#include <vector>

using namespace libsiedler2;

int main(int /*argc*/, char* /*argv*/[])
{
    Archiv lst, bbm;

    Load("GFX/PALETTE/PAL5.BBM", bbm);

    auto* palette = (ArchivItem_Palette*)bbm.get(0);

    Load("DATA/RESOURCE.DAT", lst, palette);

    Archiv to;
    for(unsigned i = 0; i < 4; ++i)
    {
        if(lst.get(i)->getBobType() == BobType::Font)
        {
            auto& font = dynamic_cast<ArchivItem_Font&>(*lst.get(i));

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
                auto* c = dynamic_cast<ArchivItem_Bitmap_Player*>(font.get(j));
                ArchivItem_Bitmap_Player o;

                if(!c)
                    continue;

                unsigned short w = c->getWidth() + 1, h = c->getHeight() + 1;

                PixelBufferPaletted buffer(w, h);

                unsigned char color = 1;
                c->print(buffer, palette, color);
                c->print(buffer, palette, color, 2);
                c->print(buffer, palette, color, 0, 2);
                c->print(buffer, palette, color, 2, 2);

                for(unsigned short z = 0; z < w * h; ++z)
                {
                    if(buffer.get(z) >= color && buffer.get(z) <= color + 3)
                        buffer.set(z, 0);
                }
                c->print(buffer, palette, color, 1, 1);

                o.create(buffer, palette, color);

                out.setC(j, o);
            }

            to.pushC(out);
        }
    }
    std::array<char, 512> file;
    snprintf(file.data(), file.size(), "outline_fonts.lst");
    Write(file.data(), to, palette);

    getchar();
    return 0;
}
