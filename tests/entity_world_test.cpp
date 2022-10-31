#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include <catch2/catch.hpp>

#include "../include/chestnut/ecs/entity_world.hpp"

using namespace chestnut::ecs;

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


TEST_CASE( "Entity world test - general" )
{
    CEntityWorld world;

    SECTION( "Creating and checking empty entities" )
    {
        entityid_t e1, e2, e3;

        e1 = world.createEntity();
        e2 = world.createEntity();
        e3 = world.createEntity();

        REQUIRE_FALSE( world.hasEntity( ENTITY_ID_INVALID ) );
        REQUIRE( world.hasEntity(e1) );
        REQUIRE( world.hasEntity(e2) );
        REQUIRE( world.hasEntity(e3) );
    }

    SECTION( "Destroying empty entities" )
    {
        entityid_t e1, e2, e3;

        e1 = world.createEntity();
        e2 = world.createEntity();
        e3 = world.createEntity();

        REQUIRE( world.hasEntity(e1) );
        REQUIRE( world.hasEntity(e2) );
        REQUIRE( world.hasEntity(e3) );

        world.destroyEntity( e2 );

        REQUIRE( world.hasEntity(e1) );
        REQUIRE_FALSE( world.hasEntity(e2) );
        REQUIRE( world.hasEntity(e3) );
    }

    SECTION( "ID recycling" )
    {
        entityid_t ent1, ent2, ent3, ent4;

        ent1 = world.createEntity();
        ent2 = world.createEntity();
        
        world.destroyEntity( ent1 );
        world.destroyEntity( ent2 );

        ent3 = world.createEntity();
        ent4 = world.createEntity();

        REQUIRE( ent3 == ent2 );
        REQUIRE( ent4 == ent1 );
    }


    SECTION( "Creating components" )
    {
        // try create component for nonexistant entity
        REQUIRE_FALSE( world.createComponent<Foo>(1) );


        entityid_t ent = world.createEntity();
        CComponentHandle<Foo> foo;

        foo = world.createComponent<Foo>( ent );
        REQUIRE( foo );
        REQUIRE( foo.owner == ent );

        foo->x = 1;

        // check if returns already created component on repeated create
        foo = world.createComponent<Foo>( ent );
        REQUIRE( foo->x == 1 );

        REQUIRE( world.hasComponent<Foo>( ent ) );
    }

    SECTION( "Getting invalid components" )
    {
        auto foo = world.getComponent<Foo>(9127);

        REQUIRE_FALSE( foo );
        REQUIRE_THROWS( foo->x = 4 );
    }

    SECTION( "Getting components" )
    {
        entityid_t ent1 = world.createEntity();
        entityid_t ent2 = world.createEntity();
        entityid_t ent3 = world.createEntity();

        CComponentHandle<Foo> foo;
        CComponentHandle<Bar> bar;
        CComponentHandle<Baz> baz;


        foo = world.createComponent<Foo>( ent1 );
        foo->x = 1;

        foo = world.createComponent<Foo>( ent2 );
        foo->x = 2;
        baz = world.createComponent<Baz>( ent2 );
        baz->z = 3;
        baz->w = 4;

        bar = world.createComponent<Bar>( ent3 );
        bar->y = 5;
        baz = world.createComponent<Baz>( ent3 );
        baz->z = 6;
        baz->w = 7;


        REQUIRE( world.hasComponent<Foo>( ent1 ) );
        REQUIRE_FALSE( world.hasComponent<Bar>( ent1 ) );
        REQUIRE_FALSE( world.hasComponent<Baz>( ent1 ) );

        REQUIRE( world.hasComponent<Foo>( ent2 ) );
        REQUIRE_FALSE( world.hasComponent<Bar>( ent2 ) );
        REQUIRE( world.hasComponent<Baz>( ent2 ) );

        REQUIRE_FALSE( world.hasComponent<Foo>( ent3 ) );
        REQUIRE( world.hasComponent<Bar>( ent3 ) );
        REQUIRE( world.hasComponent<Baz>( ent3 ) );


        foo = world.getComponent<Foo>( ent1 );
        REQUIRE( foo->x == 1 );
        bar = world.getComponent<Bar>( ent1 );
        REQUIRE_FALSE( bar );
        baz = world.getComponent<Baz>( ent1 );

        foo = world.getComponent<Foo>( ent2 );
        REQUIRE( foo->x == 2 );
        bar = world.getComponent<Bar>( ent2 );
        REQUIRE_FALSE( bar );
        baz = world.getComponent<Baz>( ent2 );
        REQUIRE( baz->z == 3 );
        REQUIRE( baz->w == 4 );

        foo = world.getComponent<Foo>( ent3 );
        REQUIRE_FALSE( foo );
        bar = world.getComponent<Bar>( ent3 );
        REQUIRE( bar->y == 5 );
        baz = world.getComponent<Baz>( ent3 );
        REQUIRE( baz->z == 6 );
        REQUIRE( baz->w == 7 );
    }

    SECTION( "Destroying components" )
    {
        entityid_t ent1, ent2;

        ent1 = world.createEntity();
        ent2 = world.createEntity();


        world.createComponent<Foo>( ent1 );
        world.createComponent<Bar>( ent1 );

        world.createComponent<Bar>( ent2 );
        world.createComponent<Baz>( ent2 );


        world.destroyComponent<Bar>( ent1 );
        REQUIRE_FALSE( world.hasComponent<Bar>( ent1 ) );
        REQUIRE( world.hasComponent<Foo>( ent1 ) );

        world.destroyComponent<Bar>( ent2 );
        world.destroyComponent<Baz>( ent2 );
        REQUIRE_FALSE( world.hasComponent<Bar>( ent2 ) );
        REQUIRE_FALSE( world.hasComponent<Baz>( ent2 ) );
    }

    SECTION( "Destroying entities with components" )
    {
        entityid_t ent = world.createEntity();

        world.createComponent<Foo>( ent );
        world.createComponent<Bar>( ent );
        world.createComponent<Baz>( ent );

        world.destroyEntity( ent );


        REQUIRE_FALSE( world.hasComponent<Foo>( ent ) );
        REQUIRE_FALSE( world.hasComponent<Bar>( ent ) );
        REQUIRE_FALSE( world.hasComponent<Baz>( ent ) );
        REQUIRE_FALSE( world.hasEntity( ent ) );
    }


    SECTION( "Find entities" )
    {
        CEntityWorld world;

        std::vector< entityid_t > vecFooBar;
        std::vector< entityid_t > vecBarBaz;
        
        // 10 with Foo and Bar
        for (size_t i = 0; i < 10; i++)
        {
            entityid_t ent = world.createEntity();
            world.createComponent<Foo>( ent )->x = ent;
            world.createComponent<Bar>( ent )->y = ent + 1;
            vecFooBar.push_back( ent );
        }
        // 10 with Bar and Baz
        for (size_t i = 0; i < 10; i++)
        {
            entityid_t ent = world.createEntity();
            world.createComponent<Bar>( ent )->y = ent;
            world.createComponent<Baz>( ent )->z = ent + 1;
            vecBarBaz.push_back( ent );
        }


        std::vector< entityid_t > vecFound = world.findEntities( []( const CEntitySignature& sign ) 
        {
            return sign.has<Bar>();
        });

        REQUIRE( vecFound.size() == 20 );
    }
}




