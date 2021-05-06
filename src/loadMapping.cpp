// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "loadMapping.h"
#include "s25util/StringConversion.h"
#include <boost/filesystem/path.hpp>
#include <boost/nowide/fstream.hpp>

namespace libsiedler2 {

void loadMapping(std::istream& stream, const std::function<void(unsigned idx, const std::string&)>& onItem)
{
    std::string line;
    const std::string delimiters(" \t");
    unsigned lineNr = 0;
    while(std::getline(stream, line))
    {
        ++lineNr;
        if(line.empty() || line[0] == '#')
            continue;
        try
        {
            const auto iDelimiter = line.find_first_of(delimiters);
            if(iDelimiter == std::string::npos || iDelimiter == 0u)
                throw std::runtime_error("No index or value");
            const std::string sIdx = line.substr(0, iDelimiter);
            int idx;
            if(!s25util::tryFromStringClassic(sIdx, idx) || idx < 0)
                throw std::runtime_error("Invalid index: " + sIdx);
            line.erase(0, iDelimiter + 1);
            onItem(static_cast<unsigned>(idx), line);
        } catch(const std::exception& e)
        {
            throw std::runtime_error("Error at line " + std::to_string(lineNr) + ": " + e.what());
        }
    }
    if(!stream.eof())
        throw std::runtime_error("Error getting line");
}

void loadMapping(const boost::filesystem::path& filepath,
                 const std::function<void(unsigned idx, const std::string&)>& onItem)
{
    boost::nowide::ifstream f(filepath);
    loadMapping(f, onItem);
}

} // namespace libsiedler2
