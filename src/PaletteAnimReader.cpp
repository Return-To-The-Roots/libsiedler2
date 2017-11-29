// Copyright (c) 2005 - 2017 Settlers Freaks (sf-team at siedler25.org)
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

#include "libSiedler2Defines.h" // IWYU pragma: keep
#include "Archiv.h"
#include "ArchivItem_PaletteAnimation.h"
#include "ErrorCodes.h"
#include "IAllocator.h"
#include "libsiedler2.h"
#include "prototypen.h"
#include "libutil/StringConversion.h"
#include <boost/interprocess/smart_ptr/unique_ptr.hpp>
#include <boost/nowide/fstream.hpp>

namespace libsiedler2 { namespace loader {
    static const std::string palAnimHeader = "Palette animations V1";

    int LoadPaletteAnim(const std::string& file, Archiv& items)
    {
        s25util::ClassicImbuedStream<bnw::ifstream> fs(file);
        if(!fs)
            return ErrorCode::FILE_NOT_ACCESSIBLE;
        std::string header;
        if(!std::getline(fs, header) || header != palAnimHeader)
            return ErrorCode::WRONG_HEADER;
        while(fs)
        {
            unsigned idx;
            if(!(fs >> idx))
            {
                if(fs.eof())
                    return ErrorCode::NONE;
                else
                    return ErrorCode::WRONG_FORMAT;
            }
            boost::interprocess::unique_ptr<ArchivItem_PaletteAnimation, Deleter<ArchivItem_PaletteAnimation> > anim(
              dynamic_cast<ArchivItem_PaletteAnimation*>(getAllocator().create(BOBTYPE_PALETTE_ANIM)));
            if(int ec = anim->loadFromTxt(fs))
                return ec;
            if(idx >= items.size())
                items.alloc_inc(idx - items.size() + 1);
            items.set(idx, anim.release());
        }
        return ErrorCode::UNEXPECTED_EOF;
    }

    int WritePaletteAnim(const std::string& file, const Archiv& items)
    {
        s25util::ClassicImbuedStream<bnw::ofstream> fs(file);
        if(!fs)
            return ErrorCode::FILE_NOT_ACCESSIBLE;
        fs << palAnimHeader << std::endl;
        for(unsigned idx = 0; idx < items.size(); idx++)
        {
            if(!items[idx])
                continue;
            if(items[idx]->getBobType() != BOBTYPE_PALETTE_ANIM)
                continue;
            fs << idx << '\t';
            if(int ec = dynamic_cast<const ArchivItem_PaletteAnimation*>(items[idx])->writeToTxt(fs))
                return ec;
        }
        return ErrorCode::NONE;
    }
}} // namespace libsiedler2::loader
