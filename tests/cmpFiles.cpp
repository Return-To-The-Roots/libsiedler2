// Copyright (c) 2017 - 2020 Settlers Freaks (sf-team at siedler25.org)
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

#include "cmpFiles.h"
#include "libsiedler2/ErrorCodes.h"
#include "libsiedler2/libsiedler2.h"
#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem/operations.hpp>

namespace bfs = boost::filesystem;

template<typename T_Func>
boost::test_tools::predicate_result testLoadWrite(T_Func func, int expectedResult, const std::string& file, libsiedler2::Archiv& items,
                                                  const libsiedler2::ArchivItem_Palette* palette /*= NULL*/)
{
    int ec = func(file, items, palette);
    if(ec == expectedResult)
        return true;
    boost::test_tools::predicate_result result(false);
    result.message() << libsiedler2::getErrorString(ec) << " != " << libsiedler2::getErrorString(expectedResult) << " for " << file;
    return result;
}

boost::test_tools::predicate_result testLoad(int expectedResult, const std::string& file, libsiedler2::Archiv& items,
                                             const libsiedler2::ArchivItem_Palette* palette /*= NULL*/)
{
    return testLoadWrite(libsiedler2::Load, expectedResult, file, items, palette);
}

boost::test_tools::predicate_result testWrite(int expectedResult, const std::string& file, libsiedler2::Archiv& items,
                                              const libsiedler2::ArchivItem_Palette* palette /*= NULL*/)
{
    return testLoadWrite(libsiedler2::Write, expectedResult, file, items, palette);
}

boost::test_tools::predicate_result testFilesEqual(const std::string& fileToCheck, const std::string& expectedFile)
{
    if(bfs::file_size(fileToCheck) != bfs::file_size(expectedFile))
    {
        boost::test_tools::predicate_result result(false);
        result.message() << fileToCheck << ": File size mismatch: " << bfs::file_size(fileToCheck) << "!=" << bfs::file_size(expectedFile);
        return result;
    }

    bfs::ifstream ifs1(fileToCheck, std::ios::binary);
    bfs::ifstream ifs2(expectedFile, std::ios::binary);
    ifs1.unsetf(std::ios::skipws);
    ifs2.unsetf(std::ios::skipws);

    size_t pos = 0;
    for(std::istream_iterator<unsigned char> b1(ifs1), b2(ifs2), endIt; b1 != endIt && b2 != endIt; ++b1, ++b2, ++pos)
    {
        if(*b1 != *b2)
        {
            boost::test_tools::predicate_result result(false);
            result.message() << fileToCheck << ": Mismatch at pos " << pos << ": " << std::hex << unsigned(*b1) << " != " << unsigned(*b2);
            return result;
        }
    }
    return true;
}
