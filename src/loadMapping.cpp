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

#include "loadMapping.h"
#include "s25util/StringConversion.h"
#include <boost/filesystem/path.hpp>
#include <boost/nowide/fstream.hpp>

namespace libsiedler2 {

void loadMapping(std::istream& stream, std::function<void(unsigned idx, const std::string&)> onItem)
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

void loadMapping(const boost::filesystem::path& filepath, std::function<void(unsigned idx, const std::string&)> onItem)
{
    boost::nowide::ifstream f(filepath);
    loadMapping(f, onItem);
}

} // namespace libsiedler2
