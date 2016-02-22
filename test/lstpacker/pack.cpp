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
#include "ArchivItem_Font.h"
#include "ArchivItem_BitmapBase.h"
#include "ArchivItem_Bitmap_Player.h"
#include "ArchivItem_Bitmap.h"
#include "libsiedler2.h"

#include <boost/filesystem.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>

namespace libsiedler2 { class ArchivItem_Palette; }
namespace bfs = boost::filesystem;

using namespace std;
using namespace libsiedler2;

struct fileentry
{
    fileentry() : bobtype(BOBTYPE_NONE), nx(0), ny(0) { }
    string file;
    string path;
    BOBTYPES bobtype;
    unsigned short nx;
    unsigned short ny;
    string type;
};

bool stringCompare( const fileentry& left, const fileentry& right )
{
    int a, b;

    stringstream aa;
    aa << left.file;
    stringstream bb;
    bb << right.file;

    if( !(aa >> a) || !(bb >> b) )
    {
        for( string::const_iterator lit = left.file.begin(), rit = right.file.begin(); lit != left.file.end() && rit != right.file.end(); ++lit, ++rit )
            if( tolower( *lit ) < tolower( *rit ) )
                return true;
            else if( tolower( *lit ) > tolower( *rit ) )
                return false;
        if( left.file.size() < right.file.size() )
            return true;
    }
    else
    {
        if(a < b)
            return true;
    }

    return false;
}

void pack(const string& directory, const string& file, const ArchivItem_Palette* palette, ArchivInfo* lst)
{
    ArchivInfo tlst;

    if(lst == NULL)
        lst = &tlst;

    vector<fileentry> files;

    cerr << "Reading directory: ";
    for(bfs::directory_iterator it = bfs::directory_iterator(directory); it != bfs::directory_iterator(); ++it)
    {
        if(!bfs::is_regular_file(it->status()))
            continue;

        bfs::path curPath = it->path();
        curPath.make_preferred();

        fileentry file;

        string whole_file = curPath.filename().string();
        string whole_path = curPath.string();

        transform ( whole_file.begin(), whole_file.end(), whole_file.begin(), ::tolower );

        vector<string> wf = explode(whole_file, '.');

        for(vector<string>::iterator it = wf.begin(); it != wf.end(); ++it)
        {
            if(*it == "rle")
                file.bobtype = BOBTYPE_BITMAP_RLE;
            else if(*it == "player")
                file.bobtype = BOBTYPE_BITMAP_PLAYER;
            else if(*it == "shadow")
                file.bobtype = BOBTYPE_BITMAP_SHADOW;

            else if(it->substr(0, 2) == "nx" || it->substr(0, 2) == "dx")
                file.nx = atoi(it->substr(2).c_str());
            else if(it->substr(0, 2) == "ny" || it->substr(0, 2) == "dy")
                file.ny = atoi(it->substr(2).c_str());

            else
                file.file += (file.file.empty() ? "" : ".") + *it;
        }

        if((wf.back()) == "fon")
            file.type = "font";
        else if((wf.back()) == "bmp")
            file.type = "bitmap";
        else if((wf.back()) == "bbm" || (wf.back()) == "act")
        {
            file.bobtype = BOBTYPE_PALETTE;
            file.type = "palette";
        }
        else if((wf.back()) == "empty")
            file.type = "empty";


        if((wf.back()) != "db") // do not add "Thumbs.db"
        {
            file.path = whole_path;
            files.push_back(file);
        }
    }

    cerr << "done" << endl;

    cerr << "Sorting directory (this can take some time!): ";
    sort(files.begin(), files.end(), stringCompare);
    cerr << "done" << endl;

    unsigned char* buffer = new unsigned char[1000 * 1000 * 4];
    for(vector<fileentry>::const_iterator it = files.begin(); it != files.end(); ++it)
    {
        string whole_path = it->path;

        // read file number, to set the index correctly
        std::string filename = whole_path.substr(whole_path.find_last_of("/\\") + 1);
        std::stringstream nrs;
        int nr = -1;
        nrs << filename;
        if(! (nrs >> nr) )
            nr = -1;

        ArchivInfo items;

        cout << "Reading file " << whole_path;
        if( nr >= 0 )
            cout << " to " << nr;
        std::cout << ": ";

        if(it->type == "font")
        {
            ArchivItem_Font font;
            font.setDx(it->nx & 0xFF);
            font.setDy(it->ny & 0xFF);
            pack(whole_path, "", palette, &font);

            // had the filename a number? then set it to the corresponding item.
            if(nr >= 0)
            {
                if((unsigned int)nr >= lst->size())
                    lst->alloc_inc(nr - lst->size() + 1);
                lst->setC(nr, font);
            }
            else
                lst->pushC(font);
        }
        else if(it->type == "empty" || Load(whole_path, items, palette) != 0)
        {
            lst->alloc_inc(1); // add empty item
            if(it->type == "empty")
                cout << "ignored" << endl;
            else
                cout << "failed" << endl;
        }
        else
        {
            cout << "done" << endl;
            // todo: andere typen als pal und bmp haben evtl mehr items!

            ArchivItem* neu = items.get(0);
            if(it->type == "bitmap")
            {
                ArchivItem_BitmapBase* i = dynamic_cast<ArchivItem_BitmapBase*>(neu);
                ArchivItem_BitmapBase* n = i;

                switch(it->bobtype)
                {
                    case BOBTYPE_BITMAP_RLE:
                    case BOBTYPE_BITMAP_PLAYER:
                    case BOBTYPE_BITMAP_SHADOW:
                    {
                        n = dynamic_cast<ArchivItem_BitmapBase*>(getAllocator().create(it->bobtype));
                    } break;
                }

                n->setName(whole_path);
                n->setNx(it->nx);
                n->setNy(it->ny);

                if(n != i)
                {
                    memset(buffer, 0, 1000 * 1000 * 4);
                    if(it->bobtype == BOBTYPE_BITMAP_PLAYER)
                        dynamic_cast<ArchivItem_Bitmap_Player*>(i)->print(buffer, 1000, 1000, FORMAT_RGBA, palette, 128);
                    else
                        dynamic_cast<ArchivItem_Bitmap*>(i)->print(buffer, 1000, 1000, FORMAT_RGBA, palette);
                }

                switch(it->bobtype)
                {
                    case BOBTYPE_BITMAP_RLE:
                    case BOBTYPE_BITMAP_SHADOW:
                    {
                        dynamic_cast<ArchivItem_Bitmap*>(n)->create(i->getWidth(), i->getHeight(), buffer, 1000, 1000, FORMAT_RGBA, palette);
                    } break;
                    case BOBTYPE_BITMAP_PLAYER:
                    {
                        dynamic_cast<ArchivItem_Bitmap_Player*>(n)->create(i->getWidth(), i->getHeight(), buffer, 1000, 1000, FORMAT_RGBA, palette, 128);
                    } break;
                }

                neu = n;
            }

            // had the filename a number? then set it to the corresponding item.
            if(nr >= 0)
            {
                if((unsigned int)nr >= lst->size())
                    lst->alloc_inc(nr - lst->size() + 1);
                lst->setC(nr, *neu);
            }
            else
                lst->pushC(*neu);
        }
    }
    delete[] buffer;

    cout << "Writing data to " << file << ": ";

    if(lst == &tlst) // only write to lstfile if the caller does not want the ArchivInfo back
    {
        if(Write(file, *lst, palette) != 0)
            cout << "failed" << endl;
        else
            cout << "done" << endl;
    }
}

