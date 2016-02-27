// Copyright (c) 2005-2009 Settlers Freaks (sf-team at siedler25.org)
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

///////////////////////////////////////////////////////////////////////////////
// Header
#include "util.h"
#include "ArchivInfo.h"
#include "ArchivItem_Palette.h"
#include "libsiedler2.h"

#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include <cstdio>

using namespace std;
using namespace libsiedler2;

void unpack(const string& directory, const ArchivInfo& lst, const ArchivItem_Palette* palette, const std::string& fileNameHexPrefix = "");
void pack(const string& directory, const string& file, const ArchivItem_Palette* palette, ArchivInfo* lst = NULL);

int main(int argc, char* argv[])
{
    if(argc < 2)
    {
        cerr << "Usage: " << endl;
        cerr << "pack:   " << argv[0] << " directory" << endl;
        cerr << "unpack: " << argv[0] << " file.lst"  << endl;

        return 1;
    }

    ArchivInfo lst, bbm;

    if(Load("GFX/PALETTE/PAL5.BBM", bbm) != 0)
    {
        if(Load("pal5.act", bbm) != 0)
        {
            cerr << "Fatal Error: " << endl;
            cerr << "GFX/PALETTE/PAL5.BBM nor pal5.act was not found or cannot be opened" << endl;

            return 2;
        }
    }

    ArchivItem_Palette* palette = (ArchivItem_Palette*)bbm.get(0);

    TEXTURFORMAT format = FORMAT_RGBA;

    setTextureFormat(format);

    FILE* f = fopen(argv[1], "rb");
    if(f)
    {
        fclose(f);

        string file(argv[1]);
        reverse(file.begin(), file.end());

        vector<string> filep =  explode(file, '.', 2);

        string directory = filep.at(1);
        reverse(directory.begin(), directory.end());

        cerr << "Unpacking file " << argv[1] << " to " << directory << endl;

        if(Load(argv[1], lst, palette) != 0)
        {
            cerr << "Fatal Error: " << endl;
            cerr << argv[1] << " was not found or cannot be opened" << endl;;

            return 3;
        }

        unpack(directory, lst, palette);
    }
    else
    {
        string directory(argv[1]);
        string file(argv[1]);

        file += ".NEW.LST";

        cerr << "Packing directory " << argv[1] << " to " << file << endl;

        pack(directory, file, palette);
    }

    cerr << "done" << endl;

    cerr << "press enter to exit" << endl;
    cin.get();
    return 0;
}
