// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "test/config.h"
#include "libsiedler2/Archiv.h"
#include "libsiedler2/ArchivItem_Raw.h"
#include "libsiedler2/IAllocator.h"
#include "libsiedler2/enumTypes.h"
#include "libsiedler2/libsiedler2.h"
#include <boost/test/unit_test.hpp>
#include <stdexcept>

namespace libsiedler2 {
// LCOV_EXCL_START
static std::ostream& boost_test_print_type(std::ostream& os, libsiedler2::BobType bt)
{
    return os << static_cast<unsigned>(bt);
}
// LCOV_EXCL_STOP
} // namespace libsiedler2

// LCOV_EXCL_START
BOOST_TEST_DONT_PRINT_LOG_VALUE(std::unique_ptr<libsiedler2::ArchivItem>)
// LCOV_EXCL_STOP

BOOST_AUTO_TEST_SUITE(Archiv)

BOOST_AUTO_TEST_CASE(Push)
{
    libsiedler2::Archiv archiv;
    BOOST_TEST_REQUIRE(archiv.empty());
    auto rawItem = std::make_unique<libsiedler2::ArchivItem_Raw>();
    rawItem->getData().push_back(42);
    archiv.pushC(*rawItem);
    BOOST_TEST_REQUIRE(!archiv.empty());
    BOOST_TEST_REQUIRE(archiv.size() == 1u);
    auto* rawItemOut = dynamic_cast<libsiedler2::ArchivItem_Raw*>(archiv[0]);
    BOOST_TEST_REQUIRE(rawItemOut);
    // Should have copied
    BOOST_TEST_REQUIRE(rawItem.get() != rawItemOut);
    BOOST_TEST_REQUIRE(rawItemOut->getData()[0] == 42u);
    // Invalid indizes return NULL
    BOOST_TEST_REQUIRE(!archiv[1]);
    // Add with passing ownership
    auto* rawItemPtr = rawItem.get();
    archiv.push(std::move(rawItem));
    BOOST_TEST_REQUIRE(archiv[1] == rawItemPtr);
    archiv.clear();
    BOOST_TEST_REQUIRE(archiv.empty());
}

BOOST_AUTO_TEST_CASE(Set)
{
    libsiedler2::Archiv archiv;
    BOOST_TEST_REQUIRE(archiv.empty());
    archiv.push(std::make_unique<libsiedler2::ArchivItem_Raw>());
    archiv.alloc(2);
    BOOST_TEST_REQUIRE(archiv.size() == 2u);
    // Archiv should have been cleared
    BOOST_TEST_REQUIRE(!archiv[0]);
    auto rawItem = std::make_unique<libsiedler2::ArchivItem_Raw>();
    rawItem->getData().push_back(42);
    archiv.setC(0, *rawItem);
    auto* rawItemOut = dynamic_cast<libsiedler2::ArchivItem_Raw*>(archiv[0]);
    BOOST_TEST_REQUIRE(rawItemOut);
    // Should have copied
    BOOST_TEST_REQUIRE(rawItem.get() != rawItemOut);
    BOOST_TEST_REQUIRE(rawItemOut->getData()[0] == 42u);
    // Invalid indices return NULL
    BOOST_TEST_REQUIRE(!archiv[1]);
    archiv.alloc_inc(3);
    BOOST_TEST_REQUIRE(archiv.size() == 5u);
    BOOST_TEST_REQUIRE(archiv[0] == rawItemOut);
    // Throw out of range exception
    BOOST_REQUIRE_THROW(archiv.set(5, std::make_unique<libsiedler2::ArchivItem_Raw>()), std::out_of_range);
    // Add with passing ownership
    auto* rawPtr = rawItem.get();
    archiv.set(4, std::move(rawItem));
    BOOST_TEST_REQUIRE(archiv[4] == rawPtr);
    archiv.set(4, nullptr);
    BOOST_TEST_REQUIRE(!archiv[4]);
}

BOOST_AUTO_TEST_CASE(Find)
{
    libsiedler2::Archiv archiv;
    auto rawItem1 = std::make_unique<libsiedler2::ArchivItem_Raw>();
    rawItem1->setName("Foo1");
    auto rawItem2 = std::make_unique<libsiedler2::ArchivItem_Raw>();
    rawItem2->setName("Foo2");
    auto rawItem3 = std::make_unique<libsiedler2::ArchivItem_Raw>();
    rawItem3->setName("Foo3");
    auto* rawPtr1 = rawItem1.get();
    auto* rawPtr2 = rawItem2.get();
    auto* rawPtr3 = rawItem3.get();
    archiv.push(std::move(rawItem1));
    archiv.push(std::move(rawItem2));
    archiv.push(nullptr);
    archiv.push(std::move(rawItem3));
    BOOST_TEST_REQUIRE(archiv.find(rawPtr1->getName()) == rawPtr1);
    BOOST_TEST_REQUIRE(archiv.find(rawPtr2->getName()) == rawPtr2);
    BOOST_TEST_REQUIRE(archiv.find(rawPtr3->getName()) == rawPtr3);
    BOOST_TEST_REQUIRE(!archiv.find("NonExistant"));
}

