// Copyright (c) 2017 - 2017 Settlers Freaks (sf-team at siedler25.org)
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

#include <string>

const std::string testOutputPath = "testOutput";

namespace boost { namespace filesystem {
}} // namespace boost::filesystem
namespace bfs = boost::filesystem;

#define RTTR_REQUIRE_EQUAL_COLLECTIONS(col1, col2) BOOST_REQUIRE_EQUAL_COLLECTIONS(col1.begin(), col1.end(), col2.begin(), col2.end())

#endif // config_h__
