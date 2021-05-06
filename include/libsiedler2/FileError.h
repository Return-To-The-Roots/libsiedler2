// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <stdexcept>

namespace libsiedler2 {
/// Exception raised in functions not returning an ErrorCode
class FileError : public std::runtime_error
{
public:
    const int ec;
    explicit FileError(int ec) : std::runtime_error(nullptr), ec(ec) {}
    const char* what() const noexcept override;
};
} // namespace libsiedler2
