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
#include "ArchivItem.h"

/** @class libsiedler2::ArchivItem
 *
 *  Basisklasse der Archivelemente.
 */

/** @var libsiedler2::ArchivItem::bobtype
 *
 *  Bobtype des Elements.
 */

/** @var libsiedler2::ArchivItem::name
 *
 *  Name des Elements.
 */

libsiedler2::ArchivItem::ArchivItem()
{
    bobtype_ = BOBTYPE_NONE;
    setName("untitled");
}

libsiedler2::ArchivItem::~ArchivItem()
{
}
