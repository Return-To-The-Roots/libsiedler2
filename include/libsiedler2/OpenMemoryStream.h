// Copyright (c) 2005 - 2017 Settlers Freaks (sf-team at siedler25.org)
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

#ifndef OpenMemoryStream_h__
#define OpenMemoryStream_h__

#include <boost/iostreams/device/mapped_file.hpp>
#include <boost/iostreams/stream.hpp>
#include <string>

namespace libsiedler2 {
using MMStream = boost::iostreams::stream<boost::iostreams::mapped_file_source>;

/// Open the given memory stream from a file and return an ErrorCode
/// Writes exceptions to stderr
int openMemoryStream(const std::string& file, MMStream& stream);
} // namespace libsiedler2

#endif // OpenMemoryStream_h__
