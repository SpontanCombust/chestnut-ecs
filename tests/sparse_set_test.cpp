#include <catch2/catch.hpp>

#include "../include/chestnut/ecs/sparse_set.hpp"


using namespace chestnut::ecs::internal;

struct FooComp
{
    int i;

    inline bool operator==(const FooComp& other) const
    {
        return this->i == other.i;
    }
};

TEST_CASE("Sparse set test")
{
    SECTION("Default constructor")
    {
        CSparseSet<FooComp> set;

        REQUIRE(set.sparse().size() == 0);
        REQUIRE(set.dense().size() == 0);
        REQUIRE(set.empty());
        REQUIRE(set.denseSize() == 0);
    }

    SECTION("Initial sparse size constructor")
    {
        CSparseSet<FooComp> set(10);

        REQUIRE(set.sparse().size() == 10);
        REQUIRE(set.dense().size() == 0);
        REQUIRE(set.empty());
        REQUIRE(set.denseSize() == 0);
    }



    CSparseSet<FooComp> testSet(10);

    SECTION("Insertion")
    {
        testSet.insert(0, FooComp{1});
        testSet.insert(1, FooComp{2});
        testSet.insert(3, FooComp{3});

        REQUIRE(testSet.contains(0));
        REQUIRE(testSet.contains(1));
        REQUIRE_FALSE(testSet.contains(2));
        REQUIRE_FALSE(testSet.empty());
        REQUIRE(testSet.denseSize() == 3);

        REQUIRE(testSet.sparse().size() == 10);
        REQUIRE(testSet.sparse()[0] == 0);
        REQUIRE(testSet.sparse()[1] == 1);
        REQUIRE(testSet.sparse()[2] == CSparseSet<int>::NIL_INDEX);
        REQUIRE(testSet.sparse()[3] == 2);

        REQUIRE(testSet.dense().size() == 3);
        REQUIRE(testSet.dense()[0].e == FooComp{1});
        REQUIRE(testSet.dense()[1].e == FooComp{2});
        REQUIRE(testSet.dense()[2].e == FooComp{3});
    }

    SECTION("Lookup")
    {
        testSet.insert(0, FooComp{1});
        testSet.insert(1, FooComp{2});
        testSet.insert(3, FooComp{3});

        REQUIRE(testSet.at(0).has_value());
        REQUIRE(*testSet.at(0).value() == FooComp{1});
        REQUIRE(testSet.at(1).has_value());
        REQUIRE(*testSet.at(1).value() == FooComp{2});
        REQUIRE_FALSE(testSet.at(2).has_value());
        REQUIRE(testSet.at(3).has_value());
        REQUIRE(*testSet.at(3).value() == FooComp{3});
    }

    SECTION("Const lookup")
    {
        testSet.insert(0, FooComp{1});
        testSet.insert(1, FooComp{2});
        testSet.insert(3, FooComp{3});

        const CSparseSet<FooComp>& constTestSet = testSet;
        REQUIRE(constTestSet.at(0).has_value());
        REQUIRE(*constTestSet.at(0).value() == FooComp{1});
        REQUIRE(constTestSet.at(1).has_value());
        REQUIRE(*constTestSet.at(1).value() == FooComp{2});
        REQUIRE_FALSE(constTestSet.at(2).has_value());
        REQUIRE(constTestSet.at(3).has_value());
        REQUIRE(*constTestSet.at(3).value() == FooComp{3});
    }

    SECTION("Erasure")
    {
        testSet.insert(0, FooComp{0});
        testSet.insert(1, FooComp{1});
        testSet.insert(2, FooComp{2});
        testSet.insert(3, FooComp{3});

        // i: 0  1  2  3
        // d: 0  1  2  3
        // s: 0  1  2  3

        testSet.erase(1);
        // i: 0  1  2  3
        // d: 0  3  2
        // s: 0 -1  2  1

        testSet.erase(2);
        // i: 0  1  2  3
        // d: 0  3
        // s: 0 -1 -1  1

        REQUIRE(testSet.contains(0));
        REQUIRE_FALSE(testSet.contains(1));
        REQUIRE_FALSE(testSet.contains(2));
        REQUIRE(testSet.contains(3));
        REQUIRE(testSet.denseSize() == 2);

        REQUIRE(testSet.sparse()[0] == 0);
        REQUIRE(testSet.sparse()[1] == CSparseSet<int>::NIL_INDEX);
        REQUIRE(testSet.sparse()[2] == CSparseSet<int>::NIL_INDEX);
        REQUIRE(testSet.sparse()[3] == 1);

        REQUIRE(testSet.dense().size() == 2);
        REQUIRE(testSet.dense()[0].e == FooComp{0});
        REQUIRE(testSet.dense()[1].e == FooComp{3});
    }



    SECTION("Clearing")
    {
        testSet.insert(0, FooComp{0});
        testSet.insert(1, FooComp{1});
        testSet.insert(2, FooComp{2});
        testSet.insert(3, FooComp{3});

        testSet.clear();

        REQUIRE(testSet.empty());
        REQUIRE(testSet.denseSize() == 0);

        REQUIRE(testSet.sparse()[0] == CSparseSet<int>::NIL_INDEX);
        REQUIRE(testSet.sparse()[1] == CSparseSet<int>::NIL_INDEX);
        REQUIRE(testSet.sparse()[2] == CSparseSet<int>::NIL_INDEX);
        REQUIRE(testSet.sparse()[3] == CSparseSet<int>::NIL_INDEX);

        REQUIRE(testSet.dense().size() == 0);
    }
}