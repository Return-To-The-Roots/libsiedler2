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
#include "FileEntry.h"
#include <ctype.h>

bool libsiedler2::operator<(const FileEntry& left, const FileEntry& right)
{
    if(left.nr < 0 || right.nr < 0)
    {
        // If any has a number, it is first
        if(left.nr >= 0)
            return true;
        if(right.nr >= 0)
            return false;
    } else if(left.nr < right.nr)
        return true;
    else if(left.nr > right.nr)
        return false;
    // Both negative or same
    for(std::string::const_iterator lit = left.name.begin(), rit = right.name.begin(); lit != left.name.end() && rit != right.name.end();
        ++lit, ++rit)
        if(tolower(*lit) < tolower(*rit))
            return true;
        else if(tolower(*lit) > tolower(*rit))
            return false;
    return left.name.size() < right.name.size();
}
