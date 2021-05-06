// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "Archiv.h"
#include "ArchivItem_Palette.h"
#include "ArchivItem_PaletteAnimation.h"
#include "ErrorCodes.h"
#include "FileError.h"
#include "IAllocator.h"
#include "libsiedler2.h"
#include "loadMapping.h"
#include "prototypen.h"
#include "s25util/StringConversion.h"
#include <boost/nowide/fstream.hpp>
#include <iomanip>
#include <memory>

namespace bnw = boost::nowide;

namespace libsiedler2 { namespace loader {
    static const std::string txtPalHeader = "Bitmap palette V1";
    static const std::string palAnimHeader = "Palette animations V1";

    int LoadTxtPalette(const boost::filesystem::path& filepath, Archiv& items)
    {
        s25util::ClassicImbuedStream<bnw::ifstream> fs(filepath);
        if(!fs)
            return ErrorCode::FILE_NOT_ACCESSIBLE;
        std::string header;
        if(!std::getline(fs, header) || header != txtPalHeader)
            return ErrorCode::WRONG_HEADER;
        auto pal = getAllocator().create<ArchivItem_Palette>(BobType::Palette);
        items.alloc(1);
        std::string transparency, sColor;
        unsigned hasTransparency, transpColorIdx;
        if(!(fs >> transparency >> hasTransparency >> sColor >> transpColorIdx))
            return ErrorCode::WRONG_FORMAT;
        if(transparency != "Transparency:" || sColor != "Color:")
            return ErrorCode::WRONG_FORMAT;
        if(hasTransparency)
            pal->setTransparentIdx(transpColorIdx);
        else
            pal->setBackgroundColorIdx(transpColorIdx);
        try
        {
            loadMapping(fs, [&pal](unsigned idx, const std::string& sClr) {
                if(idx >= 256)
                    throw std::range_error("Palettes can only have 256 colors");
                const auto clr = s25util::fromStringClassic<unsigned>(sClr, true);
                pal->set(idx, ColorRGB(clr >> 16, clr >> 8, clr));
            });
        } catch(const std::runtime_error&)
        {
            return fs.eof() ? ErrorCode::UNEXPECTED_EOF : ErrorCode::WRONG_FORMAT;
        }
        return ErrorCode::NONE;
    }

    int WriteTxtPalette(const boost::filesystem::path& filepath, const ArchivItem_Palette& palette)
    {
        s25util::ClassicImbuedStream<bnw::ofstream> fs(filepath);
        if(!fs)
            return ErrorCode::FILE_NOT_ACCESSIBLE;
        fs << txtPalHeader << std::endl;
        fs << "Transparency: " << unsigned(palette.hasTransparency())
           << " Color: " << unsigned(palette.getTransparentIdx()) << std::endl;
        for(unsigned i = 0; i < 256; i++)
        {
            fs << std::setw(3) << std::setfill(' ') << i << "\t0x" << std::uppercase << std::hex << std::setfill('0')
               << std::setw(2) << unsigned(palette.get(i).getRed()) << std::setw(2)
               << unsigned(palette.get(i).getGreen()) << std::setw(2) << unsigned(palette.get(i).getBlue()) << std::dec
               << std::endl;
        }
        return ErrorCode::NONE;
    }

    int LoadPaletteAnim(const boost::filesystem::path& filepath, Archiv& items)
    {
        s25util::ClassicImbuedStream<bnw::ifstream> fs(filepath);
        if(!fs)
            return ErrorCode::FILE_NOT_ACCESSIBLE;
        std::string header;
        if(!std::getline(fs, header) || header != palAnimHeader)
            return ErrorCode::WRONG_HEADER;
        try
        {
            loadMapping(fs, [&items](unsigned idx, const std::string& sItem) {
                std::stringstream ss(sItem);
                auto anim = getAllocator().create<ArchivItem_PaletteAnimation>(BobType::PaletteAnim);
                if(int ec = anim->loadFromTxt(ss))
                    throw FileError(ec);
                if(idx >= items.size())
                    items.alloc_inc(idx - items.size() + 1);
                items.set(idx, std::move(anim));
            });
        } catch(const FileError& e)
        {
            return e.ec;
        } catch(const std::runtime_error&)
        {
            return fs.eof() ? ErrorCode::UNEXPECTED_EOF : ErrorCode::WRONG_FORMAT;
        }
        return ErrorCode::NONE;
    }

    int WritePaletteAnim(const boost::filesystem::path& filepath, const Archiv& items)
    {
        s25util::ClassicImbuedStream<bnw::ofstream> fs(filepath);
        if(!fs)
            return ErrorCode::FILE_NOT_ACCESSIBLE;
        fs << palAnimHeader << std::endl;
        for(unsigned idx = 0; idx < items.size(); idx++)
        {
            if(!items[idx])
                continue;
            if(items[idx]->getBobType() != BobType::PaletteAnim)
                continue;
            fs << idx << '\t';
            if(int ec = dynamic_cast<const ArchivItem_PaletteAnimation*>(items[idx])->writeToTxt(fs))
                return ec;
        }
        return ErrorCode::NONE;
    }
}} // namespace libsiedler2::loader
