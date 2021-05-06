// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "FileEntry.h"
#include "s25util/strAlgos.h"
#include <boost/range/combine.hpp>

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
        if(left.bobtype == BobType::Palette)
            return true;
        else if(right.bobtype == BobType::Palette)
            return false;
    }
    // Both negative or same
    for(const auto it : boost::combine(left.name, right.name))
    {
        auto const lhs = s25util::toLower(it.get<0>());
        auto const rhs = s25util::toLower(it.get<1>());
        if(lhs < rhs)
            return true;
        else if(lhs > rhs)
            return false;
    }
    return left.name.size() < right.name.size();
}
