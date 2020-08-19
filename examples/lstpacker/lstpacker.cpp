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
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "pack.h"
#include "unpack.h"
#include "libsiedler2/Archiv.h"
#include "libsiedler2/ArchivItem_Palette.h"
#include "libsiedler2/libsiedler2.h"
#include <boost/filesystem.hpp>
#include <boost/nowide/args.hpp>
#include <boost/nowide/filesystem.hpp>
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
    bnw::args _(argc, argv);
    bnw::nowide_filesystem();

    std::string texFmt, outFileExt;

    bpo::options_description desc("Usage:\n"
                                  "pack:   lstpacker <directory>\n"
                                  "unpack: lstpacker <file.lst> <file.lst> ...\n");
    desc.add_options()("help,h", "Show help")("file,f", bpo::value<std::vector<std::string>>()->multitoken(),
                                              "File to unpack or directory to pack")("palette,p", bpo::value<std::string>(),
                                                                                     "Palette  (bbm/act) to use instead of default one")(
      "palAsTxt,t", "Output palettes as human readable txt files")(
      "texFmt", bpo::value<std::string>(&texFmt)->default_value("original"),
      "Texture format to use (usually equal output format): (o)riginal, (p)paletted, (B)GRA")(
      "outFileFmt,o", bpo::value<std::string>(&outFileExt)->default_value("LST"), "Output file format (extension)");
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
    if(texFmt == "original" || texFmt == "o")
        libsiedler2::setGlobalTextureFormat(libsiedler2::TextureFormat::Original);
    else if(texFmt == "paletted" || texFmt == "p")
        libsiedler2::setGlobalTextureFormat(libsiedler2::TextureFormat::Paletted);
    else if(texFmt == "BGRA" || texFmt == "B" || texFmt == "bgra" || texFmt == "b")
        libsiedler2::setGlobalTextureFormat(libsiedler2::TextureFormat::BGRA);
    else
        throw bpo::validation_error(bpo::validation_error::invalid_option_value, "texFmt");

    libsiedler2::Archiv bbm;

    if(options.count("palette"))
    {
        const auto palPath = options["palette"].as<std::string>();
        if(libsiedler2::Load(palPath, bbm) != 0)
        {
            bnw::cerr << "Error: Could not load given palette: " << palPath << std::endl;
            bnw::cerr << "Retrying with default ones" << std::endl;
        } else
            bnw::cout << "Using non-standard color palette: " << palPath << std::endl;
    }

    bfs::path pal5Path("GFX/PALETTE/PAL5.BBM");
    bfs::path pal5Path2("pal5.act");

    if(bbm.empty() && (!bfs::exists(pal5Path) || Load(pal5Path, bbm) != 0))
    {
        if(Load(pal5Path2, bbm) != 0)
        {
            bnw::cerr << "Fatal Error: " << std::endl;
            bnw::cerr << "Neither " << pal5Path << " nor " << pal5Path2 << " was found or it cannot be opened" << std::endl;
            return 2;
        }
    }

    auto* palette = dynamic_cast<libsiedler2::ArchivItem_Palette*>(bbm[0]);

    const auto inputPaths = options["file"].as<std::vector<std::string>>();
    for(const bfs::path inputPath : inputPaths)
    {
        if(!bfs::exists(inputPath))
        {
            bnw::cerr << "Input file or folder does not exist: " << inputPath;
            return 1;
        }

        if(bfs::is_regular_file(inputPath))
        {
            if(!inputPath.has_extension())
            {
                bnw::cerr << "Input filepath has no extension: " << inputPath;
                return 1;
            }
            const auto outPath = inputPath.stem();

            bnw::cout << "Unpacking file " << inputPath << " to " << outPath << std::endl;

            libsiedler2::Archiv lst;
            if(Load(inputPath, lst, palette) != 0)
            {
                bnw::cerr << "Fatal Error: " << std::endl;
                bnw::cerr << inputPath << " was not found or cannot be opened" << std::endl;
                return 3;
            }

            unpack(outPath, lst, palette, "", options.count("palAsTxt") != 0u);
        } else if(bfs::is_directory(inputPath))
        {
            bfs::path outFilepath = (inputPath / ".").parent_path(); // Get real path to parent dir
            outFilepath += ".NEW." + outFileExt;
            bnw::cout << "Packing directory " << inputPath << " to " << outFilepath << std::endl;
            pack(inputPath, outFilepath, palette);
        } else
        {
            bnw::cerr << "Unknown type. Not a file or folder: " << inputPath;
            return 1;
        }
    }

    bnw::cout << "done" << std::endl;
    bnw::cout << "press enter to exit" << std::endl;
    bnw::cin.get();
    return 0;
}
