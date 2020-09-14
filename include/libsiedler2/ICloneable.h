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

#include <cassert>
#include <memory>
#include <type_traits>

namespace libsiedler2 {

template<typename T, typename U, typename = void>
struct is_static_castable : std::false_type
{};

template<typename T, typename U>
struct is_static_castable<T, U, decltype(void(static_cast<U>(std::declval<T>())))> : std::true_type
{};

template<typename To, typename From>
std::enable_if_t<is_static_castable<From*, To*>::value, To*> safePtrCast(From* from)
{
    return static_cast<To*>(from);
}

template<typename To, typename From>
std::enable_if_t<!is_static_castable<From*, To*>::value, To*> safePtrCast(From* from)
{
    return dynamic_cast<To*>(from);
}

template<class T, std::enable_if_t<!std::is_pointer<T>::value, int> = 0>
auto clone(const T& obj)
{
    return std::unique_ptr<T>(safePtrCast<T>(obj.clone()));
}
template<class T>
auto clone(const T* obj)
{
    return obj ? clone(*obj) : nullptr;
}
template<class T>
auto clone(const std::unique_ptr<T>& obj)
{
    return clone(obj.get());
}

/// Class is cloneable. Calling clone() creates a copy of the correct subtype
template<class T_Base>
class ICloneable
{
public:
    virtual ~ICloneable() = default;
    virtual T_Base* clone() const = 0;
};

} // namespace libsiedler2

/// Macro for implementing the clone function
/// To be used in a class TFoo by calling
/// RTTR_CLONEABLE(TFoo) in the public section of the class declaration
#define RTTR_CLONEABLE(TYPE) \
    TYPE* clone() const override { return new TYPE(*this); }
