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

#include "FileEntry.h"
#include <boost/range/combine.hpp>
#include <cctype>

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
    // Palettes come first
    if(left.bobtype != right.bobtype)
    {
        if(left.bobtype == BOBTYPE_PALETTE)
            return true;
        else if(right.bobtype == BOBTYPE_PALETTE)
            return false;
    }
    // Both negative or same
    for(const auto it : boost::combine(left.name, right.name))
    {
        auto const lhs = tolower(it.get<0>());
        auto const rhs = tolower(it.get<1>());
        if(lhs < rhs)
            return true;
        else if(lhs > rhs)
            return false;
    }
    return left.name.size() < right.name.size();
}
