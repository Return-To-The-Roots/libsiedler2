// $Id: unpack.cpp 9359 2014-04-25 15:37:22Z FloSoft $
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
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <windows.h>

#define snprintf _snprintf

#include "../../src/libsiedler2.h"

using namespace std;
using namespace libsiedler2;
using namespace loader;

void unpack(const string& directory, const ArchivInfo& lst, const ArchivItem_Palette* palette)
{
    CreateDirectoryA(directory.c_str(), NULL);

    for(unsigned int i = 0; i < lst.getCount(); ++i)
    {
        const ArchivItem* item = lst.get(i);

        bool changed = false;
        stringstream newfile;
        newfile << directory << "\\" << i << ".";

        if(!item)
        {
            /*newfile << "empty";

            FILE *n = fopen(newfile.str().c_str(), "wb");
            if(n)
                fclose(n);*/
            continue;
        }
        switch(item->getBobType())
        {
            case BOBTYPE_SOUND: // WAVs, MIDIs
            {
                unsigned short subtype = SOUNDTYPE_NONE;

                const ArchivItem_Sound* i = dynamic_cast<const ArchivItem_Sound*>(item);
                if(item)
                    subtype = i->getType();

                switch(subtype)
                {
                    case SOUNDTYPE_NONE:
                    {
                    } break;
                    case SOUNDTYPE_MIDI: // MIDI
                    {
                    } break;
                    case SOUNDTYPE_WAVE: // WAV
                    {
                        newfile << "wav";

                        cout << "extracting " << newfile.str() << ": ";

                        const ArchivItem_Sound_Wave* wave = dynamic_cast<const ArchivItem_Sound_Wave*>(item);
                        FILE* fwave = fopen(newfile.str().c_str(), "wb");
                        if(fwave && wave && wave->write(fwave, false) == 0)
                        {
                            fclose(fwave);
                            cout << "done" << endl;
                        }
                        else
                            cout << "failed" << endl;
                    } break;
                    case SOUNDTYPE_XMIDI: // XMIDI
                    {
                    } break;
                    case SOUNDTYPE_OTHER: // Andere
                    {
                    } break;
                }
            } break;
            case BOBTYPE_FONT: // Font
            {
                cout << "extracting " << newfile.str() << ": ";

                const ArchivItem_Font* font = dynamic_cast<const ArchivItem_Font*>(item);

                newfile << "dx" << (short)font->getDx() << ".dy" << (short)font->getDy() << ".";
                newfile << "fon";

                unpack(newfile.str(), *font, palette);
            } break;
            case BOBTYPE_PALETTE: // Palette
            {
                ArchivInfo items;
                items.pushC(item);
                newfile << "bbm";

                cout << "extracting " << newfile.str() << ": ";

                if(WriteBBM(newfile.str().c_str(), &items) != 0)
                    cout << "failed" << endl;
                else
                    cout << "done" << endl;
            } break;
            case BOBTYPE_BOB: // Bobfiles
            {
                const ArchivItem_Bob* bob = dynamic_cast<const ArchivItem_Bob*>(item);
                unpack(directory, bob, palette);
            } break;
            case BOBTYPE_MAP: // Mapfiles
            {
            } break;
            case BOBTYPE_TEXT: // Text
            {
            } break;
            case BOBTYPE_RAW: // Raw-Item
            {
            } break;
            case BOBTYPE_MAP_HEADER: // Mapheader-Item
            {
            } break;
            case BOBTYPE_BITMAP_RLE: // RLE komprimiertes Bitmap
            {
                if(!changed)
                {
                    newfile << "rle.";
                    changed = true;
                }
            }
            case BOBTYPE_BITMAP_PLAYER: // Bitmap mit spezifischer Spielerfarbe
            {
                if(!changed)
                {
                    newfile << "player.";
                    changed = true;
                }
            }
            case BOBTYPE_BITMAP_SHADOW: // Schatten
            {
                if(!changed)
                {
                    newfile << "shadow.";
                    changed = true;
                }
            }
            case BOBTYPE_BITMAP_RAW: // unkomprimiertes Bitmap
            {
                const ArchivItem_Bitmap* bitmap = dynamic_cast<const ArchivItem_Bitmap*>(item);
                ArchivInfo items;
                items.pushC(bitmap);
                newfile << "nx" << bitmap->getNx() << ".ny" << bitmap->getNy() << ".";
                newfile << "bmp";

                cout << "extracting " << newfile.str() << ": ";

                if(WriteBMP(newfile.str().c_str(), palette, &items) != 0)
                    cout << "failed" << endl;
                else
                    cout << "done" << endl;
            } break;
        }
    }
}