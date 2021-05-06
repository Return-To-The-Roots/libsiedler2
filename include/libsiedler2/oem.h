// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <string>

namespace libsiedler2 {
/// Wandelt einen String vom ANSI ins OEM Format um.
std::string AnsiToOem(const std::string& from);

/// Wandelt einen String vom OEM ins ANSI Format um.
std::string OemToAnsi(const std::string& from);
} // namespace libsiedler2
