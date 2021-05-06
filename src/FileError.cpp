// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "FileError.h"
#include "ErrorCodes.h"

namespace libsiedler2 {
const char* FileError::what() const noexcept
{
    static std::string s;
    s = getErrorString(ec);
    return s.c_str();
}
} // namespace libsiedler2
