// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "Archiv.h"
#include "ArchivItem_Sound.h"
#include "ErrorCodes.h"
#include "GetIStreamSize.h"
#include "OpenMemoryStream.h"
#include "prototypen.h"

/**
 *  lädt eine Sounddatei in ein Archiv. (midi, xmidi, wave)
 *
 *  @param[in]  filepath    Dateiname der Sounddatei
 *  @param[out] items   Archiv-Struktur, welche gefüllt wird
 *
 *  @return Null bei Erfolg, ein Wert ungleich Null bei Fehler
 */
int libsiedler2::loader::LoadSND(const boost::filesystem::path& filepath, Archiv& items)
{
    MMStream snd;
    if(int ec = openMemoryStream(filepath, snd))
        return ec;

    auto sound = ArchivItem_Sound::findSubType(snd);

    if(!sound)
        return ErrorCode::WRONG_HEADER;

    size_t size = getIStreamSize(snd);
    if(int ec = sound->load(snd, static_cast<uint32_t>(size)))
        return ec;

    items.clear();
    items.push(std::move(sound));

    return ErrorCode::NONE;
}
