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

#pragma once

#include "enumTypes.h"
#include <boost/pointer_cast.hpp>
#include <memory>

namespace libsiedler2 {
class ArchivItem;

/**
 *  @brief Abstract ArchivItem factory
 */
class IAllocator
{
public:
    virtual ~IAllocator() = default;
    virtual std::unique_ptr<ArchivItem> create(BobType type, SoundType subtype = SoundType::None) const = 0;
    template<class T>
    std::unique_ptr<T> create(BobType type, SoundType subtype = SoundType::None) const
    {
        return boost::dynamic_pointer_cast<T>(create(type, subtype));
    }
};

} // namespace libsiedler2
