// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "OpenMemoryStream.h"
#include "ErrorCodes.h"
#include <boost/filesystem/operations.hpp>
#include <iostream>

namespace bfs = boost::filesystem;

namespace libsiedler2 {
int openMemoryStream(const boost::filesystem::path& filepath, MMStream& stream)
{
    if(filepath.empty())
        return ErrorCode::INVALID_BUFFER;

    if(!bfs::exists(filepath))
        return ErrorCode::FILE_NOT_FOUND;

    try
    {
        stream.open(filepath);
    } catch(std::exception& e)
    {
        std::cerr << "Could not open " << filepath << ": " << e.what() << std::endl;
        return ErrorCode::FILE_NOT_ACCESSIBLE;
    }

    return (!stream) ? ErrorCode::FILE_NOT_ACCESSIBLE : ErrorCode::NONE;
}
} // namespace libsiedler2
