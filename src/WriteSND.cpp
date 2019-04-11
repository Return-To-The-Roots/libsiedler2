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

#include "Archiv.h"
#include "ArchivItem_Sound.h"
#include "ErrorCodes.h"
#include "prototypen.h"
#include <boost/nowide/fstream.hpp>

int libsiedler2::loader::WriteSND(const std::string& file, const Archiv& items)
{
    if(file.empty())
        return ErrorCode::INVALID_BUFFER;

    // Can only write single sounds
    if(items.size() != 1)
        return ErrorCode::WRONG_ARCHIV;

    const auto* snd = dynamic_cast<const ArchivItem_Sound*>(items[0]);
    if(!snd)
        return ErrorCode::WRONG_ARCHIV;

    boost::nowide::ofstream fs(file, std::ios_base::binary);
    if(!fs)
        return ErrorCode::FILE_NOT_ACCESSIBLE;

    return snd->write(fs);
}
