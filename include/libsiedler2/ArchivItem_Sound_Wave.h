// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "ArchivItem_Sound.h"
#include "WAV_Header.h"
#include <cstdint>
#include <vector>

namespace libsiedler2 {
/// Klasse für WAVE-Sounds.
class ArchivItem_Sound_Wave : public ArchivItem_Sound
{
public:
    ArchivItem_Sound_Wave();
    RTTR_CLONEABLE(ArchivItem_Sound_Wave)

    /// lädt die Wave-Daten aus einer Datei.
    int load(std::istream& file, uint32_t length) override;

    /// schreibt die Wave-Daten in eine Datei.
    int write(std::ostream& file) const override { return write(file, false); }
    int write(std::ostream& file, bool stripheader) const;

    /// räumt den Soundspeicher auf.
    void clear();

    const std::vector<uint8_t>& getData() const { return data; }
    void setData(const std::vector<uint8_t>& data) { this->data = data; }
    const WAV_Header& getHeader() const { return header; }
    void setHeader(const WAV_Header& newHeader) { header = newHeader; }
    uint32_t getLength() { return static_cast<uint32_t>(data.size()); }

protected:
    WAV_Header header; //-V730_NOINIT
    std::vector<uint8_t> data;
};

} // namespace libsiedler2
