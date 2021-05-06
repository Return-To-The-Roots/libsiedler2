// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
// Copyright (C) 2005 - 2021 Settlers Freaks <sf-team at siedler25.org>
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "IAllocator.h"

namespace libsiedler2 {
class ArchivItem;

class StandardAllocator : public IAllocator
{
public:
    ~StandardAllocator() override = default;
    std::unique_ptr<ArchivItem> create(BobType type, SoundType subtype = SoundType::None) const override;
};

} // namespace libsiedler2
