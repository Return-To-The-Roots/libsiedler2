// Copyright (c) 2005 - 2015 Settlers Freaks (sf-team at siedler25.org)
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

#include "libSiedler2Defines.h" // IWYU pragma: keep
#include "ArchivItem_Sound_XMidi.h"
#include <fstream>
#include "libendian/src/EndianIStreamAdapter.h"
#include "libendian/src/EndianOStreamAdapter.h"
#include <cstring>

namespace libsiedler2{

/** @class baseArchivItem_Sound_XMidi
 *
 *  Basisklasse für XMIDI-Sounds.
 */

baseArchivItem_Sound_XMidi::baseArchivItem_Sound_XMidi() : baseArchivItem_Sound()
{
    setType(SOUNDTYPE_XMIDI);

    tracks = 0;
}

baseArchivItem_Sound_XMidi::baseArchivItem_Sound_XMidi(const baseArchivItem_Sound_XMidi& item) : baseArchivItem_Sound( item )
{
    tracks = item.tracks;
    tracklist = item.tracklist;
}

baseArchivItem_Sound_XMidi::~baseArchivItem_Sound_XMidi()
{
}

baseArchivItem_Sound_XMidi& baseArchivItem_Sound_XMidi::operator=(const baseArchivItem_Sound_XMidi& item)
{
    if(this == &item)
        return *this;
    baseArchivItem_Sound::operator=(item);
    tracks = item.tracks;
    tracklist = item.tracklist;
    return *this;
}

int baseArchivItem_Sound_XMidi::load(std::istream& file, unsigned length)
{
    if(!file || length == 0)
        return 1;

    libendian::EndianIStreamAdapter<true, std::istream &> fs(file);
    unsigned item_length = length;
    long position = fs.getPosition();

    char header[4], subheader[4];
    unsigned chunk;

    // Header einlesen
    fs >> header;

    // ist es eine XMIDI-File? (Header "FORM")
    if(strncmp(header, "FORM", 4) != 0)
        return 3;

    // Länge einlesen
    fs >> length;

    // Typ einlesen
    fs >> subheader;

    // ist es eine singleTrack-XMIDI-File? (Typ "XMID")
    if(strncmp(subheader, "XMID", 4) == 0)
    {
        tracks = 1;
    }
    else if(strncmp(subheader, "XDIR", 4) == 0)
    {
        fs >> subheader;
        if(strncmp(subheader, "INFO", 4) != 0)
            return 4;
        // Länge einlesen
        fs >> length;
        // Bei ungerader Zahl aufrunden
        if(length & 1)
            ++length;
        if(length != sizeof(tracks))
            return 8;

        libendian::EndianIStreamAdapter<false, std::istream&> fsLE(file);
        fsLE >> tracks;

        fs >> subheader;
        if(strncmp(subheader, "CAT ", 4) != 0)
            return 5;
        fs.ignore(4);
        fs >> subheader;
        if(strncmp(subheader, "XMID", 4) != 0)
            return 12;
    }
    else
        return 13;

    if(tracks == 0 || tracks > 256)
        return 14;

    unsigned short track_nr = 0;
    while(track_nr < tracks)
    {
        // Chunk-Typ einlesen
        fs >> chunk;

        switch(chunk)
        {
            case 0x464F524D: // "FORM"
            {
                fs.ignore(4);
            } break;
            case 0x584D4944: // "XMID"
            {
            } break;
            case 0x54494D42: // "TIMB"
            {
                // Länge einlesen
                fs >> length;

                // Bei ungerader Zahl aufrunden
                if(length & 1)
                    ++length;

                fs.ignore(length);
            } break;
            case 0x45564E54: // "EVNT"
            {
                // Länge einlesen
                 fs >> length;

                // Bei ungerader Zahl aufrunden
                if(length & 1)
                    ++length;

                if(tracklist[track_nr].readXMid(file, length) != 0)
                    return 18;

                if(tracklist[track_nr].XMid2Mid() != 0)
                    return 19;

                ++track_nr;
            } break;
        }
    }

    // auf jeden Fall kompletten Datensatz überspringen
    fs.setPosition(position + item_length);
    return (!file) ? 99 : 0;
}

int baseArchivItem_Sound_XMidi::write(std::ostream& file) const
{
    if(!file)
        return 1;

    unsigned length = 0;
    for(unsigned short i = 0; i < tracks; ++i)
        length += tracklist[i].getMidLength(false);
    libendian::EndianOStreamAdapter<true, std::ostream&> fs(file);
    libendian::EndianOStreamAdapter<false, std::ostream&> fsLE(file);

    // LST-Länge schreiben (Little Endian!)
    fsLE << (length + 14);

    // Header schreiben
    fs.write("MThd", 4);

    // Länge schreiben
    fs << length;

    // Typ schreiben
    fs << uint16_t(0);

    // Tracksanzahl schreiben
    fs << tracks;

    // PPQS schreiben
    fs << uint16_t(96);

    for(unsigned short i = 0; i < tracks; ++i)
    {
        fs.write(tracklist[i].getMid(false), tracklist[i].getMidLength(false));
    }

    return (!file) ? 99 : 0;
}

} // namespace libsiedler2
