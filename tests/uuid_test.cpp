#include <catch2/catch.hpp>

#include "../include/chestnut/ecs/uuid.hpp"

using namespace chestnut::ecs;


#include <algorithm>
#include <vector>

TEST_CASE("UUID test")
{
    SECTION("uniqueness")
    {
        std::vector<CUniqueIdentifier> uuids;
        const size_t N = 10000000;

        for (size_t i = 0; i < N; i++)
        {
            uuids.push_back(CUniqueIdentifier());
        }
        
        std::sort(uuids.begin(), uuids.end());

        // should not find any two equal values
        REQUIRE(std::adjacent_find(uuids.begin(), uuids.end()) == uuids.end());
    }

    SECTION("equality")
    {
        CUniqueIdentifier uuid1;
        CUniqueIdentifier uuid2;
        CUniqueIdentifier uuid3 = uuid1;

        REQUIRE(uuid1 == uuid1);
        REQUIRE(uuid1 != uuid2);
        REQUIRE(uuid1 == uuid3);
        REQUIRE(uuid2 == uuid2);
        REQUIRE(uuid2 != uuid3);
        REQUIRE(uuid3 == uuid3);
    }
}