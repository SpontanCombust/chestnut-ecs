#include <catch2/catch.hpp>

#include "../include/chestnut/ecs/entity_world.hpp"

using namespace chestnut;

class Foo : public CComponent
{
public:
    int x;
};

class Bar : public CComponent
{
public:
    long y;
};

class Baz : public CComponent
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
        entityid e1, e2, e3;

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
        entityid e1, e2, e3;

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

    SECTION( "Destroying multiple empty entities at once" )
    {
        std::vector< entityid > ids1, ids2;

        for (size_t i = 0; i < 10; i++)
        {
            ids1.push_back( world.createEntity() );
            ids2.push_back( world.createEntity() );
        }
        

        world.destroyEntities( ids2 );

        for( entityid id : ids1 )
        {
            REQUIRE( world.hasEntity( id ) );
        }
        for( entityid id : ids2 )
        {
            REQUIRE_FALSE( world.hasEntity( id ) );
        }


        world.destroyAllEntities();

        for( entityid id : ids1 )
        {
            REQUIRE_FALSE( world.hasEntity( id ) );
        }
        for( entityid id : ids2 )
        {
            REQUIRE_FALSE( world.hasEntity( id ) );
        }
    }

    SECTION( "ID recycling" )
    {
        entityid ent1, ent2, ent3, ent4;

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


        entityid ent = world.createEntity();
        Foo *foo;

        foo = world.createComponent<Foo>( ent );
        REQUIRE( foo->owner == ent );

        foo->x = 1;

        // check if returns already created component on repeated create
        foo = world.createComponent<Foo>( ent );
        REQUIRE( foo->x == 1 );

        REQUIRE( world.hasComponent<Foo>( ent ) );
    }

    SECTION( "Getting components" )
    {
        entityid ent1 = world.createEntity();
        entityid ent2 = world.createEntity();
        entityid ent3 = world.createEntity();

        Foo *foo;
        Bar *bar;
        Baz *baz;


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
        entityid ent1, ent2;

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
        entityid ent = world.createEntity();

        world.createComponent<Foo>( ent );
        world.createComponent<Bar>( ent );
        world.createComponent<Baz>( ent );

        world.destroyEntity( ent );


        REQUIRE_FALSE( world.hasComponent<Foo>( ent ) );
        REQUIRE_FALSE( world.hasComponent<Bar>( ent ) );
        REQUIRE_FALSE( world.hasComponent<Baz>( ent ) );
        REQUIRE_FALSE( world.hasEntity( ent ) );
    }

}