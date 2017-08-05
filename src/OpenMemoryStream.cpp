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

#include "libSiedler2Defines.h" // IWYU pragma: keep
#include "OpenMemoryStream.h"
#include "ErrorCodes.h"
#include <boost/filesystem/operations.hpp>
#include <iostream>

namespace libsiedler2 {
int openMemoryStream(const std::string& file, MMStream& stream)
{
    if(file.empty())
        return ErrorCode::INVALID_BUFFER;

    if(!bfs::exists(file))
        return ErrorCode::FILE_NOT_FOUND;

    try
    {
        stream.open(bfs::path(file));
    } catch(std::exception& e)
    {
        std::cerr << "Could not open '" << file << "': " << e.what() << std::endl;
        return ErrorCode::FILE_NOT_ACCESSIBLE;
    }

    return (!stream) ? ErrorCode::FILE_NOT_ACCESSIBLE : ErrorCode::NONE;
}
} // namespace libsiedler2
