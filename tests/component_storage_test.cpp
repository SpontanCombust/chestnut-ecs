#include <catch2/catch.hpp>

#include "../include/chestnut/ecs/component_storage.hpp"

using namespace chestnut::ecs;
using namespace chestnut::ecs::internal;

struct FooComp
{
    int i = 0;

    inline bool operator==(const FooComp& other) const
    {
        return this->i == other.i;
    }
};

struct BarComp
{
    char c = 0;

    inline bool operator==(const BarComp& other) const
    {
        return this->c == other.c;
    }
};

struct BazComp
{
    short s1 = 0;
    short s2 = 0;

    inline bool operator==(const BazComp& other) const
    {
        return this->s1 == other.s1 && this->s2 == other.s2;
    }
};


TEST_CASE( "Component storage test" )
{
    CComponentStorage storage;

    SECTION("Initial state")
    {
        REQUIRE(storage.empty<FooComp>());
        REQUIRE(storage.size<FooComp>() == 0);

        REQUIRE(storage.empty<BarComp>());
        REQUIRE(storage.size<BarComp>() == 0);

        REQUIRE(storage.empty<BazComp>());
        REQUIRE(storage.size<BazComp>() == 0);
    }

    SECTION("Insertion")
    {
        REQUIRE_FALSE(storage.contains<FooComp>(0));
        REQUIRE_FALSE(storage.contains<FooComp>(1));
        REQUIRE_FALSE(storage.contains<BarComp>(1));
        REQUIRE_FALSE(storage.contains<BarComp>(2));
        REQUIRE_FALSE(storage.contains<BazComp>(3));

        storage.insert<FooComp>(0, {0});
        storage.insert<FooComp>(1);
        storage.insert<FooComp>(1, {1});
        storage.insert<BarComp>(1);
        storage.insert<BarComp>(2, {2});
        storage.insert<BazComp>(3, {3, 3});

        REQUIRE(storage.contains<FooComp>(0));
        REQUIRE(storage.contains<FooComp>(1));
        REQUIRE(storage.contains<BarComp>(1));
        REQUIRE(storage.contains<BarComp>(2));
        REQUIRE(storage.contains<BazComp>(3));


        REQUIRE(storage.size<FooComp>() == 2);
        REQUIRE(storage.size<BarComp>() == 2);
        REQUIRE(storage.size<BazComp>() == 1);       
    }

    SECTION("Lookup")
    {
        storage.insert<FooComp>(0, {0});
        storage.insert<FooComp>(1);
        storage.insert<FooComp>(1, {1});
        storage.insert<BarComp>(1);
        storage.insert<BarComp>(2, {2});
        storage.insert<BazComp>(3, {3, 3});

        SECTION("Mutable")
        {
            REQUIRE(storage.at<FooComp>(0).has_value());
            REQUIRE(*storage.at<FooComp>(0).value() == FooComp{0});

            REQUIRE(storage.at<FooComp>(1).has_value());
            REQUIRE(*storage.at<FooComp>(1).value() == FooComp{1});

            REQUIRE(storage.at<BarComp>(1).has_value());
            REQUIRE(*storage.at<BarComp>(1).value() == BarComp{});

            REQUIRE(storage.at<BarComp>(2).has_value());
            REQUIRE(*storage.at<BarComp>(2).value() == BarComp{2});

            REQUIRE_FALSE(storage.at<BazComp>(2).has_value());

            REQUIRE(storage.at<BazComp>(3).has_value());
            REQUIRE(*storage.at<BazComp>(3).value() == BazComp{3, 3});
        }

        SECTION("Const")
        {
            const auto& constStorage = storage;

            REQUIRE(constStorage.at<FooComp>(0).has_value());
            REQUIRE(*constStorage.at<FooComp>(0).value() == FooComp{0});

            REQUIRE(constStorage.at<FooComp>(1).has_value());
            REQUIRE(*constStorage.at<FooComp>(1).value() == FooComp{1});

            REQUIRE(constStorage.at<BarComp>(1).has_value());
            REQUIRE(*constStorage.at<BarComp>(1).value() == BarComp{});

            REQUIRE(constStorage.at<BarComp>(2).has_value());
            REQUIRE(*constStorage.at<BarComp>(2).value() == BarComp{2});

            REQUIRE_FALSE(constStorage.at<BazComp>(2).has_value());

            REQUIRE(constStorage.at<BazComp>(3).has_value());
            REQUIRE(constStorage.at<BazComp>(3).has_value());
            REQUIRE(*constStorage.at<BazComp>(3).value() == BazComp{3, 3});   
        }
    }

    SECTION("Erase and clear")
    {
        storage.insert<FooComp>(0, {0});
        storage.insert<FooComp>(1);
        storage.insert<FooComp>(1, {1});
        storage.insert<BarComp>(1);
        storage.insert<BarComp>(2, {2});
        storage.insert<BazComp>(3, {3, 3});

        storage.erase<FooComp>(1);
        storage.erase<BarComp>(1);
        storage.erase<BarComp>(2);

        REQUIRE(storage.contains<FooComp>(0));
        REQUIRE_FALSE(storage.contains<FooComp>(1));
        REQUIRE_FALSE(storage.contains<BarComp>(1));
        REQUIRE_FALSE(storage.contains<BarComp>(2));
        REQUIRE(storage.contains<BazComp>(3));


        storage.clear<BazComp>();
        REQUIRE_FALSE(storage.contains<BazComp>(3));
    }

    SECTION("EraseAll")
    {
        storage.insert<FooComp>(0, {0});
        storage.insert<FooComp>(1);
        storage.insert<FooComp>(1, {1});
        storage.insert<BarComp>(1);
        storage.insert<BarComp>(2, {2});
        storage.insert<BazComp>(3, {3, 3});

        storage.eraseAll(0);
        storage.eraseAll(1);

        REQUIRE_FALSE(storage.contains<FooComp>(0));
        REQUIRE_FALSE(storage.contains<FooComp>(1));
        REQUIRE_FALSE(storage.contains<BarComp>(1));
        REQUIRE(storage.contains<BarComp>(2));
        REQUIRE(storage.contains<BazComp>(3));
    }

    SECTION("Signature")
    {
        storage.insert<FooComp>(0, {0});
        storage.insert<FooComp>(1);
        storage.insert<FooComp>(1, {1});
        storage.insert<BarComp>(1);
        storage.insert<BarComp>(2, {2});
        storage.insert<BazComp>(3, {3, 3});

        auto sign0 = storage.signature(0);
        REQUIRE((sign0.has<FooComp>() && !sign0.has<BarComp, BazComp>()));

        auto sign1 = storage.signature(1);
        REQUIRE((sign1.has<FooComp, BarComp>() && !sign1.has<BazComp>()));

        auto sign2 = storage.signature(2);
        REQUIRE((sign2.has<BarComp>() && !sign2.has<FooComp, BazComp>()));

        auto sign3 = storage.signature(3);
        REQUIRE((sign3.has<BazComp>() && !sign3.has<FooComp, BarComp>()));
    }
}