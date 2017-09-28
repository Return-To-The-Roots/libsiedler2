// Copyright (c) 2005 - 2017 Settlers Freaks (sf-team at siedler25.org)
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

#include "unpack.h"
#include "libsiedler2/Archiv.h"
#include "libsiedler2/ArchivItem_BitmapBase.h"
#include "libsiedler2/ArchivItem_Bob.h"
#include "libsiedler2/ArchivItem_Font.h"
#include "libsiedler2/ArchivItem_Sound.h"
#include "libsiedler2/ArchivItem_Sound_Wave.h"
#include "libsiedler2/ArchivItem_Text.h"
#include "libsiedler2/libsiedler2.h"
#include <boost/filesystem.hpp>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>

using namespace std;
using namespace libsiedler2;

void checkTxtExtraction(const string& directory, const Archiv& lst)
{
    for(unsigned i = 0; i < lst.size(); ++i)
    {
        const ArchivItem* item = lst.get(i);

        if(!item)
            continue;
        if(item->getBobType() != BOBTYPE_TEXT)
            return;
    }

    const std::string filePath = directory + ".summary.txt";

    cout << "extracting " << filePath.c_str() << ": ";
    std::ofstream fTxt(filePath.c_str(), ios::binary);

    for(unsigned i = 0; i < lst.size(); ++i)
    {
        const ArchivItem* item = lst.get(i);

        if(item)
        {
            const ArchivItem_Text* txt = dynamic_cast<const ArchivItem_Text*>(item);
            if(txt->write(fTxt, false) != 0)
                fTxt << " [error]";
            else if(!txt->getText().empty())
                fTxt.seekp(-1, std::ios_base::cur); // Remove NULL terminator
        }
        fTxt << char('\n');
    }
}

void unpack(const string& directory, const Archiv& lst, const ArchivItem_Palette* palette, const std::string& fileNameHexPrefix)
{
    boost::filesystem::create_directories(directory);

    for(unsigned i = 0; i < lst.size(); ++i)
    {
        const ArchivItem* item = lst[i];

        if(!item)
            continue;

        bool filenameAdjusted = false;
        stringstream newfile;
        newfile << directory << "/" << fileNameHexPrefix;
        if(!fileNameHexPrefix.empty())
            newfile << std::hex << std::setfill('0') << std::setw(4);
        newfile << i << std::dec << ".";

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
                    case SOUNDTYPE_NONE: cerr << "Unsupported sound ignored: " << newfile.str() << endl; break;
                    case SOUNDTYPE_MIDI: // MIDI
                        cerr << "Unsupported midi sound ignored: " << newfile.str() << endl;
                        break;
                    case SOUNDTYPE_WAVE: // WAV
                    {
                        newfile << "wav";

                        cout << "extracting " << newfile.str() << ": ";

                        const ArchivItem_Sound_Wave* wave = dynamic_cast<const ArchivItem_Sound_Wave*>(item);
                        std::ofstream fwave(newfile.str().c_str(), ios::binary);
                        if(fwave && wave && wave->write(fwave, false) == 0)
                        {
                            cout << "done" << endl;
                        } else
                            cout << "failed" << endl;
                    }
                    break;
                    case SOUNDTYPE_XMIDI: // XMIDI
                        cerr << "Unsupported xmidi sound ignored: " << newfile.str() << endl;
                        break;
                    case SOUNDTYPE_OTHER: // Andere
                        cerr << "Unsupported other sound ignored: " << newfile.str() << endl;
                        break;
                }
            }
            break;
            case BOBTYPE_FONT: // Font
            {
                cout << "extracting " << newfile.str() << ": ";

                const ArchivItem_Font* font = dynamic_cast<const ArchivItem_Font*>(item);

                newfile << "dx" << (short)font->getDx() << ".dy" << (short)font->getDy() << ".";
                if(font->isUnicode)
                    newfile << "fonX";
                else
                    newfile << "fon";

                unpack(newfile.str(), *font, palette, font->isUnicode ? "U+" : "");
            }
            break;
            case BOBTYPE_PALETTE: // Palette
            {
                Archiv items;
                items.pushC(*item);
                newfile << "bbm";

                cout << "extracting " << newfile.str() << ": ";

                if(Write(newfile.str().c_str(), items) != 0)
                    cout << "failed" << endl;
                else
                    cout << "done" << endl;
            }
            break;
            case BOBTYPE_BOB: // Bobfiles
            {
                const ArchivItem_Bob* bob = dynamic_cast<const ArchivItem_Bob*>(item);
                unpack(directory, *bob, palette);
            }
            break;
            case BOBTYPE_MAP: // Mapfiles
                cerr << "MapFile is not supported. Ignored: " << newfile.str() << endl;
                break;
            case BOBTYPE_TEXT: // Text
            {
                newfile << "txt";
                cout << "extracting " << newfile.str() << ": ";

                const ArchivItem_Text* txt = dynamic_cast<const ArchivItem_Text*>(item);
                std::ofstream fTxt(newfile.str().c_str(), ios::binary);
                if(fTxt && txt && txt->write(fTxt, false) == 0)
                {
                    cout << "done" << endl;
                } else
                    cout << "failed" << endl;
            }
            break;
            case BOBTYPE_RAW: // Raw-Item
                cerr << "Raw item is not supported. Ignored: " << newfile.str() << endl;
                break;
            case BOBTYPE_MAP_HEADER: // Mapheader-Item
                cerr << "Map-header is not supported. Ignored: " << newfile.str() << endl;
                break;
            case BOBTYPE_BITMAP_RLE: // RLE komprimiertes Bitmap
                if(!filenameAdjusted)
                {
                    newfile << "rle.";
                    filenameAdjusted = true;
                }
            // no break
            case BOBTYPE_BITMAP_PLAYER: // Bitmap mit spezifischer Spielerfarbe
                if(!filenameAdjusted)
                {
                    newfile << "player.";
                    filenameAdjusted = true;
                }
            // no break
            case BOBTYPE_BITMAP_SHADOW: // Schatten
                if(!filenameAdjusted)
                {
                    newfile << "shadow.";
                    filenameAdjusted = true;
                }
            // no break
            case BOBTYPE_BITMAP_RAW: // unkomprimiertes Bitmap
            {
                Archiv items;
                const ArchivItem_BitmapBase& bitmap = dynamic_cast<const ArchivItem_BitmapBase&>(*item);
                items.pushC(bitmap);
                newfile << "nx" << bitmap.getNx() << ".ny" << bitmap.getNy();
                newfile << ".bmp";

                cout << "extracting " << newfile.str() << ": ";

                if(Write(newfile.str(), items, palette) != 0)
                    cout << "failed" << endl;
                else
                    cout << "done" << endl;
            }
            break;
            default: cerr << "Unhandled bobtype: " << item->getBobType() << endl;
        }
    }

    checkTxtExtraction(directory, lst);
}
