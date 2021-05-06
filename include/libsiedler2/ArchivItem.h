// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "ICloneable.h" // IWYU pragma: export
#include "enumTypes.h"
#include <string>

namespace libsiedler2 {
/// Base class for all ArchivItems. Defined by a type and possibly a name
/// Implements the cloneable concept:
/// A copy of this object can be created by calling clone(obj)
/// Note: All subclasses "TFoo" must use "RTTR_CLONEABLE(TFoo)" in the public declaration
class ArchivItem : public ICloneable<ArchivItem> //-V690
{
public:
    ArchivItem(BobType bobtype = BobType::None);
    virtual ~ArchivItem() override;
    /// liefert den Bobtype des Items.
    BobType getBobType() const { return bobtype_; }
    /// Set the name if the item
    void setName(const std::string& name) { name_ = name; }
    /// Return the name of the item
    std::string getName() const { return name_; }

protected:
    ArchivItem(const ArchivItem&) = default;
    ArchivItem(ArchivItem&&) noexcept = default;
    ArchivItem& operator=(const ArchivItem&) = default;
    ArchivItem& operator=(ArchivItem&&) noexcept = default;
    // TODO: protected because classes with virtual inheritance may want to set it instead of calling many ctors down
    // the line
    BobType bobtype_; /// Type of the element
private:
    std::string name_; /// Element name
};
} // namespace libsiedler2
