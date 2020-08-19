// Copyright (c) 2005 - 2020 Settlers Freaks (sf-team at siedler25.org)
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
//
// SPDX-License-Identifier: GPL-2.0-or-later

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
#include "s25util/StringConversion.h"

#include <boost/filesystem.hpp>
#include <boost/nowide/fstream.hpp>

#include <fstream>
#include <iostream>
#include <limits>
#include <vector>

#if BOOST_VERSION < 106500 && defined(BOOST_GCC) && __GNUC__ >= 7
#ifdef BOOST_FALLTHROUGH
#undef BOOST_FALLTHROUGH
#endif
#define BOOST_FALLTHROUGH __attribute__((fallthrough))
#endif

using namespace std;
using namespace libsiedler2;
namespace bnw = boost::nowide;
namespace bfs = boost::filesystem;

static std::string makeBaseFilename(unsigned idx, const std::string& fileNameHexPrefix)
{
    if(fileNameHexPrefix.empty())
        return s25util::toStringClassic(idx);
    std::string hexValue = (idx <= std::numeric_limits<uint16_t>::max()) ? s25util::toStringClassic(static_cast<uint16_t>(idx), true) :
                                                                           s25util::toStringClassic(idx, true);
    return fileNameHexPrefix + hexValue.substr(2); // Replace prefix
}

void unpack(const bfs::path& directory, const libsiedler2::Archiv& lst, const libsiedler2::ArchivItem_Palette* palette,
            const std::string& fileNameHexPrefix, bool paletteAsTxt)
{
    boost::filesystem::create_directories(directory);
    bool containsPalAnim = false;

    for(unsigned i = 0; i < lst.size(); ++i)
    {
        const ArchivItem* item = lst[i];

        if(!item)
            continue;

        const std::string newFileStem = makeBaseFilename(i, fileNameHexPrefix);
        bfs::path newFilepath = directory / newFileStem;

        switch(item->getBobType())
        {
            case BobType::Sound: // WAVs, MIDIs
            {
                auto subtype = SoundType::None;

                const auto* i = dynamic_cast<const ArchivItem_Sound*>(item);
                if(item)
                    subtype = i->getType();

                switch(subtype)
                {
                    case SoundType::Midi: cerr << "Unsupported midi sound ignored: " << newFileStem << endl; break;
                    case SoundType::Wave:
                    {
                        newFilepath.replace_extension(".wav");

                        cout << "extracting " << newFilepath << ": ";

                        const auto* wave = dynamic_cast<const ArchivItem_Sound_Wave*>(item);
                        bnw::ofstream fwave(newFilepath, ios::binary);
                        if(fwave && wave && wave->write(fwave, false) == 0)
                        {
                            cout << "done" << endl;
                        } else
                            cout << "failed" << endl;
                    }
                    break;
                    case SoundType::XMidi:
                    {
                        newFilepath.replace_extension(".midi");

                        cout << "extracting " << newFilepath << ": ";

                        auto wave = clone(dynamic_cast<const ArchivItem_Sound_XMidi&>(*item));
                        const MIDI_Track& midiTrack = wave->getMidiTrack(0);
                        ArchivItem_Sound_Midi soundArchiv;
                        soundArchiv.addTrack(midiTrack);
                        soundArchiv.setPPQ(wave->getPPQN());
                        bnw::ofstream fwave(newFilepath, ios::binary);
                        if(fwave && soundArchiv.write(fwave) == 0)
                            cout << "done";
                        else
                            cout << "failed";
                        cout << endl;
                        break;
                    }
                    default: cerr << "Unsupported other sound ignored: " << newFileStem << endl; break;
                }
            }
            break;
            case BobType::Font:
            {
                const auto* font = dynamic_cast<const ArchivItem_Font*>(item);

                newFilepath += ".dx" + s25util::toStringClassic((short)font->getDx());
                newFilepath += ".dy" + s25util::toStringClassic((short)font->getDy());
                newFilepath += (font->isUnicode) ? ".fonX" : ".fon";
                cout << "extracting " << newFilepath << ": ";

                unpack(newFilepath, *font, palette, font->isUnicode ? "U+" : "");
            }
            break;
            case BobType::Palette:
            {
                Archiv items;
                items.pushC(*item);
                newFilepath.replace_extension(".bbm");

                cout << "extracting " << newFilepath << ": ";

                if(Write(newFilepath, items) != 0)
                    cout << "failed" << endl;
                else
                    cout << "done" << endl;
                if(paletteAsTxt)
                    loader::WriteTxtPalette(newFilepath.replace_extension(".palette.txt"),
                                            static_cast<const libsiedler2::ArchivItem_Palette&>(*item));
            }
            break;
            case BobType::Bob:
            {
                const auto* bob = dynamic_cast<const ArchivItem_Bob*>(item);
                unpack(directory, *bob, palette);
                // links[][8][2][6]
                bnw::ofstream linksFile(directory / "mapping.links");
                bob->writeLinks(linksFile);
            }
            break;
            case BobType::Map: cerr << "MapFile is not supported. Ignored: " << newFileStem << endl; break;
            case BobType::Text:
            {
                newFilepath.replace_extension(".txt");
                cout << "extracting " << newFilepath << ": ";

                const auto* txt = dynamic_cast<const ArchivItem_Text*>(item);
                bnw::ofstream fTxt(newFilepath, ios::binary);
                if(fTxt && txt && txt->write(fTxt, false) == 0)
                {
                    cout << "done" << endl;
                } else
                    cout << "failed" << endl;
            }
            break;
            case BobType::Raw: cerr << "Raw item is not supported. Ignored: " << newFileStem << endl; break;
            case BobType::MapHeader: cerr << "Map-header is not supported. Ignored: " << newFileStem << endl; break;
            case BobType::BitmapRLE: // RLE komprimiertes Bitmap
                newFilepath.replace_extension(".rle");
                BOOST_FALLTHROUGH;
                // no break
            case BobType::BitmapPlayer: // Bitmap mit spezifischer Spielerfarbe
                if(!newFilepath.has_extension())
                    newFilepath.replace_extension(".player");
                BOOST_FALLTHROUGH;
                // no break
            case BobType::BitmapShadow:
                if(!newFilepath.has_extension())
                    newFilepath.replace_extension(".shadow");
                BOOST_FALLTHROUGH;
                // no break
            case BobType::Bitmap: // uncompressed Bitmap
            {
                Archiv items;
                const auto& bitmap = dynamic_cast<const ArchivItem_BitmapBase&>(*item);
                items.pushC(bitmap);

                newFilepath += ".nx" + s25util::toStringClassic(bitmap.getNx());
                newFilepath += ".ny" + s25util::toStringClassic(bitmap.getNy());
                newFilepath += ".bmp";

                cout << "extracting " << newFilepath << ": ";

                if(Write(newFilepath, items, palette) != 0)
                    cout << "failed" << endl;
                else
                    cout << "done" << endl;
                if(bitmap.getPalette() && (paletteAsTxt || (*bitmap.getPalette() != *palette)))
                    loader::WriteTxtPalette((directory / newFileStem).replace_extension(".palette.txt"), *bitmap.getPalette());
            }
            break;
            case BobType::PaletteAnim: containsPalAnim = true; break;
            default: cerr << "Unhandled bobtype: " << static_cast<unsigned>(item->getBobType()) << endl;
        }
    }

    if(containsPalAnim)
    {
        const bfs::path newfile = directory / "paletteAnims.txt";

        cout << "extracting " << newfile << ": ";

        if(Write(newfile, lst) != 0)
            cout << "failed" << endl;
        else
            cout << "done" << endl;
    }
}