BOOST_AUTO_TEST_CASE(CreateAllTypesAndCopy)
{
    using namespace libsiedler2;
    libsiedler2::Archiv archiv;
    for(unsigned i = 1; i < NUM_BOB_TYPES; i++)
    {
        auto bobType = BobType(i);
        if(bobType == BobType::Unset || bobType == BobType::Sound)
            continue;
        auto item = getAllocator().create(bobType);
        BOOST_TEST_REQUIRE(item);
        item->setName("Item" + std::to_string(i));
        archiv.push(std::move(item));
    }
    for(SoundType soundType :
        {SoundType::Wave, SoundType::Midi, SoundType::XMidi, SoundType::MP3, SoundType::OGG, SoundType::Other})
    {
        auto item = libsiedler2::getAllocator().create(BobType::Sound, soundType);
        BOOST_TEST_REQUIRE(item);
        item->setName("Sound" + std::to_string(static_cast<int>(soundType)));
        archiv.push(std::move(item));
    }
    // Copy ctor
    libsiedler2::Archiv archiv2(archiv), archiv3;
    BOOST_TEST_REQUIRE(archiv.size() == archiv2.size());
    // Avoid ctor init
    archiv3.push(std::make_unique<libsiedler2::ArchivItem_Raw>());
    // Assign
    archiv3 = archiv;
    BOOST_TEST_REQUIRE(archiv.size() == archiv3.size());
    // All should be equal
    for(unsigned i = 0; i < archiv.size(); i++)
    {
        BOOST_TEST_INFO_SCOPE("Item" << i);

        BOOST_TEST_REQUIRE(archiv[i]);
        BOOST_TEST_REQUIRE(archiv2[i]);
        BOOST_TEST_REQUIRE(archiv3[i]);
        // Items should have been copied
        BOOST_TEST(archiv[i] != archiv2[i]);
        BOOST_TEST(archiv[i] != archiv3[i]);
        BOOST_TEST(archiv[i]->getBobType() == archiv2[i]->getBobType());
        BOOST_TEST(archiv[i]->getBobType() == archiv3[i]->getBobType());
        BOOST_TEST(archiv[i]->getName() == archiv2[i]->getName());
        BOOST_TEST(archiv[i]->getName() == archiv3[i]->getName());
    }
}

struct TestItem : libsiedler2::ArchivItem
{
    static int numLiveItems;
    int i;
    TestItem(int i = 0) : i(i) { numLiveItems++; }
    TestItem(const TestItem& o) : libsiedler2::ArchivItem(o), i(o.i) { numLiveItems++; }
    ~TestItem() override { numLiveItems--; }
    RTTR_CLONEABLE(TestItem)
};

static_assert(!std::is_copy_constructible<libsiedler2::ArchivItem>::value, "Should not be Copyable/Movable");
static_assert(!std::is_move_constructible<libsiedler2::ArchivItem>::value, "Should not be Copyable/Movable");
static_assert(!std::is_copy_assignable<libsiedler2::ArchivItem>::value, "Should not be Copyable/Movable");
static_assert(!std::is_move_assignable<libsiedler2::ArchivItem>::value, "Should not be Copyable/Movable");

int TestItem::numLiveItems = 0;

