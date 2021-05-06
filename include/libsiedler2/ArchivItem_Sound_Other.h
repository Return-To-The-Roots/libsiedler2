// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "ArchivItem_Sound.h"
#include <cstdint>
#include <vector>

namespace libsiedler2 {
/// Klasse für Other-Sounds.
class ArchivItem_Sound_Other : public ArchivItem_Sound
{
public:
    explicit ArchivItem_Sound_Other(SoundType sndType);
    RTTR_CLONEABLE(ArchivItem_Sound_Other)

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

} // namespace libsiedler2
