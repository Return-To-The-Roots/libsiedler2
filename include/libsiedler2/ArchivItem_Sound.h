// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "ArchivItem.h"
#include "enumTypes.h"
#include <cstdint>
#include <iosfwd>
#include <memory>

namespace libsiedler2 {
/// Klasse für Sounditems.
class ArchivItem_Sound : public ArchivItem
{
public:
    ArchivItem_Sound(SoundType soundType);
    ~ArchivItem_Sound() override;

    /// liefert den Typ des Sounds.
    SoundType getType() const;

    /// lädt die Sound-Daten aus einer Datei.
    virtual int load(std::istream& file, uint32_t length) = 0;

    /// schreibt die Sound-Daten in eine Datei.
    virtual int write(std::ostream& file) const = 0;

    static std::unique_ptr<ArchivItem_Sound> findSubType(std::istream& file);

private:
    SoundType soundType_;
};

} // namespace libsiedler2
