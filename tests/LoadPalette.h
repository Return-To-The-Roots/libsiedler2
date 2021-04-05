// Copyright (c) 2017 - 2020 Settlers Freaks (sf-team at siedler25.org)
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

#include "ColorOutput.h"
#include "test/config.h"
#include "libsiedler2/Archiv.h"
#include "libsiedler2/ArchivItem_Palette.h"
#include "libsiedler2/libsiedler2.h"
#include <boost/test/unit_test.hpp>

struct LoadPalette
{
    libsiedler2::ArchivItem_Palette* palette;
    libsiedler2::ArchivItem_Palette* modPal;
    static constexpr uint8_t modClr1 = 22;
    static constexpr uint8_t modClr2 = 44;
    explicit LoadPalette()
    {
        BOOST_TEST_REQUIRE(libsiedler2::Load(libsiedler2::test::inputPath / "pal5.act", paletteArchiv) == 0);
        palette = dynamic_cast<libsiedler2::ArchivItem_Palette*>(paletteArchiv.get(0));
        BOOST_REQUIRE(palette);
        paletteArchiv.pushC(*palette);
        modPal = dynamic_cast<libsiedler2::ArchivItem_Palette*>(paletteArchiv.get(1));
        BOOST_TEST_REQUIRE(modPal);
        BOOST_TEST_REQUIRE(modPal->get(modClr1) != modPal->get(modClr1 + 1));
        BOOST_TEST_REQUIRE(modPal->get(modClr2) != modPal->get(modClr2 + 1));
        libsiedler2::ColorRGB clr = modPal->get(modClr1);
        modPal->set(modClr1, modPal->get(modClr1 + 1));
        modPal->set(modClr1 + 1, clr);
        clr = modPal->get(modClr2);
        modPal->set(modClr2, modPal->get(modClr2 + 1));
        modPal->set(modClr2 + 1, clr);
    }

private:
    libsiedler2::Archiv paletteArchiv;
};
