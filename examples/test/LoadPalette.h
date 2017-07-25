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

#ifndef LoadPalette_h__
#define LoadPalette_h__

#include "libsiedler2/src/libsiedler2.h"
#include "libsiedler2/src/ArchivInfo.h"
#include "libsiedler2/src/ArchivItem_Palette.h"
#include <boost/test/unit_test.hpp>

struct LoadPalette
{
    libsiedler2::ArchivInfo paletteArchiv;
    libsiedler2::ArchivItem_Palette* palette;
    LoadPalette()
    {
        BOOST_REQUIRE_EQUAL(libsiedler2::Load("pal5.act", paletteArchiv), 0);
        palette = dynamic_cast<libsiedler2::ArchivItem_Palette*>(paletteArchiv.get(0));
        BOOST_REQUIRE(palette);
    }
};

#endif // LoadPalette_h__
