// $Id: util.h 9359 2014-04-25 15:37:22Z FloSoft $
//
// Copyright (c) 2005-2009 Settlers Freaks (sf-team at siedler25.org)
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
#ifndef UTIL_H_INCLUDED
#define UTIL_H_INCLUDED

#pragma once

#include <sstream>

inline std::vector<std::string> explode(std::string const& line, const char delim, const unsigned int max = 0xFFFFFFFF)
{
    std::istringstream in(line);
    std::vector<std::string> result;
    std::string token;

    unsigned int len = 0;
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
