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

#pragma once

#ifndef config_h__
#define config_h__

#include <boost/filesystem/path.hpp>

#cmakedefine RTTR_LIBSIEDLER2_TEST_FILES_DIR "@RTTR_LIBSIEDLER2_TEST_FILES_DIR@"
#cmakedefine RTTR_LIBSIEDLER2_S2_DIR "@RTTR_LIBSIEDLER2_S2_DIR@"

namespace libsiedler2
{
namespace test
{
#ifdef RTTR_LIBSIEDLER2_S2_DIR
const boost::filesystem::path s2Path =  RTTR_LIBSIEDLER2_S2_DIR;
constexpr bool hasS2Data = true;
#else
// Proprietary files (from S2) required. Set RTTR_LIBSIEDLER2_S2_DIR cmake variable to enable more tests
constexpr bool hasS2Data = false;
const boost::filesystem::path s2Path;
#endif

const boost::filesystem::path inputPath = RTTR_LIBSIEDLER2_TEST_FILES_DIR;
const boost::filesystem::path outputPath = "@PROJECT_BINARY_DIR@/testOutput";
}
}

#endif // config_h__
