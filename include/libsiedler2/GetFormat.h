// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

namespace libsiedler2 { namespace traits {
    /// Return the texture/buffer format of the given type as the value member
    template<class T_PixelBuffer>
    struct GetFormat;
}} // namespace libsiedler2::traits
