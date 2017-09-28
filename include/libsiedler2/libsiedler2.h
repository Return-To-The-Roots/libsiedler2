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
#ifndef LIBSIEDLER2_H_INCLUDED
#define LIBSIEDLER2_H_INCLUDED

#pragma once

#include "enumTypes.h"
#include <string>

namespace libsiedler2 {
// Fwd decl
class Archiv;
class ArchivItem_Palette;
class IAllocator;

/// Setzt das verwendete Texturausgabeformat.
TextureFormat setGlobalTextureFormat(TextureFormat format);
/// liefert das verwendete Texturausgabeformat.
TextureFormat getGlobalTextureFormat();

const IAllocator& getAllocator();
/// Setzt den Item-Allocator.
void setAllocator(IAllocator* newAllocator);

/// Lädt die Datei im Format ihrer Endung.
int Load(const std::string& file, Archiv& items, const ArchivItem_Palette* palette = NULL);
/// Schreibt die Datei im Format ihrer Endung.
int Write(const std::string& file, const Archiv& items, const ArchivItem_Palette* palette = NULL);
} // namespace libsiedler2

#endif // !LIBSIEDLER2_H_INCLUDED