BOOST_AUTO_TEST_CASE(AllocAndGet)
{
    libsiedler2::Archiv archiv;
    BOOST_TEST_REQUIRE(archiv.empty());
    archiv.alloc(3);
    BOOST_TEST_REQUIRE(archiv.size() == 3u);
    BOOST_TEST_REQUIRE(!archiv[0]);
    BOOST_TEST_REQUIRE(!archiv[1]);
    BOOST_TEST_REQUIRE(!archiv[2]);
    archiv.set(1, std::make_unique<libsiedler2::ArchivItem_Raw>());
    BOOST_TEST_REQUIRE(archiv[1]);
    BOOST_TEST_REQUIRE(archiv[1] == archiv.get(1));
    archiv.set(2, std::make_unique<libsiedler2::ArchivItem_Raw>());
    BOOST_TEST_REQUIRE(archiv.size() == 3u);
    BOOST_TEST_REQUIRE(archiv[2] == archiv.get(2));
    BOOST_REQUIRE_THROW(archiv.set(3, nullptr), std::out_of_range);
    BOOST_TEST_REQUIRE(!archiv[3]);
    BOOST_TEST_REQUIRE(!archiv.get(3));
    archiv.alloc_inc(2);
    BOOST_TEST_REQUIRE(archiv.size() == 5u);
    BOOST_TEST_REQUIRE(archiv[1]);
    BOOST_TEST_REQUIRE(archiv[2]);
    BOOST_TEST_REQUIRE(!archiv[3]);
    archiv.alloc(5);
    BOOST_TEST_REQUIRE(archiv.size() == 5u);
    for(unsigned i = 0; i < 5; i++)
        BOOST_TEST_REQUIRE(!archiv[i]);
    // Auto delete
    {
        libsiedler2::Archiv archiv2;
        BOOST_TEST_REQUIRE(TestItem::numLiveItems == 0);
        archiv2.push(std::make_unique<TestItem>());
        BOOST_TEST_REQUIRE(TestItem::numLiveItems == 1);
    }
    BOOST_TEST_REQUIRE(TestItem::numLiveItems == 0);
    // No delete
    std::unique_ptr<libsiedler2::ArchivItem> item = std::make_unique<TestItem>();
    {
        libsiedler2::Archiv archiv2;
        BOOST_TEST_REQUIRE(TestItem::numLiveItems == 1);
        auto* rawPtr = item.get();
        archiv2.push(std::move(item));
        item = archiv2.release(0);
        BOOST_TEST_REQUIRE(item.get() == rawPtr);
    }
    BOOST_TEST_REQUIRE(TestItem::numLiveItems == 1);
}

BOOST_AUTO_TEST_CASE(RangeBasedIteration)
{
    libsiedler2::Archiv archive;
    archive.alloc(5);
    archive.set(0, std::make_unique<libsiedler2::ArchivItem_Raw>(std::vector<uint8_t>(1)));
    archive.set(2, std::make_unique<libsiedler2::ArchivItem_Raw>(std::vector<uint8_t>(42)));
    archive.set(3, std::make_unique<libsiedler2::ArchivItem_Raw>(std::vector<uint8_t>(1337)));
    std::vector<std::unique_ptr<libsiedler2::ArchivItem>> items;
    for(auto& i : archive)
        items.push_back(std::move(i));
    BOOST_TEST(!archive[0]);
    BOOST_TEST(!archive[2]);
    BOOST_TEST(!archive[3]);
    BOOST_TEST_REQUIRE(items.size() == 5u);
    // .get() is a workaround for a printing bug until Boost 1.69
    BOOST_TEST(items[0].get());
    BOOST_TEST(!items[1].get());
    BOOST_TEST(items[2].get());
    BOOST_TEST(items[3].get());
    BOOST_TEST(!items[4].get());
    BOOST_TEST(static_cast<libsiedler2::ArchivItem_Raw&>(*items[0]).getData().size() == 1u);
    BOOST_TEST(static_cast<libsiedler2::ArchivItem_Raw&>(*items[2]).getData().size() == 42u);
    BOOST_TEST(static_cast<libsiedler2::ArchivItem_Raw&>(*items[3]).getData().size() == 1337u);
}

BOOST_AUTO_TEST_CASE(CopyArchive)
{
    {
        libsiedler2::Archiv archiv;
        archiv.alloc(3);
        archiv.set(0, std::make_unique<TestItem>(42));
        archiv.set(2, std::make_unique<TestItem>(1337));
        libsiedler2::Archiv archiv2 = archiv;
        BOOST_TEST_REQUIRE(archiv2.size() == archiv.size());
        BOOST_TEST_REQUIRE(archiv2[0]);
        BOOST_TEST_REQUIRE(!archiv2[1]);
        BOOST_TEST_REQUIRE(archiv2[2]);
        BOOST_TEST(dynamic_cast<TestItem&>(*archiv2[0]).i == 42);
        BOOST_TEST(dynamic_cast<TestItem&>(*archiv2[2]).i == 1337);

        // Move
        libsiedler2::Archiv archiv3 = std::move(archiv2);
        BOOST_TEST_REQUIRE(archiv3.size() == archiv.size());
        BOOST_TEST_REQUIRE(archiv3[0]);
        BOOST_TEST_REQUIRE(!archiv3[1]);
        BOOST_TEST_REQUIRE(archiv3[2]);
        BOOST_TEST(dynamic_cast<TestItem&>(*archiv3[0]).i == 42);
        BOOST_TEST(dynamic_cast<TestItem&>(*archiv3[2]).i == 1337);
    }
    // No leaks
    BOOST_TEST(TestItem::numLiveItems == 0);
}

BOOST_AUTO_TEST_SUITE_END()
