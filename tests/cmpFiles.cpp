// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "cmpFiles.h"
#include "libsiedler2/ErrorCodes.h"
#include "libsiedler2/libsiedler2.h"
#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem/operations.hpp>

namespace bfs = boost::filesystem;

template<typename T_Func>
boost::test_tools::predicate_result testLoadWrite(T_Func func, int expectedResult,
                                                  const boost::filesystem::path& filepath, libsiedler2::Archiv& items,
                                                  const libsiedler2::ArchivItem_Palette* palette /*= NULL*/)
{
    int ec = func(filepath, items, palette);
    if(ec == expectedResult)
        return true;
    // LCOV_EXCL_START
    boost::test_tools::predicate_result result(false);
    result.message() << ec << "(" << libsiedler2::getErrorString(ec) << ") != " << expectedResult << "("
                     << libsiedler2::getErrorString(expectedResult) << ") for " << filepath;
    return result;
    // LCOV_EXCL_STOP
}

boost::test_tools::predicate_result testLoad(int expectedResult, const boost::filesystem::path& filepath,
                                             libsiedler2::Archiv& items,
                                             const libsiedler2::ArchivItem_Palette* palette /*= NULL*/)
{
    return testLoadWrite(libsiedler2::Load, expectedResult, filepath, items, palette);
}

boost::test_tools::predicate_result testWrite(int expectedResult, const boost::filesystem::path& filepath,
                                              libsiedler2::Archiv& items,
                                              const libsiedler2::ArchivItem_Palette* palette /*= NULL*/)
{
    return testLoadWrite(libsiedler2::Write, expectedResult, filepath, items, palette);
}

boost::test_tools::predicate_result testFilesEqual(const boost::filesystem::path& fileToCheck,
                                                   const boost::filesystem::path& expectedFile)
{
    if(bfs::file_size(fileToCheck) != bfs::file_size(expectedFile))
    {
        boost::test_tools::predicate_result result(false);
        result.message() << fileToCheck << ": File size mismatch: " << bfs::file_size(fileToCheck)
                         << "!=" << bfs::file_size(expectedFile);
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
            result.message() << fileToCheck << ": Mismatch at pos " << pos << ": " << std::hex << unsigned(*b1)
                             << " != " << unsigned(*b2);
            return result;
        }
    }
    return true;
}
