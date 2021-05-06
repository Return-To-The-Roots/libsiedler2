// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <boost/filesystem/path.hpp>
#include <string>

namespace libsiedler2 {

class Archiv;
class ArchivItem_Palette;

} // namespace libsiedler2

void unpack(const boost::filesystem::path& directory, const libsiedler2::Archiv& lst,
            const libsiedler2::ArchivItem_Palette* palette, const std::string& fileNameHexPrefix = "",
            bool paletteAsTxt = false);
