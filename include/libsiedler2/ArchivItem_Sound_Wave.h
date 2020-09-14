// Copyright (c) 2005 - 2020 Settlers Freaks (sf-team at siedler25.org)
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
