// Copyright (c) 2005 - 2017 Settlers Freaks (sf-team at siedler25.org)
//
// This file is part of Siedler II.5 RTTR.
//
// Siedler II.5 RTTR is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// Siedler II.5 RTTR is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Siedler II.5 RTTR. If not, see <http://www.gnu.org/licenses/>.


#include "pack.h"
#include "unpack.h"
#include "libsiedler2/src/ArchivInfo.h"
#include "libsiedler2/src/ArchivItem_Palette.h"
#include "libsiedler2/src/libsiedler2.h"
#include <boost/filesystem.hpp>
#include <iostream>
#include <string>
#include <vector>

using namespace std;
using namespace libsiedler2;
namespace bfs = boost::filesystem;

int main(int argc, char* argv[])
{
    if(argc < 2)
    {
        cerr << "Usage: " << endl;
        cerr << "pack:   " << argv[0] << " directory" << endl;
        cerr << "unpack: " << argv[0] << " file.lst"  << endl;
        cerr << "Optionally pass a color palette file (bbm/act) to use instead" << endl;

        return 1;
    }

    bfs::path inputPath(argv[1]);
    if(!bfs::exists(inputPath))
    {
        cerr << "Input file or folder does not exist: " << inputPath;
        return 1;
    }

    ArchivInfo bbm;

    if(argc == 3)
    {
        if(Load(argv[2], bbm) != 0)
        {
            cerr << "Error: Could not load given palette: " << argv[2] << endl;
            cerr << "Retrying with default ones" << endl;
        }else
            cout << "Using non-standard color palette: " << argv[2] << endl;
    }

    bfs::path pal5Path("GFX/PALETTE/PAL5.BBM");
    bfs::path pal5Path2("pal5.act");

    if(bbm.empty() && (!bfs::exists(pal5Path) || Load(pal5Path.string(), bbm) != 0))
    {
        if(Load(pal5Path2.string(), bbm) != 0)
        {
            cerr << "Fatal Error: " << endl;
            cerr << "Neither " << pal5Path << " nor " << pal5Path2 << " was found or it cannot be opened" << endl;

            return 2;
        }
    }

    ArchivItem_Palette* palette = (ArchivItem_Palette*)bbm.get(0);

    TEXTURFORMAT format = FORMAT_RGBA;

    setTextureFormat(format);

    if(bfs::is_regular_file(inputPath))
    {
        if(!inputPath.has_extension())
        {
            cerr << "Input filepath has no extension: " << inputPath;
            return 1;
        }
        std::string outPath = inputPath.stem().string();

        cerr << "Unpacking file " << argv[1] << " to " << outPath << endl;

        ArchivInfo lst;
        if(Load(argv[1], lst, palette) != 0)
        {
            cerr << "Fatal Error: " << endl;
            cerr << argv[1] << " was not found or cannot be opened" << endl;;

            return 3;
        }

        unpack(outPath, lst, palette);
    }
    else if(bfs::is_directory(inputPath))
    {
        bfs::path outFilepath = inputPath.string() + ".NEW.LST";

        cerr << "Packing directory " << inputPath << " to " << outFilepath << endl;

        pack(inputPath.string(), outFilepath.string(), palette);
    }else
    {
        cerr << "Unknown type. Not a file or folder: " << inputPath;
        return 1;
    }

    cerr << "done" << endl;

    cerr << "press enter to exit" << endl;
    cin.get();
    return 0;
}
