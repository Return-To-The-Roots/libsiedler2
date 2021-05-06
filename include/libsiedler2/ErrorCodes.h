// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <string>

namespace libsiedler2 {
/// Error code for reading/writing functions
struct ErrorCode
{
    enum EErrorCode
    {
        NONE = 0,            /// No error
        FILE_NOT_FOUND,      /// File does not exist
        FILE_NOT_ACCESSIBLE, /// File could not be opened
        WRONG_HEADER,        /// Header format did not match
        WRONG_FORMAT,        /// Format in the file did not match (e.g. from sub ids)
        WRONG_ARCHIV,        /// Archiv content does not match file format
        UNEXPECTED_EOF,      /// Encountered EOF/Read-/Write-Error while data was still to be read/written
        PALETTE_MISSING,     /// No palette given when loading/writing paletted image
        INVALID_BUFFER,      /// Buffer was invalid/ missing
        UNSUPPORTED_FORMAT,  /// Format not (yet) supported
        CUSTOM = 0x1000      /// Other errors can be signaled by returning CUSTOM + x
    };
};

std::string getErrorString(int errorCode);
} // namespace libsiedler2
