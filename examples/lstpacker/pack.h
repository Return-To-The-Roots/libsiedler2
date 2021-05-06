// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <boost/filesystem/path.hpp>

namespace libsiedler2 {

class Archiv;
class ArchivItem_Palette;

} // namespace libsiedler2

void pack(const boost::filesystem::path& directory, const boost::filesystem::path& outFilepath,
          const libsiedler2::ArchivItem_Palette* palette);
