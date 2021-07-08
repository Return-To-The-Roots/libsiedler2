// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "ErrorCodes.h"

std::string libsiedler2::getErrorString(int errorCode)
{
    switch(static_cast<ErrorCode::EErrorCode>(errorCode))
    {
        case ErrorCode::NONE: return "None";
        case ErrorCode::FILE_NOT_FOUND: return "File was not found";
        case ErrorCode::FILE_NOT_ACCESSIBLE: return "File could not be opened";
        case ErrorCode::WRONG_HEADER: return "File had the wrong header. Possibly wrong file format";
        case ErrorCode::WRONG_FORMAT: return "File data did not match expected layout. Wrong file format?";
        case ErrorCode::WRONG_ARCHIVE: return "The archive contained data that could not be written to the file format";
        case ErrorCode::UNEXPECTED_EOF: return "Found end of file while there was still data to be read/written";
        case ErrorCode::PALETTE_MISSING: return "Palette is missing";
        case ErrorCode::INVALID_BUFFER: return "No or invalid buffer given";
        case ErrorCode::UNSUPPORTED_FORMAT: return "File format is not (yet) supported";
        default: return "Custom error (" + std::to_string(errorCode - ErrorCode::CUSTOM) + ")";
    }
}
