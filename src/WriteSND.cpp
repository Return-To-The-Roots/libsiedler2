// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "Archiv.h"
#include "ArchivItem_Sound.h"
#include "ErrorCodes.h"
#include "prototypen.h"
#include <boost/nowide/fstream.hpp>

int libsiedler2::loader::WriteSND(const boost::filesystem::path& filepath, const Archiv& items)
{
    if(filepath.empty())
        return ErrorCode::INVALID_BUFFER;

    // Can only write single sounds
    if(items.size() != 1)
        return ErrorCode::WRONG_ARCHIVE;

    const auto* snd = dynamic_cast<const ArchivItem_Sound*>(items[0]);
    if(!snd)
        return ErrorCode::WRONG_ARCHIVE;

    boost::nowide::ofstream fs(filepath, std::ios_base::binary);
    if(!fs)
        return ErrorCode::FILE_NOT_ACCESSIBLE;

    return snd->write(fs);
}
