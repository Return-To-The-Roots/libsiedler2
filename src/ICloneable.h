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

#pragma once

#ifndef ICloneable_h__
#define ICloneable_h__

namespace libsiedler2 {

/// Class is cloneable. Calling clone() creates a copy of the correct subtype
template<class T_Base>
class ICloneable
{
public:
    virtual ~ICloneable() {}
    virtual T_Base* clone() const = 0;
};

} // namespace libsiedler2

/// Macro for implementing the clone function
/// To be used in a class TFoo by calling
/// RTTR_CLONEABLE(TFoo) in the public section of the class declaration
#define RTTR_CLONEABLE(TYPE) \
    TYPE* clone() const override { return new TYPE(*this); }

#endif // ICloneable_h__
