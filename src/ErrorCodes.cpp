// Copyright (c) 2005 - 2015 Settlers Freaks (sf-team at siedler25.org)
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

#include "ErrorCodes.h"
#include <boost/lexical_cast.hpp>

std::string libsiedler2::getErrorString(int errorCode)
{
    switch(static_cast<ErrorCode::EErrorCode>(errorCode))
    {
    case ErrorCode::NONE:
        return "None";
    case ErrorCode::FILE_NOT_FOUND:
        return "File was not found";
    case ErrorCode::FILE_NOT_ACCESSIBLE:
        return "File could not be opened";
    case ErrorCode::WRONG_HEADER:
        return "File had the wrong header. Possibly wrong file format";
    case ErrorCode::WRONG_FORMAT:
        return "File data did not match expected layout. Wrong file format?";
    case ErrorCode::WRONG_ARCHIV:
        return "The archiv contained data that could not be written to the file format";
    case ErrorCode::UNEXPECTED_EOF:
        return "Found end of file while there was still data to be read/written";
    case ErrorCode::PALETTE_MISSING:
        return "Palette is missing";
    case ErrorCode::INVALID_BUFFER:
        return "No or invalid buffer given";
    case ErrorCode::UNSUPPORTED_FORMAT:
        return "File format is not (yet) supported";
    default:
        return "Custom error (" + boost::lexical_cast<std::string>(errorCode) + ")";
    }
}
