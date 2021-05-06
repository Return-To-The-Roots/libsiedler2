// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <boost/filesystem/path.hpp>
#include <boost/test/unit_test.hpp>

namespace libsiedler2 {
class Archiv;
class ArchivItem_Palette;
} // namespace libsiedler2

boost::test_tools::predicate_result testLoad(int expectedResult, const boost::filesystem::path& filepath,
                                             libsiedler2::Archiv& items,
                                             const libsiedler2::ArchivItem_Palette* palette = nullptr);
boost::test_tools::predicate_result testWrite(int expectedResult, const boost::filesystem::path& filepath,
                                              libsiedler2::Archiv& items,
                                              const libsiedler2::ArchivItem_Palette* palette = nullptr);
boost::test_tools::predicate_result testFilesEqual(const boost::filesystem::path& fileToCheck,
                                                   const boost::filesystem::path& expectedFile);
