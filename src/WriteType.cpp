// $Id: WriteType.cpp 9359 2014-04-25 15:37:22Z FloSoft $
//
// Copyright (c) 2005 - 2011 Settlers Freaks (sf-team at siedler25.org)
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

///////////////////////////////////////////////////////////////////////////////
// Header
#include "main.h"

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#if defined _WIN32 && defined _DEBUG && defined _MSC_VER
#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////////
/**
 *  schreibt eine spezifizierten Bobtype aus einem ArchivItem in eine Datei.
 *
 *  @param[in]  bobtype Typ des Items
 *  @param[in]  file    Filehandle auf die auszulesende Datei
 *  @param[in]  palette Grundpalette
 *  @param[out] item    ArchivItem-Struktur, welche gefüllt wird
 *
 *  @return Null bei Erfolg, ein Wert ungleich Null bei Fehler
 *
 *  @author FloSoft
 *  @author OLiver
 */
int libsiedler2::loader::WriteType(unsigned short bobtype, FILE* file, const ArchivItem_Palette* palette, const ArchivItem* item)
{
    if(file == NULL || item == NULL)
        return 1;

    switch(bobtype)
    {
        case BOBTYPE_SOUND: // WAVs, MIDIs
        {
            const baseArchivItem_Sound* i = dynamic_cast<const baseArchivItem_Sound*>(item);

            if(i->write(file) != 0)
                return 2;
        } break;
        case BOBTYPE_BITMAP_RLE: // RLE komprimiertes Bitmap
        {
            const baseArchivItem_Bitmap_RLE* i = dynamic_cast<const baseArchivItem_Bitmap_RLE*>(item);

            if(i->write(file, palette) != 0)
                return 3;
        } break;
        case BOBTYPE_FONT: // Font
        {
            const ArchivItem_Font* i = dynamic_cast<const ArchivItem_Font*>(item);

            if(i->write(file, palette) != 0)
                return 4;
        } break;
        case BOBTYPE_BITMAP_PLAYER: // Bitmap mit spezifischer Spielerfarbe
        {
            const baseArchivItem_Bitmap_Player* i = dynamic_cast<const baseArchivItem_Bitmap_Player*>(item);

            if(i->write(file, palette) != 0)
                return 5;
        } break;
        case BOBTYPE_PALETTE: // Palette
        {
            const ArchivItem_Palette* i = dynamic_cast<const ArchivItem_Palette*>(item);

            if(i->write(file) != 0)
                return 6;
        } break;
        case BOBTYPE_BITMAP_SHADOW: // Schatten
        {
            const baseArchivItem_Bitmap_Shadow* i = dynamic_cast<const baseArchivItem_Bitmap_Shadow*>(item);

            if(i->write(file, palette) != 0)
                return 7;
        } break;
        case BOBTYPE_BOB: // Bobfile
        {
            const ArchivItem_Bob* i = dynamic_cast<const ArchivItem_Bob*>(item);

            if(i->write(file, palette) != 0)
                return 8;
        } break;
        case BOBTYPE_MAP: // Mapfile
        {
            const ArchivItem_Map* i = dynamic_cast<const ArchivItem_Map*>(item);

            if(i->write(file) != 0)
                return 9;
        } break;
        case BOBTYPE_BITMAP_RAW: // unkomprimiertes Bitmap
        {
            const baseArchivItem_Bitmap_Raw* i = dynamic_cast<const baseArchivItem_Bitmap_Raw*>(item);

            if(i->write(file, palette) != 0)
                return 10;
        } break;
    }

    return 0;
}
