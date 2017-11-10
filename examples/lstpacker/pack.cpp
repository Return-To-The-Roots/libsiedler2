// Copyright (c) 2005 - 2017 Settlers Freaks (sf-team at siedler25.org)
//
// This file is part of Siedler II.5 RTTR.
//
// Siedler II.5 RTTR is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// Siedler II.5 RTTR is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Siedler II.5 RTTR. If not, see <http://www.gnu.org/licenses/>.

#include "pack.h"
#include "util.h"
#include "libSiedler2/FileEntry.h"
#include "libsiedler2/Archiv.h"
#include "libsiedler2/ArchivItem_Bitmap.h"
#include "libsiedler2/ArchivItem_BitmapBase.h"
#include "libsiedler2/ArchivItem_Bitmap_Player.h"
#include "libsiedler2/ArchivItem_Font.h"
#include "libsiedler2/ErrorCodes.h"
#include "libsiedler2/IAllocator.h"
#include "libsiedler2/PixelBufferARGB.h"
#include "libsiedler2/libsiedler2.h"
#include "libutil/StringConversion.h"
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <algorithm>
#include <cctype>
#include <iostream>
#include <vector>

namespace libsiedler2 {
class ArchivItem_Palette;
}
namespace bfs = boost::filesystem;

using namespace std;
using namespace libsiedler2;

void pack(const std::string& directory, const std::string& file, const libsiedler2::ArchivItem_Palette* palette)
{
    Archiv lst;

    cout << "Reading directory: " << std::flush;
    vector<FileEntry> files = ReadFolderInfo(directory);
    cout << "done" << endl;

    // Not really required but for output below
    cout << "Sorting directory (this can take some time!): " << std::flush;
    sort(files.begin(), files.end());
    cout << "done" << endl;

    cout << "Listing files to pack:" << endl;
    unsigned ct = 0;
    BOOST_FOREACH(const FileEntry& entry, files)
    {
        if(entry.nr >= 0 && static_cast<unsigned>(entry.nr) > ct)
            ct = static_cast<unsigned>(entry.nr);
        cout << "[" << ct << "]: " << entry.filePath << endl;
    }

    cout << "Loading files " << std::flush;
    if(int ec = LoadFolder(files, lst, palette))
        cout << "Error: " << getErrorString(ec) << endl;
    else
        cout << "done" << endl;

    cout << "Writing data to " << file << ": ";
    if(int ec = Write(file, lst, palette))
        cout << "failed: " << getErrorString(ec) << endl;
    else
        cout << "done" << endl;
}
