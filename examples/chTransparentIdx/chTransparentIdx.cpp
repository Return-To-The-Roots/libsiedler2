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

#include "libsiedler2/Archiv.h"
#include "libsiedler2/ArchivItem_Palette.h"
#include "libsiedler2/ErrorCodes.h"
#include "libsiedler2/libsiedler2.h"
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <boost/nowide/args.hpp>
#include <boost/nowide/integration/filesystem.hpp>
#include <boost/nowide/iostream.hpp>
#include <boost/program_options.hpp>
#include <iostream>
#include <string>
#include <vector>

namespace bfs = boost::filesystem;
namespace bpo = boost::program_options;
namespace bnw = boost::nowide;

int main(int argc, char* argv[])
{
    bnw::args(argc, argv);
    bnw::nowide_filesystem();

    bpo::options_description desc("Usage:\n"
                                  "unpack: chTransparentIdx <file.lst>\n"
                                  "Optionally pass a color palette file (bbm/act) to use instead of default one");
    desc.add_options()("help,h", "Show help")("file,f", bpo::value<std::vector<bfs::path> >()->multitoken(),
                                              "File(s) to repack")("palette,p", bpo::value<bfs::path>(), "Palette to use")(
      "from", bpo::value<uint8_t>()->default_value(254), "Original transparent color index")("to", bpo::value<uint8_t>()->default_value(0),
                                                                                             "New transparent color index");
    bpo::positional_options_description positionalOptions;
    positionalOptions.add("file", -1);

    bpo::variables_map options;
    bpo::store(bpo::command_line_parser(argc, argv).options(desc).positional(positionalOptions).run(), options);
    bpo::notify(options);

    if(options.count("help") || !options.count("file"))
    {
        bnw::cout << desc << std::endl;
        return 1;
    }

    libsiedler2::Archiv bbm;

    if(options.count("palette"))
    {
        bfs::path palPath = options["palette"].as<bfs::path>();
        if(libsiedler2::Load(palPath.string(), bbm) != 0)
        {
            bnw::cerr << "Error: Could not load given palette: " << palPath << std::endl;
            bnw::cerr << "Retrying with default ones" << std::endl;
        } else
            bnw::cout << "Using non-standard color palette: " << palPath << std::endl;
    }

    bfs::path pal5Path("GFX/PALETTE/PAL5.BBM");
    bfs::path pal5Path2("pal5.act");

    if(bbm.empty() && (!bfs::exists(pal5Path) || Load(pal5Path.string(), bbm) != 0))
    {
        if(Load(pal5Path2.string(), bbm) != 0)
        {
            bnw::cerr << "Fatal Error: " << std::endl;
            bnw::cerr << "Neither " << pal5Path << " nor " << pal5Path2 << " was found or it cannot be opened" << std::endl;
            return 2;
        }
    }

    libsiedler2::ArchivItem_Palette* palette = (libsiedler2::ArchivItem_Palette*)bbm[0];

    BOOST_FOREACH(const bfs::path& inputPath, options["file"].as<std::vector<bfs::path> >())
    {
        if(!bfs::exists(inputPath))
        {
            bnw::cerr << "Input file or folder does not exist: " << inputPath;
            return 1;
        }

        if(!inputPath.has_extension())
        {
            bnw::cerr << "Input filepath has no extension: " << inputPath;
            return 1;
        }
        bfs::path outPath = inputPath;
        outPath.replace_extension(".new" + inputPath.extension().string());

        bnw::cout << "Repacking file " << inputPath << " to " << outPath << std::endl;

        palette->setTransparentIdx(options["from"].as<uint8_t>());
        libsiedler2::setGlobalTextureFormat(libsiedler2::FORMAT_BGRA);
        libsiedler2::Archiv lst;
        if(int ec = Load(inputPath.string(), lst, palette))
        {
            bnw::cerr << "Fatal Error during reading: " << libsiedler2::getErrorString(ec) << std::endl;
            return 3;
        }
        palette->setTransparentIdx(options["to"].as<uint8_t>());
        if(int ec = Write(outPath.string(), lst, palette))
        {
            bnw::cerr << "Fatal Error during writing: " << libsiedler2::getErrorString(ec) << std::endl;
            return 4;
        }
    }

    bnw::cout << "done" << std::endl;
    bnw::cout << "press enter to exit" << std::endl;
    bnw::cin.get();
    return 0;
}
