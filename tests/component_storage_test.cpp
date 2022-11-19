#include <catch2/catch.hpp>

#include "../include/chestnut/ecs/component_storage.hpp"

using namespace chestnut::ecs;
using namespace chestnut::ecs::internal;

struct FooComp
{
    int i;
};

struct BarComp
{
    char c;
};

struct BazComp
{
    short s1;
    short s2;
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

        REQUIRE(storage.at<FooComp>(0).i == 0);
        REQUIRE(storage.at<FooComp>(1).i == 1);
        REQUIRE(storage.at<BarComp>(1).c == BarComp{}.c);
        REQUIRE(storage.at<BarComp>(2).c == 2);
        REQUIRE_THROWS(storage.at<BazComp>(2));
        REQUIRE((storage.at<BazComp>(3).s1 == 3 && storage.at<BazComp>(3).s2 == 3));
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

    SECTION("Iterator")
    {
        storage.insert<FooComp>(0, {0});
        storage.insert<FooComp>(1);
        storage.insert<FooComp>(1, {1});
        storage.insert<BarComp>(1);
        storage.insert<BarComp>(2, {2});
        storage.insert<BazComp>(3, {3, 3});

        auto it = storage.begin();
        REQUIRE(it != storage.end());
        REQUIRE(it.id() == 0);
        REQUIRE(it.contains<FooComp>());
        REQUIRE(it.get<FooComp>().i == 0);

        it++;
        REQUIRE(it.id() == 1);
        REQUIRE(it.contains<FooComp>());
        REQUIRE(it.contains<BarComp>());
        {
            auto sign = it.signature();
            REQUIRE(sign.has<FooComp, BarComp>());
        }
        REQUIRE(it.get<FooComp>().i == 1);
        REQUIRE(it.get<BarComp>().c == BarComp{}.c);
        it.set<BarComp>({1});
        REQUIRE(it.get<BarComp>().c == 1);

        ++it;
        REQUIRE(it.id() == 2);
        REQUIRE(it.contains<BarComp>());
        it.eraseAll();
        REQUIRE(!it.contains<BarComp>());

        ++it;
        REQUIRE(it.id() == 3);
        REQUIRE(it.contains<BazComp>());
        REQUIRE_THROWS(it.get<BarComp>());

        it++;
        REQUIRE(it == storage.end());

        it--;
        REQUIRE(it.id() == 3);
        REQUIRE(it.contains<BazComp>());
        REQUIRE_THROWS(it.get<BarComp>());
    }

    SECTION("ConstIterator")
    {
        storage.insert<FooComp>(0, {0});
        storage.insert<FooComp>(1);
        storage.insert<FooComp>(1, {1});
        storage.insert<BarComp>(1);
        storage.insert<BarComp>(2, {2});
        storage.insert<BazComp>(3, {3, 3});

        auto it = storage.cbegin();
        REQUIRE(it != storage.cend());
        REQUIRE(it.id() == 0);
        REQUIRE(it.contains<FooComp>());
        REQUIRE(it.get<FooComp>().i == 0);

        it++;
        REQUIRE(it.id() == 1);
        REQUIRE(it.contains<FooComp>());
        REQUIRE(it.contains<BarComp>());
        {
            auto sign = it.signature();
            REQUIRE(sign.has<FooComp, BarComp>());
        }
        REQUIRE(it.get<FooComp>().i == 1);
        REQUIRE(it.get<BarComp>().c == BarComp{}.c);

        ++it;
        REQUIRE(it.id() == 2);
        REQUIRE(it.contains<BarComp>());

        ++it;
        REQUIRE(it.id() == 3);
        REQUIRE(it.contains<BazComp>());
        REQUIRE_THROWS(it.get<BarComp>());

        it++;
        REQUIRE(it == storage.cend());

        it--;
        REQUIRE(it.id() == 3);
        REQUIRE(it.contains<BazComp>());
        REQUIRE_THROWS(it.get<BarComp>());
    }
}