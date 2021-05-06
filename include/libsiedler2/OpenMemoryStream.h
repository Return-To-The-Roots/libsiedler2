// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <boost/filesystem/path.hpp>
#include <boost/iostreams/device/mapped_file.hpp>
#include <boost/iostreams/stream.hpp>

namespace libsiedler2 {
using MMStream = boost::iostreams::stream<boost::iostreams::mapped_file_source>;

/// Open the given memory stream from a file and return an ErrorCode
/// Writes exceptions to stderr
int openMemoryStream(const boost::filesystem::path& filepath, MMStream& stream);
} // namespace libsiedler2
