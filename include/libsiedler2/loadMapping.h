// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <boost/filesystem/path.hpp>
#include <functional>
#include <iosfwd>
#include <string>

namespace libsiedler2 {

// Mapping file format:
// '#' at the start of a line denotes a comment
// Each data line is in format <idx><space><item>
// <idx> has to be an unsigned integer with the index where the value will be put
// <space> us either a single space or tab
// <value> is file format specific

/// Load a mapping from stream. Call the callback for each item
void loadMapping(std::istream& stream, const std::function<void(unsigned idx, const std::string&)>& onItem);
/// Load a mapping from file. Call the callback for each item
void loadMapping(const boost::filesystem::path& filepath,
                 const std::function<void(unsigned idx, const std::string&)>& onItem);

} // namespace libsiedler2
