// $Id: lstpacker.cpp 9359 2014-04-25 15:37:22Z FloSoft $
//
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
#include <iostream>
#include <sstream>
#include <string>
#include <algorithm>
#include <vector>
#include <windows.h>

#include "util.h"

#define snprintf _snprintf

#include "../../src/libsiedler2.h"

using namespace std;
using namespace libsiedler2;
using namespace loader;

void unpack(const string& directory, const ArchivInfo& lst, const ArchivItem_Palette* palette);
void pack(const string& directory, const string& file, const ArchivItem_Palette* palette, ArchivInfo* lst = NULL);

int main(int argc, char* argv[])
{
    if(argc < 2)
    {
        stringstream msg;

        msg << "Usage: " << endl;
        msg << "pack:   " << argv[0] << " directory" << endl;
        msg << "unpack: " << argv[0] << " file.lst"  << endl;

        cerr << msg.str();

        MessageBoxA(NULL, msg.str().c_str(), "Usage", MB_OK | MB_ICONSTOP);

        return 1;
    }

    ArchivInfo lst, bbm;

    if(LoadBBM("GFX/PALETTE/PAL5.BBM", &bbm) != 0)
    {
        if(LoadBBM("pal5.act", &bbm) != 0)
        {
            stringstream msg;

            msg << "Fatal Error: " << endl;
            msg << "GFX/PALETTE/PAL5.BBM nor pal5.act was not found or cannot be opened" << endl;

            cerr << msg.str();

            MessageBoxA(NULL, msg.str().c_str(), "Fatal Error", MB_OK | MB_ICONSTOP);

            return 2;
        }
    }

    ArchivItem_Palette* palette = (ArchivItem_Palette*)bbm.get(0);

    int format = FORMAT_RGBA;

    setTextureFormat((TEXTURFORMAT)format);

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

        if(Load(argv[1], &lst, palette) != 0)
        {
            stringstream msg;

            msg << "Fatal Error: " << endl;
            msg << argv[1] << " was not found or cannot be opened" << endl;

            cerr << msg.str();

            MessageBoxA(NULL, msg.str().c_str(), "Fatal Error", MB_OK | MB_ICONSTOP);

            return 3;
        }

        unpack(directory, lst, palette);

        //argv[1][directory.size()] = '\0';
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
