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

#include "ArchivItem_PaletteAnimation.h"
#include "ArchivItem_Palette.h"
#include "ErrorCodes.h"
#include "libendian/EndianIStreamAdapter.h"
#include "libendian/EndianOStreamAdapter.h"
#include <iostream>

namespace libsiedler2 {
ArchivItem_PaletteAnimation::ArchivItem_PaletteAnimation()
    : ArchivItem(BobType::PaletteAnim), isActive(false), moveUp(true), rate(16384), firstClr(0), lastClr(0)
{}

int ArchivItem_PaletteAnimation::load(std::istream& file)
{
    if(!file)
        return ErrorCode::FILE_NOT_ACCESSIBLE;
    libendian::EndianIStreamAdapter<true, std::istream&> fs(file);
    uint16_t padding;
    uint16_t flags;
    if(!(fs >> padding >> rate >> flags >> firstClr >> lastClr))
        return ErrorCode::UNSUPPORTED_FORMAT;
    isActive = (flags & 1) != 0;
    moveUp = (flags & 2) != 0;
    // S2 ignores flags and is always a moveUp
    if(rate)
        isActive = moveUp = true;
    return ErrorCode::NONE;
}

int ArchivItem_PaletteAnimation::write(std::ostream& file) const
{
    if(!file)
        return ErrorCode::FILE_NOT_ACCESSIBLE;
    libendian::EndianOStreamAdapter<true, std::ostream&> fs(file);
    uint16_t flags = (isActive ? 1 : 0) + (moveUp ? 2 : 0);
    if(!(fs << uint16_t(0) << rate << flags << firstClr << lastClr))
        return ErrorCode::UNEXPECTED_EOF;
    return ErrorCode::NONE;
}

int ArchivItem_PaletteAnimation::loadFromTxt(std::istream& file)
{
    if(!file)
        return ErrorCode::FILE_NOT_ACCESSIBLE;
    std::string sActive, sMoveUp;
    uint32_t firstClrTmp, lastClrTmp;
    if(!(file >> rate >> sActive >> sMoveUp >> firstClrTmp >> lastClrTmp))
        return ErrorCode::UNSUPPORTED_FORMAT;
    firstClr = firstClrTmp;
    lastClr = lastClrTmp;
    if(sActive == "active")
        isActive = true;
    else if(sActive == "inactive")
        isActive = false;
    else
        return ErrorCode::UNSUPPORTED_FORMAT;
    if(sMoveUp == "forwards")
        moveUp = true;
    else if(sMoveUp == "backwards")
        moveUp = false;
    else
        return ErrorCode::UNSUPPORTED_FORMAT;
    return ErrorCode::NONE;
}

int ArchivItem_PaletteAnimation::writeToTxt(std::ostream& file) const
{
    if(!file)
        return ErrorCode::FILE_NOT_ACCESSIBLE;
    std::string sActive = isActive ? "active" : "inactive";
    std::string sMoveUp = moveUp ? "forwards" : "backwards";
    if(!(file << rate << '\t' << sActive << '\t' << sMoveUp << '\t' << uint32_t(firstClr) << '\t' << uint32_t(lastClr) << '\n'))
        return ErrorCode::UNSUPPORTED_FORMAT;
    return ErrorCode::NONE;
}

std::unique_ptr<ArchivItem_Palette> ArchivItem_PaletteAnimation::apply(const ArchivItem_Palette& pal) const
{
    auto result = libsiedler2::clone(pal);
    if(isActive && firstClr < lastClr)
    {
        if(moveUp)
        {
            ColorRGB lastClrSave = result->get(lastClr);
            for(unsigned i = firstClr; i < lastClr; i++)
                result->set(i + 1, result->get(i));
            result->set(firstClr, lastClrSave);
        } else
        {
            ColorRGB firstClrSave = result->get(firstClr);
            for(unsigned i = firstClr; i < lastClr; i++)
                result->set(i, result->get(i + 1));
            result->set(lastClr, firstClrSave);
        }
    }
    return result;
}

} // namespace libsiedler2
