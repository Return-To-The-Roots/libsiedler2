// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "pack.h"
#include "util.h"
#include "libsiedler2/Archiv.h"
#include "libsiedler2/ErrorCodes.h"
#include "libsiedler2/FileEntry.h"
#include "libsiedler2/libsiedler2.h"
#include <boost/filesystem.hpp>
#include <algorithm>
#include <iostream>
#include <vector>

using namespace libsiedler2;

void pack(const boost::filesystem::path& directory, const boost::filesystem::path& outFilepath,
          const libsiedler2::ArchivItem_Palette* palette)
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

    std::cout << "Writing data to " << outFilepath << ": ";
    if(int ec = Write(outFilepath, lst, palette))
        std::cout << "failed: " << getErrorString(ec) << std::endl;
    else
        std::cout << "done" << std::endl;
}
