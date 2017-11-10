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

#ifndef FileEntry_h__
#define FileEntry_h__

#include "enumTypes.h"
#include <string>

namespace libsiedler2 {
struct FileEntry
{
    explicit FileEntry(const std::string& filePath) : filePath(filePath), nr(-1), bobtype(BOBTYPE_UNSET), nx(0), ny(0) {}
    /// Path to file and (remaining) name
    std::string filePath, name;
    /// Index in the archiv/folder
    int nr;
    BobType bobtype;
    /// Origin or delta (fonts)
    unsigned nx, ny;
};
bool operator<(const FileEntry& lhs, const FileEntry& rhs);
} // namespace libsiedler2

#endif // FileEntry_h__
