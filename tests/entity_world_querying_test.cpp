#include <catch2/catch.hpp>

#include "../include/chestnut/ecs/entity_world.hpp"

using namespace chestnut::ecs;
using namespace chestnut::ecs::internal;

class Foo
{
public:
    int x;
};

class Bar
{
public:
    long y;
};

class Baz
{
public:
    char z;
    short w;
};



TEST_CASE( "Entity world test - querying" )
{
    CEntityWorld world;
    std::vector<entityid_t> vEnts;

    // 10 entities with Foo
    for (size_t i = 0; i < 10; i++)
    {
        entityid_t ent = world.createEntity();
        world.createComponent<Foo>( ent )->x = i;
        vEnts.push_back(ent);
    }
    // 10 with Foo and Bar
    for (size_t i = 10; i < 20; i++)
    {
        entityid_t ent = world.createEntity();
        world.createComponent<Foo>( ent )->x = i;
        world.createComponent<Bar>( ent )->y = i + 1;
        vEnts.push_back(ent);
    }
    // 10 with Bar and Baz
    for (size_t i = 20; i < 30; i++)
    {
        entityid_t ent = world.createEntity();
        world.createComponent<Bar>( ent )->y = i;
        world.createComponent<Baz>( ent )->z = i + 1;
        world.createComponent<Baz>( ent )->w = i + 2;
        vEnts.push_back(ent);
    }
    // 10 with Foo and Baz
    for (size_t i = 30; i < 40; i++)
    {
        entityid_t ent = world.createEntity();
        world.createComponent<Foo>( ent )->x = i;
        world.createComponent<Baz>( ent )->z = i + 1;
        world.createComponent<Baz>( ent )->w = i + 2;
        vEnts.push_back(ent);
    }
    // 10 with Foo, Bar and Baz
    for (size_t i = 40; i < 50; i++)
    {
        entityid_t ent = world.createEntity();
        world.createComponent<Foo>( ent )->x = i;
        world.createComponent<Bar>( ent )->y = i + 1;
        world.createComponent<Baz>( ent )->z = i + 2;
        world.createComponent<Baz>( ent )->w = i + 3;
        vEnts.push_back(ent);
    }
    

    SECTION( "Check for null" )
    {
        REQUIRE( world.queryEntities(nullptr) == nullptr );

        auto q = world.createQuery( CEntitySignature(), CEntitySignature() );

        REQUIRE( world.queryEntities(q) != nullptr );

        world.destroyQuery(q);

        REQUIRE( world.queryEntities(q) == nullptr );        
    }

    SECTION( "Query entities with Foo" )
    {
        auto q = world.createQuery( makeEntitySignature<Foo>(), makeEntitySignature<>() );
        world.queryEntities(q);

        REQUIRE( q->getEntityCount() == 40 );

        for(auto it = q->begin<Foo>(); it != q->end<Foo>(); it++)
        {
            std::apply([it](Foo& foo) {
                REQUIRE(( foo.x < 20 || foo.x >= 30 ));
                foo.x *= 2;
            }, *it);
        }

        REQUIRE(world.getComponent<Foo>(vEnts[0])->x == 0);
        REQUIRE(world.getComponent<Foo>(vEnts[10])->x == 20);
        REQUIRE(world.getComponent<Foo>(vEnts[30])->x == 60);

        world.destroyQuery(q);
    }

    SECTION( "Query entities with Foo, Bar and Baz" )
    {
        auto q = world.createQuery( makeEntitySignature<Foo, Bar, Baz>(), makeEntitySignature<>() );
        world.queryEntities(q);

        REQUIRE( q->getEntityCount() == 10 );

        for(auto it = q->begin<Foo, Bar, Baz>(); it != q->end<Foo, Bar, Baz>(); it++)
        {
            std::apply([it](Foo& foo, Bar& bar, Baz& baz) {
                REQUIRE(( foo.x >= 40 && foo.x < 50 ));
                foo.x *= 2;
                bar.y *= 2;
                baz.w *= 2;
            }, *it);
        }

        REQUIRE(world.getComponent<Foo>(vEnts[40])->x == 80);
        REQUIRE(world.getComponent<Bar>(vEnts[45])->y == 92);
        REQUIRE(world.getComponent<Baz>(vEnts[49])->w == 104);

        world.destroyQuery(q);
    }

    SECTION( "Query entities with Baz but no Foo" )
    {
        auto q = world.createQuery( makeEntitySignature<Baz>(), makeEntitySignature<Foo>() );
        world.queryEntities(q);

        REQUIRE( q->getEntityCount() == 10 );

        for(auto it = q->begin<Baz>(); it != q->end<Baz>(); it++)
        {
            std::apply([it](Baz& baz) {
                REQUIRE(( baz.z >= 21 && baz.z < 31 ));
            }, *it);
        }

        world.destroyQuery(q);
    }

    SECTION( "Query non-existing entities" )
    {
        auto q = world.createQuery( makeEntitySignature<Bar>(), makeEntitySignature<Foo, Baz>() );

        world.queryEntities(q);

        REQUIRE( q->getEntityCount() == 0 );

        world.destroyQuery(q);
    }
}
