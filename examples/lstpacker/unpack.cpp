// Copyright (c) 2005 - 2017 Settlers Freaks (sf-team at siedler25.org)
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

#include "unpack.h"
#include "libsiedler2/Archiv.h"
#include "libsiedler2/ArchivItem_BitmapBase.h"
#include "libsiedler2/ArchivItem_Bob.h"
#include "libsiedler2/ArchivItem_Font.h"
#include "libsiedler2/ArchivItem_Palette.h"
#include "libsiedler2/ArchivItem_Sound.h"
#include "libsiedler2/ArchivItem_Sound_Midi.h"
#include "libsiedler2/ArchivItem_Sound_Wave.h"
#include "libsiedler2/ArchivItem_Sound_XMidi.h"
#include "libsiedler2/ArchivItem_Text.h"
#include "libsiedler2/libsiedler2.h"
#include "libsiedler2/prototypen.h"
#include <boost/filesystem.hpp>
#include <boost/nowide/fstream.hpp>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>

using namespace std;
using namespace libsiedler2;
namespace bnw = boost::nowide;
namespace bfs = boost::filesystem;

void unpack(const std::string& directory, const libsiedler2::Archiv& lst, const libsiedler2::ArchivItem_Palette* palette,
            const std::string& fileNameHexPrefix, bool paletteAsTxt)
{
    boost::filesystem::create_directories(directory);
    bool containsPalAnim = false;

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
        std::string newFileBaseName = newfile.str();

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
                    case SOUNDTYPE_NONE: cerr << "Unsupported sound ignored: " << newFileBaseName << endl; break;
                    case SOUNDTYPE_MIDI: // MIDI
                        cerr << "Unsupported midi sound ignored: " << newFileBaseName << endl;
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
                    {
                        newfile << "midi";

                        cout << "extracting " << newfile.str() << ": ";

                        auto wave = clone(dynamic_cast<const ArchivItem_Sound_XMidi&>(*item));
                        const MIDI_Track& midiTrack = wave->getMidiTrack(0);
                        ArchivItem_Sound_Midi soundArchiv;
                        soundArchiv.addTrack(midiTrack);
                        soundArchiv.setPPQ(wave->getPPQN());
                        std::ofstream fwave(newfile.str().c_str(), ios::binary);
                        if(fwave && soundArchiv.write(fwave) == 0)
                            cout << "done";
                        else
                            cout << "failed";
                        cout << endl;
                        break;
                    }
                    case SOUNDTYPE_OTHER: // Andere
                        cerr << "Unsupported other sound ignored: " << newFileBaseName << endl;
                        break;
                }
            }
            break;
            case BOBTYPE_FONT: // Font
            {
                cout << "extracting " << newFileBaseName << ": ";

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
                if(paletteAsTxt)
                    loader::WriteTxtPalette(newFileBaseName + "palette.txt", static_cast<const libsiedler2::ArchivItem_Palette&>(*item));
            }
            break;
            case BOBTYPE_BOB: // Bobfiles
            {
                const ArchivItem_Bob* bob = dynamic_cast<const ArchivItem_Bob*>(item);
                unpack(directory, *bob, palette);
                // links[][8][2][6]
                bnw::ofstream linksFile(directory + ".links");
                for(unsigned i = 0; i < bob->getNumItems(); i++)
                {
                    if(i % (8 * 2 * 6) == 0)
                        linksFile << "Job ID " << i / (8 * 2 * 6) << std::endl;
                    linksFile << i << ": " << bob->getLink(i) << std::endl;
                }
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
                cerr << "Raw item is not supported. Ignored: " << newFileBaseName << endl;
                break;
            case BOBTYPE_MAP_HEADER: // Mapheader-Item
                cerr << "Map-header is not supported. Ignored: " << newFileBaseName << endl;
                break;
            case BOBTYPE_BITMAP_RLE: // RLE komprimiertes Bitmap
                if(!filenameAdjusted)
                {
                    newfile << "rle.";
                    filenameAdjusted = true;
                }
                BOOST_FALLTHROUGH;
                // no break
            case BOBTYPE_BITMAP_PLAYER: // Bitmap mit spezifischer Spielerfarbe
                if(!filenameAdjusted)
                {
                    newfile << "player.";
                    filenameAdjusted = true;
                }
                BOOST_FALLTHROUGH;
                // no break
            case BOBTYPE_BITMAP_SHADOW: // Schatten
                if(!filenameAdjusted)
                {
                    newfile << "shadow.";
                    filenameAdjusted = true;
                }
                BOOST_FALLTHROUGH;
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
                if(bitmap.getPalette() && (paletteAsTxt || (*bitmap.getPalette() != *palette)))
                    loader::WriteTxtPalette(newFileBaseName + "palette.txt", *bitmap.getPalette());
            }
            break;
            case BOBTYPE_PALETTE_ANIM: containsPalAnim = true; break;
            default: cerr << "Unhandled bobtype: " << item->getBobType() << endl;
        }
    }

    if(containsPalAnim)
    {
        bfs::path newfile(directory);
        newfile /= "paletteAnims.txt";

        cout << "extracting " << newfile.string() << ": ";

        if(Write(newfile.string().c_str(), lst) != 0)
            cout << "failed" << endl;
        else
            cout << "done" << endl;
    }
}
