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
#ifndef UTIL_H_INCLUDED
#define UTIL_H_INCLUDED

#pragma once

#include <sstream>
#include <string>
#include <vector>

inline std::vector<std::string> explode(std::string const& line, const char delim, const unsigned max = 0xFFFFFFFF)
{
    std::istringstream in(line);
    std::vector<std::string> result;
    std::string token;

    size_t len = 0;
    while(std::getline(in, token, delim) && result.size() < max - 1)
    {
        len += token.size() + 1;
        result.push_back(token);
    }

    if(len < in.str().length())
        result.push_back(in.str().substr(len));

    return result;
}

#endif // !UTIL_H_INCLUDED
