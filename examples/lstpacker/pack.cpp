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

#include "pack.h"
#include "util.h"

#include "libsiedler2/Archiv.h"
#include "libsiedler2/ErrorCodes.h"
#include "libsiedler2/FileEntry.h"
#include "libsiedler2/libsiedler2.h"

#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>

#include <algorithm>
#include <iostream>
#include <vector>

using namespace libsiedler2;

void pack(const std::string& directory, const std::string& file, const libsiedler2::ArchivItem_Palette* palette)
{
    Archiv lst;

    std::cout << "Reading directory: " << std::flush;
    std::vector<FileEntry> files = ReadFolderInfo(directory);
    std::cout << "done" << std::endl;

    // Not really required but for output below
    std::cout << "Sorting directory (this can take some time!): " << std::flush;
    std::sort(files.begin(), files.end());
    std::cout << "done" << std::endl;

    std::cout << "Listing files to pack:" << std::endl;
    unsigned ct = 0;
    for(const FileEntry& entry : files)
    {
        if(entry.nr >= 0 && static_cast<unsigned>(entry.nr) > ct)
            ct = static_cast<unsigned>(entry.nr);
        std::cout << "[" << ct << "]: " << entry.filePath << std::endl;
    }

    std::cout << "Loading files " << std::flush;
    if(int ec = LoadFolder(files, lst, palette))
    {
        std::cout << "Error: " << getErrorString(ec) << std::endl;
        return;
    } else
        std::cout << "done" << std::endl;

    std::cout << "Writing data to " << file << ": ";
    if(int ec = Write(file, lst, palette))
        std::cout << "failed: " << getErrorString(ec) << std::endl;
    else
        std::cout << "done" << std::endl;
}
