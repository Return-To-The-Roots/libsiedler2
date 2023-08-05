// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <cassert>
#include <memory>
#include <type_traits>

namespace libsiedler2 {

namespace detail {
    template<typename T, typename U, typename = void>
    struct is_static_castable : std::false_type
    {};

    template<typename T, typename U>
    struct is_static_castable<T, U, decltype(void(static_cast<U>(std::declval<T>())))> : std::true_type
    {};

    template<typename T, typename U>
    inline constexpr bool is_static_castable_v = is_static_castable<T, U>::value;

    template<typename To, typename From>
    std::enable_if_t<is_static_castable_v<From*, To*>, To*> safePtrCast(From* from)
    {
        return static_cast<To*>(from);
    }

    template<typename To, typename From>
    std::enable_if_t<!is_static_castable_v<From*, To*>, To*> safePtrCast(From* from)
    {
        return dynamic_cast<To*>(from);
    }
} // namespace detail

template<class T, std::enable_if_t<!std::is_pointer<T>::value, int> = 0>
auto clone(const T& obj)
{
    return std::unique_ptr<T>(detail::safePtrCast<T>(obj.clone()));
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
