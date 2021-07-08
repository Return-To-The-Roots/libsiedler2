// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "enumTypes.h"
#include <boost/filesystem/path.hpp>
#include <string>
#include <utility>

namespace libsiedler2 {
struct FileEntry
{
    explicit FileEntry(boost::filesystem::path filePath) : filePath(std::move(filePath)) {}
    FileEntry(boost::filesystem::path filePath, std::string name, int nr, BobType bobtype, int nx, int ny)
        : filePath(std::move(filePath)), name(std::move(name)), nr(nr), bobtype(bobtype), nx(nx), ny(ny)
    {}
    /// Path to file and (remaining) name
    boost::filesystem::path filePath;
    std::string name;
    /// Index in the archive/folder
    int nr = -1;
    BobType bobtype = BobType::Unset;
    /// Origin or delta (fonts)
    int nx = 0, ny = 0;
};
bool operator<(const FileEntry& left, const FileEntry& right);
} // namespace libsiedler2
