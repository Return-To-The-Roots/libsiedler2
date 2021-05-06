// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

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
