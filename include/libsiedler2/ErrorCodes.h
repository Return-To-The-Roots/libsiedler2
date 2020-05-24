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

#ifndef ErrorCodes_h__
#define ErrorCodes_h__

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

#endif // ErrorCodes_h__
