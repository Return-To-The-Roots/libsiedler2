// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <s25util/boostTestHelpers.h>
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
