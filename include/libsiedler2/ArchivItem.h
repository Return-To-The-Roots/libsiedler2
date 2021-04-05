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
