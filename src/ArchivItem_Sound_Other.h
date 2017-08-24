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
#ifndef ARCHIVITEM_SOUND_OTHER_H_INCLUDED
#define ARCHIVITEM_SOUND_OTHER_H_INCLUDED

#pragma once

#include "ArchivItem_Sound.h"
#include <stdint.h>
#include <vector>

namespace libsiedler2 {
/// Basisklasse für Other-Sounds.
class baseArchivItem_Sound_Other : public virtual baseArchivItem_Sound
{
public:
    baseArchivItem_Sound_Other(SoundType sndType);

    ~baseArchivItem_Sound_Other() override;

    /// lädt die Daten aus einer Datei.
    int load(std::istream& file, uint32_t length) override;

    /// schreibt die Daten in eine Datei.
    int write(std::ostream& file) const override;

    /// räumt den Soundspeicher auf.
    void clear();

    const std::vector<uint8_t>& getData() const { return data; }
    void setData(const std::vector<uint8_t>& newData) { data = newData; }
    uint32_t getLength() const { return static_cast<uint32_t>(data.size()); }

protected:
    std::vector<uint8_t> data;
};

/// Klasse für Other-Sounds.
class ArchivItem_Sound_Other : public virtual baseArchivItem_Sound_Other, public ArchivItem_Sound
{
public:
    ArchivItem_Sound_Other(SoundType sndType) : baseArchivItem_Sound_Other(sndType) {}

    ArchivItem_Sound_Other(const ArchivItem_Sound_Other& item) : baseArchivItem_Sound(item), baseArchivItem_Sound_Other(item) {}
    RTTR_CLONEABLE(ArchivItem_Sound_Other)
};
} // namespace libsiedler2

#endif // !ARCHIVITEM_SOUND_OTHER_H_INCLUDED
