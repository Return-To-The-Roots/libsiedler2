// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "libsiedler2/ICloneable.h"
#include <boost/mpl/list.hpp>
#include <boost/test/unit_test.hpp>

using namespace libsiedler2;

namespace {

struct Base : ICloneable<Base>
{};

struct Foo : Base
{
    RTTR_CLONEABLE(Foo);
    int i;
    Foo(int i) { this->i = i; }
};
struct BarBase : Base
{
    int i;
};
struct Bar : virtual BarBase
{
    RTTR_CLONEABLE(Bar);
    Bar(int i) { this->i = i; }
    Bar(int i, int j) : j(j) { this->i = i; }
    int j;
};

using TestedTypes = boost::mpl::list<Foo, Bar>;
} // namespace

static_assert(detail::is_static_castable_v<int, int>);
static_assert(detail::is_static_castable_v<int, float>);
static_assert(detail::is_static_castable_v<int*, const int*>);
static_assert(!detail::is_static_castable_v<const int*, int*>);

static_assert(detail::is_static_castable_v<Foo*, Foo*>);
static_assert(detail::is_static_castable_v<Foo*, const Foo*>);
static_assert(!detail::is_static_castable_v<const Foo*, Foo*>);
static_assert(detail::is_static_castable_v<Foo*, Base*>);
static_assert(detail::is_static_castable_v<Foo*, const Base*>);
static_assert(detail::is_static_castable_v<Base*, Foo*>);
static_assert(!detail::is_static_castable_v<const Base*, Foo*>);

static_assert(detail::is_static_castable_v<BarBase*, Base*>);
static_assert(detail::is_static_castable_v<Base*, BarBase*>);
static_assert(detail::is_static_castable_v<Bar*, BarBase*>);
static_assert(detail::is_static_castable_v<Bar*, Base*>);
// Virtual inheritance -> no base to derived cast
static_assert(!detail::is_static_castable_v<Base*, Bar*>);
static_assert(!detail::is_static_castable_v<BarBase*, Bar*>);

BOOST_AUTO_TEST_CASE_TEMPLATE(CloneWorks, T, TestedTypes)
{
    auto f = std::make_unique<T>(42);
    auto f2 = clone(f);       // By unique_ptr
    auto f3 = clone(f.get()); // By raw ptr
    auto f4 = clone(*f);      // By reference
    static_assert(std::is_same_v<decltype(f2), decltype(f)>, "Must be the same");
    static_assert(std::is_same_v<decltype(f3), decltype(f)>, "Must be the same");
    static_assert(std::is_same_v<decltype(f4), decltype(f)>, "Must be the same");
    BOOST_TEST((f && f2 && f3 && f4));
    BOOST_TEST(f->i == 42);
    BOOST_TEST(f2->i == 42);
    BOOST_TEST(f3->i == 42);
    BOOST_TEST(f4->i == 42);
}

BOOST_AUTO_TEST_CASE(CloneFromBasePtr)
{
    auto f = std::make_unique<Bar>(42, 1337);
    auto* basePtr = static_cast<BarBase*>(f.get());
    auto f2 = clone(basePtr);
    static_assert(std::is_same_v<decltype(f2), std::unique_ptr<BarBase>>, "Must be the same");
    BOOST_TEST((f && f2 && basePtr));
    BOOST_TEST(f->i == 42);
    BOOST_TEST(f->j == 1337);
    BOOST_TEST(f2->i == 42);
    BOOST_TEST(dynamic_cast<Bar*>(f2.get())->j == 1337);
}

BOOST_AUTO_TEST_CASE(CloneNull)
{
    std::unique_ptr<Foo> f;
    auto f2 = clone(f);       // By unique_ptr
    auto f3 = clone(f.get()); // By raw ptr
    static_assert(std::is_same_v<decltype(f2), decltype(f)>, "Must be the same");
    static_assert(std::is_same_v<decltype(f3), decltype(f)>, "Must be the same");
    BOOST_TEST(!f2);
    BOOST_TEST(!f3);
}
