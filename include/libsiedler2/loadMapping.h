// Copyright (c) 2005 - 2020 Settlers Freaks (sf-team at siedler25.org)
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

#ifndef loadMapping_h__
#define loadMapping_h__

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
void loadMapping(std::istream& stream, std::function<void(unsigned idx, const std::string&)> onItem);
/// Load a mapping from file. Call the callback for each item
void loadMapping(const boost::filesystem::path& filepath, std::function<void(unsigned idx, const std::string&)> onItem);

} // namespace libsiedler2

#endif // loadMapping_h__