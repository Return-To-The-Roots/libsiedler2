// Copyright (c) 2017 - 2020 Settlers Freaks (sf-team at siedler25.org)
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

static_assert(detail::is_static_castable<int, int>::value, "!");
static_assert(detail::is_static_castable<int, float>::value, "!");
static_assert(detail::is_static_castable<int*, const int*>::value, "!");
static_assert(!detail::is_static_castable<const int*, int*>::value, "!");

static_assert(detail::is_static_castable<Foo*, Foo*>::value, "!");
static_assert(detail::is_static_castable<Foo*, const Foo*>::value, "!");
static_assert(!detail::is_static_castable<const Foo*, Foo*>::value, "!");
static_assert(detail::is_static_castable<Foo*, Base*>::value, "!");
static_assert(detail::is_static_castable<Foo*, const Base*>::value, "!");
static_assert(detail::is_static_castable<Base*, Foo*>::value, "!");
static_assert(!detail::is_static_castable<const Base*, Foo*>::value, "!");

static_assert(detail::is_static_castable<BarBase*, Base*>::value, "!");
static_assert(detail::is_static_castable<Base*, BarBase*>::value, "!");
static_assert(detail::is_static_castable<Bar*, BarBase*>::value, "!");
static_assert(detail::is_static_castable<Bar*, Base*>::value, "!");
// Virtual inheritance -> no base to derived cast
static_assert(!detail::is_static_castable<Base*, Bar*>::value, "!");
static_assert(!detail::is_static_castable<BarBase*, Bar*>::value, "!");

BOOST_AUTO_TEST_CASE_TEMPLATE(CloneWorks, T, TestedTypes)
{
    auto f = std::make_unique<T>(42);
    auto f2 = clone(f);       // By unique_ptr
    auto f3 = clone(f.get()); // By raw ptr
    auto f4 = clone(*f);      // By reference
    static_assert(std::is_same<decltype(f2), decltype(f)>::value, "Must be the same");
    static_assert(std::is_same<decltype(f3), decltype(f)>::value, "Must be the same");
    static_assert(std::is_same<decltype(f4), decltype(f)>::value, "Must be the same");
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
    static_assert(std::is_same<decltype(f2), std::unique_ptr<BarBase>>::value, "Must be the same");
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
    static_assert(std::is_same<decltype(f2), decltype(f)>::value, "Must be the same");
    static_assert(std::is_same<decltype(f3), decltype(f)>::value, "Must be the same");
    BOOST_TEST(!f2);
    BOOST_TEST(!f3);
}
