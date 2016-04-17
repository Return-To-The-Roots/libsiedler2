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

#include "main.h" // IWYU pragma: keep
#include "ArchivItem_Sound.h"
#include "ArchivInfo.h"
#include "prototypen.h"
#include <boost/iostreams/device/mapped_file.hpp>
#include <boost/iostreams/stream.hpp>

// Include last!
#include "DebugNew.h" // IWYU pragma: keep

/**
 *  lädt eine Sounddatei in ein ArchivInfo. (midi, xmidi, wave)
 *
 *  @param[in]  file    Dateiname der Sounddatei
 *  @param[out] items   ArchivInfo-Struktur, welche gefüllt wird
 *
 *  @return Null bei Erfolg, ein Wert ungleich Null bei Fehler
 */
int libsiedler2::loader::LoadSND(const std::string& file, ArchivInfo& items)
{
    if(file.empty())
        return 1;

    // Datei zum lesen öffnen
    boost::iostreams::mapped_file_source mmapFile(file);
    typedef boost::iostreams::stream<boost::iostreams::mapped_file_source> MMStream;
    MMStream snd(mmapFile);

    // hat das geklappt?
    if(!snd)
        return 2;

    baseArchivItem_Sound* sound = baseArchivItem_Sound::findSubType(snd);

    if(!sound)
        return 3;

    size_t size = getIStreamSize(snd);
    if(sound->load(snd, static_cast<unsigned>(size)) != 0)
        return 4;

    items.clear();
    items.push(sound);

    return 0;
}
