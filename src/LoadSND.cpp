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

#include "libSiedler2Defines.h" // IWYU pragma: keep
#include "Archiv.h"
#include "ArchivItem_Sound.h"
#include "ErrorCodes.h"
#include "OpenMemoryStream.h"
#include "prototypen.h"

/**
 *  lädt eine Sounddatei in ein Archiv. (midi, xmidi, wave)
 *
 *  @param[in]  file    Dateiname der Sounddatei
 *  @param[out] items   Archiv-Struktur, welche gefüllt wird
 *
 *  @return Null bei Erfolg, ein Wert ungleich Null bei Fehler
 */
int libsiedler2::loader::LoadSND(const std::string& file, Archiv& items)
{
    MMStream snd;
    if(int ec = openMemoryStream(file, snd))
        return ec;

    ArchivItem_Sound* sound = ArchivItem_Sound::findSubType(snd);

    if(!sound)
        return ErrorCode::WRONG_HEADER;

    size_t size = getIStreamSize(snd);
    if(int ec = sound->load(snd, static_cast<uint32_t>(size)))
        return ec;

    items.clear();
    items.push(sound);

    return ErrorCode::NONE;
}
