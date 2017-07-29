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
#include "ArchivItem_Sound.h"
#include "ArchivInfo.h"
#include "prototypen.h"
#include <boost/filesystem/fstream.hpp>

int libsiedler2::loader::WriteSND(const std::string& file, const ArchivInfo& items)
{
    if(file.empty())
        return 1;

    // Can only write single sounds
    if(items.size() != 1)
        return 2;

    const ArchivItem_Sound* sound = dynamic_cast<const ArchivItem_Sound*>(items[0]);
    if(!sound)
        return 3;

    bfs::ofstream fs(file, std::ios_base::binary);
    if(!fs)
        return 3;

    if(sound->write(fs) != 0)
        return 4;

    // alles ok
    return 0;
}
