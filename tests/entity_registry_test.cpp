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
    CComponentStorage storage;
    CEntityRegistry registry(&storage);

    SECTION("Check if registered")
    {
        auto ent1 = registry.registerNewEntity();
        auto ent2 = registry.registerNewEntity();
        auto ent3 = registry.registerNewEntity();

        REQUIRE(registry.isEntityRegistered(ent1));
        REQUIRE(registry.isEntityRegistered(ent2));
        REQUIRE(registry.isEntityRegistered(ent3));
        

        registry.unregisterEntity(ent1);
        registry.unregisterEntity(ent3);

        REQUIRE_FALSE(registry.isEntityRegistered(ent1));
        REQUIRE(registry.isEntityRegistered(ent2));
        REQUIRE_FALSE(registry.isEntityRegistered(ent3));
    }

    SECTION("ID recycling")
    {
        /*auto ent1 = */registry.registerNewEntity();
        auto ent2 = registry.registerNewEntity();
        auto ent3 = registry.registerNewEntity();

        registry.unregisterEntity(ent2);
        auto ent4 = registry.registerNewEntity(true);
        REQUIRE(ent2 == ent4);

        registry.unregisterEntity(ent3);
        auto ent5 = registry.registerNewEntity(false);
        REQUIRE(ent3 != ent5);
    }

    SECTION("Get signature")
    {
        auto ent1 = registry.registerNewEntity();
        // ent1 will have no components

        auto ent2 = registry.registerNewEntity();
        storage.insert(ent2, FooComp{});

        auto ent3 = registry.registerNewEntity();
        storage.insert(ent3, FooComp{});
        storage.insert(ent3, BazComp{});


        auto sign1 = registry.getEntitySignature(ent1);
        REQUIRE(sign1.isEmpty());

        auto sign2 = registry.getEntitySignature(ent2);
        REQUIRE((sign2.has<FooComp>() && !sign2.has<BarComp, BazComp>()));

        auto sign3 = registry.getEntitySignature(ent3);
        REQUIRE((sign3.has<FooComp, BazComp>() && !sign2.has<BarComp>()));


        registry.unregisterEntity(ent3);
        sign3 = registry.getEntitySignature(ent3);
        REQUIRE(sign3.isEmpty());
    }

    SECTION("Count entities - total")
    {
        REQUIRE(registry.getEntityCount() == 0);

        std::vector<entityid_t> ids;
        for (size_t i = 0; i < 1000; i++)
        {
            ids.push_back(registry.registerNewEntity());
        }
        
        REQUIRE(registry.getEntityCount() == 1000);

        for (size_t i = 100; i < 200; i++)
        {
            registry.unregisterEntity(ids[i]);
        }

        REQUIRE(registry.getEntityCount() == 900);
    }

    SECTION("Count entities - with signature")
    {
        for (size_t i = 0; i < 50; i++)
        {
            auto ent = registry.registerNewEntity();
            storage.insert(ent, FooComp{});
        }

        for (size_t i = 0; i < 100; i++)
        {
            auto ent = registry.registerNewEntity();
            storage.insert(ent, FooComp{});
            storage.insert(ent, BazComp{});
        }

        for (size_t i = 0; i < 10; i++)
        {
            auto ent = registry.registerNewEntity();
            storage.insert(ent, BarComp{});
            storage.insert(ent, BazComp{});
        }


        SECTION("partial")
        {
            CEntitySignature sign;

            sign = makeEntitySignature<FooComp>();
            REQUIRE(registry.getEntityCountOfPartialSignature(sign) == 150);

            sign = makeEntitySignature<BarComp>();
            REQUIRE(registry.getEntityCountOfPartialSignature(sign) == 10);

            sign = makeEntitySignature<FooComp, BarComp>();
            REQUIRE(registry.getEntityCountOfPartialSignature(sign) == 0);
        }

        SECTION("exact")
        {
            CEntitySignature sign;

            sign = makeEntitySignature<FooComp>();
            REQUIRE(registry.getEntityCountOfExactSignature(sign) == 50);

            sign = makeEntitySignature<BarComp>();
            REQUIRE(registry.getEntityCountOfExactSignature(sign) == 0);

            sign = makeEntitySignature<FooComp, BarComp>();
            REQUIRE(registry.getEntityCountOfExactSignature(sign) == 0);

            sign = makeEntitySignature<FooComp, BazComp>();
            REQUIRE(registry.getEntityCountOfExactSignature(sign) == 100);
        }
    }
    

    SECTION("Find entities")
    {
        for (size_t i = 0; i < 10; i++)
        {
            auto ent = registry.registerNewEntity();
            storage.insert(ent, FooComp{});
            storage.insert(ent, BazComp{});
        }

        for (size_t i = 0; i < 50; i++)
        {
            auto ent = registry.registerNewEntity();
            storage.insert(ent, FooComp{});
            storage.insert(ent, BarComp{});
        }

        for (size_t i = 0; i < 100; i++)
        {
            auto ent = registry.registerNewEntity();
            storage.insert(ent, BarComp{});
            storage.insert(ent, BazComp{});
        }


        auto ents1 = registry.findEntities([](auto _) {
            return true;
        });

        REQUIRE(ents1.size() == 160);


        auto ents2 = registry.findEntities([](const CEntitySignature& sign) {
            return sign.has<FooComp>() && !sign.has<BazComp>();
        });

        REQUIRE(ents2.size() == 50);
    }
}