TEST_CASE( "Entity world test - benchmarks", "[benchmark]" )
{
    const entityid_t ENTITY_COUNT = 10000;
    CEntityWorld world;

    BENCHMARK( "Creating a lot of similair entities" )
    {
        CComponentHandle<Foo> foo;
        CComponentHandle<Bar> bar;
        CComponentHandle<Baz> baz;

        for (entityid_t i = 0; i < ENTITY_COUNT; i++)
        {
            entityid_t ent = world.createEntity();

            // every foo will be different
            foo = world.createComponent<Foo>( ent );
            foo->x = i;

            // but every bar and baz will be the same
            bar = world.createComponent<Bar>( ent );
            bar->y = 2137;
            baz = world.createComponent<Baz>( ent );
            baz->z = 123;
        }

        return foo->x; //returning something because catch2 wants it
    };

    BENCHMARK( "Creating a lot of similair entities and use getter" )
    {
        CComponentHandle<Foo> foo;
        CComponentHandle<Bar> bar;
        CComponentHandle<Baz> baz;

        for (entityid_t i = 0; i < ENTITY_COUNT; i++)
        {
            entityid_t ent = world.createEntity();

            // every foo will be different
            foo = world.createComponent<Foo>( ent );
            foo = world.getComponent<Foo>( ent );
            foo->x = i;

            // but every bar and baz will be the same
            bar = world.createComponent<Bar>( ent );
            bar = world.getComponent<Bar>( ent );
            bar->y = 2137;
            baz = world.createComponent<Baz>( ent );
            baz = world.getComponent<Baz>( ent );
            baz->z = 123;
        }

        return foo->x; //returning something because catch2 wants it
    };
}
