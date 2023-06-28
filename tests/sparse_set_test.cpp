#include <catch2/catch.hpp>

#include "../include/chestnut/ecs/sparse_set.hpp"


using namespace chestnut::ecs::internal;

TEST_CASE("Sparse set test")
{
    SECTION("Default constructor")
    {
        CSparseSet<int> set;

        REQUIRE(set.sparse().size() == 0);
        REQUIRE(set.dense().size() == 0);
        REQUIRE(set.empty());
        REQUIRE(set.denseSize() == 0);
    }

    SECTION("Initial sparse size constructor")
    {
        CSparseSet<int> set(10);

        REQUIRE(set.sparse().size() == 10);
        REQUIRE(set.dense().size() == 0);
        REQUIRE(set.empty());
        REQUIRE(set.denseSize() == 0);
    }



    CSparseSet<int> testSet(10);

    SECTION("Insertion")
    {
        testSet.insert(0, 1);
        testSet.insert(1, 2);
        testSet.insert(3, 3);

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
        REQUIRE(testSet.dense()[0].e == 1);
        REQUIRE(testSet.dense()[1].e == 2);
        REQUIRE(testSet.dense()[2].e == 3);
    }

    SECTION("Lookup")
    {
        testSet.insert(0, 1);
        testSet.insert(1, 2);
        testSet.insert(3, 3);

        REQUIRE(testSet.at(0) == 1);
        REQUIRE(testSet.at(1) == 2);
        REQUIRE_THROWS(testSet.at(2) == 3);
        REQUIRE(testSet.at(3) == 3);
    }

    SECTION("Erasure")
    {
        testSet.insert(0, 0);
        testSet.insert(1, 1);
        testSet.insert(2, 2);
        testSet.insert(3, 3);

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
        REQUIRE(testSet.dense()[0].e == 0);
        REQUIRE(testSet.dense()[1].e == 3);
    }



    SECTION("Clearing")
    {
        testSet.insert(0, 0);
        testSet.insert(1, 1);
        testSet.insert(2, 2);
        testSet.insert(3, 3);

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