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

///////////////////////////////////////////////////////////////////////////////
// Header
#include "main.h" // IWYU pragma: keep
#include "ArchivItem_Sound.h"
#include "libsiedler2.h"
#include "EndianStream.h"
#include <cstring>

// Include last!
#include "DebugNew.h" // IWYU pragma: keep

///////////////////////////////////////////////////////////////////////////////
/** @class libsiedler2::ArchivItem_Sound
 *
 *  Basisklasse für Sounditems.
 *
 *  @author FloSoft
 */

///////////////////////////////////////////////////////////////////////////////
/**
 *  Konstruktor von @p ArchivItem_Sound.
 *
 *  @author FloSoft
 */
libsiedler2::baseArchivItem_Sound::baseArchivItem_Sound(void) : ArchivItem()
{
    setBobType(BOBTYPE_SOUND);
    setType(SOUNDTYPE_NONE);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  virtueller Destruktor von @p ArchivItem_Sound.
 *
 *  @author FloSoft
 */
libsiedler2::baseArchivItem_Sound::~baseArchivItem_Sound(void)
{
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  setzt den Typ des Sounds.
 *
 *  @param[in] type Der Typ der gesetzt werden soll
 *
 *  @author FloSoft
 */
void libsiedler2::baseArchivItem_Sound::setType(SOUNDTYPES type)
{
    this->type_ = type;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  liefert den Typ des Sounds.
 *
 *  @return Der eingestellte Typ
 *
 *  @author FloSoft
 */
libsiedler2::SOUNDTYPES libsiedler2::baseArchivItem_Sound::getType(void) const
{
    return type_;
}

libsiedler2::baseArchivItem_Sound* libsiedler2::baseArchivItem_Sound::findSubType(std::istream& file)
{
    libendian::LittleEndianIStreamRef fs(file);
    long oldpos = fs.getPosition();
    baseArchivItem_Sound* item = NULL;

    char header[4];
    unsigned int length;

    // Header einlesen
    fs >> header;

    // ist es eine RIFF-File? (Header "FORM" bzw "RIFF")
    if(strncmp(header, "FORM", 4) == 0 || strncmp(header, "RIFF", 4) == 0)
    {
        // Länge einlesen
        fs >> length;

        // Typ einlesen
        fs >> header;

        if(strncmp(header, "XMID", 4) == 0 || strncmp(header, "XDIR", 4) == 0)
        {
            // xmidi
            item = dynamic_cast<baseArchivItem_Sound*>(getAllocator().create(BOBTYPE_SOUND, SOUNDTYPE_XMIDI));
        }
        else if(strncmp(header, "WAVE", 4) == 0)
        {
            // wave-format inkl-header
            item = dynamic_cast<baseArchivItem_Sound*>(getAllocator().create(BOBTYPE_SOUND, SOUNDTYPE_WAVE));
        }
    }
    else if(strncmp(header, "MThd", 4) == 0)
    {
        // midi
        item = dynamic_cast<baseArchivItem_Sound*>(getAllocator().create(BOBTYPE_SOUND, SOUNDTYPE_MIDI));
    }
    else if(strncmp(header, "OggS", 4) == 0 || strncmp(header, "ID3", 3) == 0 || ((unsigned char)header[0] == 0xFF && (unsigned char)header[1] == 0xFB) )
    {
        // ogg, mp3 (id3tag, ohne),
        item = dynamic_cast<baseArchivItem_Sound*>(getAllocator().create(BOBTYPE_SOUND, SOUNDTYPE_OTHER));
    }
    else
    {
        // wave-format ohne header?
        item = dynamic_cast<baseArchivItem_Sound*>(getAllocator().create(BOBTYPE_SOUND, SOUNDTYPE_WAVE));
    }

    fs.setPosition(oldpos);
    return item;
}
