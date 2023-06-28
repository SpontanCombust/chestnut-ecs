#include <catch2/catch.hpp>

#include "../include/chestnut/ecs/component_storage.hpp"
#include "../include/chestnut/ecs/entity_registry.hpp"

using namespace chestnut::ecs;
using namespace chestnut::ecs::internal;

struct FooComp {};
struct BarComp {};
struct BazComp {};

TEST_CASE( "Entity registry test" )
{
    CEntityRegistry registry;

    SECTION("Check if registered")
    {
        auto ent1 = registry.registerNewEntity();
        auto ent2 = registry.registerNewEntity();
        auto ent3 = registry.registerNewEntity();

        REQUIRE(registry.isEntityRegistered(ent1.uuid));
        REQUIRE(registry.isEntityRegistered(ent2.uuid));
        REQUIRE(registry.isEntityRegistered(ent3.uuid));
        

        registry.unregisterEntity(ent1.uuid);
        registry.unregisterEntity(ent3.uuid);

        REQUIRE_FALSE(registry.isEntityRegistered(ent1.uuid));
        REQUIRE(registry.isEntityRegistered(ent2.uuid));
        REQUIRE_FALSE(registry.isEntityRegistered(ent3.uuid));
    }

    SECTION("ID recycling")
    {
        /*auto ent1 = */registry.registerNewEntity();
        auto ent2 = registry.registerNewEntity();
        auto ent3 = registry.registerNewEntity();

        registry.unregisterEntity(ent2.uuid);
        auto ent4 = registry.registerNewEntity();
        REQUIRE(ent2.slot == ent4.slot);
    }

    SECTION("Count entities - total")
    {
        REQUIRE(registry.getEntityCount() == 0);

        std::vector<CUniqueIdentifier> ids;
        for (size_t i = 0; i < 1000; i++)
        {
            ids.push_back(registry.registerNewEntity().uuid);
        }
        
        REQUIRE(registry.getEntityCount() == 1000);

        for (size_t i = 100; i < 200; i++)
        {
            registry.unregisterEntity(ids[i]);
        }

        REQUIRE(registry.getEntityCount() == 900);
    }
}