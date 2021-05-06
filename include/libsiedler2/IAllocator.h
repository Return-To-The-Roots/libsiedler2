// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

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
