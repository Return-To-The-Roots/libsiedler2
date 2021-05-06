// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "FileEntry.h"
#include "enumTypes.h"
#include <boost/filesystem/path.hpp>
#include <string>
#include <vector>

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
int Load(const boost::filesystem::path& filepath, Archiv& items, const ArchivItem_Palette* palette = nullptr);
/// Schreibt die Datei im Format ihrer Endung.
int Write(const boost::filesystem::path& filepath, const Archiv& items, const ArchivItem_Palette* palette = nullptr);
/// List all files in the folder and fills them into the vector
std::vector<FileEntry> ReadFolderInfo(const boost::filesystem::path& folderPath);
/// Load all files from the folderInfos into the archiv. Sorts the infos first
int LoadFolder(std::vector<FileEntry> folderInfos, Archiv& items, const ArchivItem_Palette* palette = nullptr);

} // namespace libsiedler2
