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
        entityslot_t e1, e2, e3;

        e1 = world.createEntity();
        e2 = world.createEntity();
        e3 = world.createEntity();

        REQUIRE_FALSE( world.hasEntity( ENTITY_SLOT_INVALID ) );
        REQUIRE( world.hasEntity(e1) );
        REQUIRE( world.hasEntity(e2) );
        REQUIRE( world.hasEntity(e3) );
    }

    SECTION( "Destroying empty entities" )
    {
        entityslot_t e1, e2, e3;

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
        entityslot_t ent1, ent2, ent3, ent4;

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


        entityslot_t ent = world.createEntity();
        CComponentHandle<Foo> foo;

        foo = world.createComponent<Foo>(ENTITY_SLOT_INVALID);
        REQUIRE_FALSE(foo);

        foo = world.createComponent<Foo>( ent );
        REQUIRE( foo );
        REQUIRE( foo.owner == ent );

        foo->x = 1;

        // check if returns already created component on repeated create
        foo = world.createComponent<Foo>( ent );
        REQUIRE( foo->x == 1 );

        REQUIRE( world.hasComponent<Foo>( ent ) );


        // create with arg
        auto bar = world.createComponent(ent, Bar {3});
        REQUIRE(bar);
        REQUIRE(bar->y == 3);
    }

    SECTION("Creating or updating components")
    {
        CComponentHandle<Foo> foo;

        foo = world.createComponent<Foo>(ENTITY_SLOT_INVALID);
        REQUIRE_FALSE(foo);

        entityslot_t ent = world.createEntity();
        foo = world.createOrUpdateComponent<Foo>(ent, Foo {8});
        REQUIRE(foo);
        REQUIRE(foo->x == 8);

        foo = world.createOrUpdateComponent<Foo>(ent, Foo {11});
        REQUIRE(foo);
        REQUIRE(foo->x == 11);
    }

    SECTION( "Getting invalid components" )
    {
        auto foo = world.getComponent<Foo>(9127);

        REQUIRE_FALSE( foo );
        REQUIRE_THROWS( foo->x = 4 );
    }

    SECTION( "Getting components" )
    {
        entityslot_t ent1 = world.createEntity();
        entityslot_t ent2 = world.createEntity();
        entityslot_t ent3 = world.createEntity();

        CComponentHandle<Foo> foo;
        CComponentHandle<Bar> bar;
        CComponentHandle<Baz> baz;


        foo = world.createComponent<Foo>( ent1 );
        foo->x = 1;

        foo = world.createComponent<Foo>( ent2 );
        foo->x = 2;
        baz = world.createComponent<Baz>( ent2, Baz {3, 4} );

        // test out inner value assignment operator
        bar = world.createComponent<Bar>( ent3, Bar{5} );
        baz = world.createComponent<Baz>( ent3 );
        baz = Baz{6, 7};


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
        REQUIRE( foo );
        REQUIRE( foo->x == 1 );
        bar = world.getComponent<Bar>( ent1 );
        REQUIRE_FALSE( bar );
        baz = world.getComponent<Baz>( ent1 );

        foo = world.getComponent<Foo>( ent2 );
        REQUIRE( foo );
        REQUIRE( foo->x == 2 );
        bar = world.getComponent<Bar>( ent2 );
        REQUIRE_FALSE( bar );
        baz = world.getComponent<Baz>( ent2 );
        REQUIRE( baz );
        REQUIRE( baz->z == 3 );
        REQUIRE( baz->w == 4 );

        foo = world.getComponent<Foo>( ent3 );
        REQUIRE_FALSE( foo );
        bar = world.getComponent<Bar>( ent3 );
        REQUIRE( bar );
        REQUIRE( bar->y == 5 );
        baz = world.getComponent<Baz>( ent3 );
        REQUIRE( baz );
        REQUIRE( baz->z == 6 );
        REQUIRE( baz->w == 7 );
    }

    SECTION("Creating entities with data")
    {
        CComponentHandle<Foo> foo;
        CComponentHandle<Bar> bar;
        CComponentHandle<Baz> baz;

        // one component
        entityslot_t ent2 = world.createEntityWithComponents(Foo{1});
        CEntitySignature sign2 = world.getEntitySignature(ent2);
        REQUIRE(sign2 == CEntitySignature::from<Foo>());
        foo = world.getComponent<Foo>(ent2);
        REQUIRE(foo->x == 1);

        // many components
        entityslot_t ent3 = world.createEntityWithComponents(std::make_tuple(Foo{1}, Bar{16}, Baz{21, 37}));
        CEntitySignature sign3 = world.getEntitySignature(ent3);
        REQUIRE(sign3 == CEntitySignature::from<Foo, Bar, Baz>());
        foo = world.getComponent<Foo>(ent3);
        REQUIRE(foo->x == 1);
        bar = world.getComponent<Bar>(ent3);
        REQUIRE(bar->y == 16);
        baz = world.getComponent<Baz>(ent3);
        REQUIRE(baz->z == 21);
        REQUIRE(baz->w == 37);
    }

    SECTION( "Destroying components" )
    {
        entityslot_t ent1, ent2;

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

    SECTION("Accessing invalid handle")
    {
        // null handle
        CComponentHandle<Foo> handle;
        REQUIRE_THROWS(handle.get());

        entityslot_t ent = world.createEntity();
        handle = world.createComponent<Foo>(ent);
        REQUIRE_NOTHROW(handle.get());

        world.destroyComponent<Foo>(ent);
        REQUIRE_THROWS(handle.get());
    }

    SECTION( "Destroying entities with components" )
    {
        entityslot_t ent = world.createEntity();

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
        std::vector< entityslot_t > vecFooBar;
        std::vector< entityslot_t > vecBarBaz;
        
        // 10 with Foo and Bar
        for (size_t i = 0; i < 10; i++)
        {
            entityslot_t ent = world.createEntity();
            world.createComponent<Foo>( ent )->x = ent;
            world.createComponent<Bar>( ent )->y = ent + 1;
            vecFooBar.push_back( ent );
        }
        // 10 with Bar and Baz
        for (size_t i = 0; i < 10; i++)
        {
            entityslot_t ent = world.createEntity();
            world.createComponent<Bar>( ent )->y = ent;
            world.createComponent<Baz>( ent )->z = (char)ent + 1;
            vecBarBaz.push_back( ent );
        }


        std::vector< entityslot_t > vecFound = world.findEntities( []( const CEntitySignature& sign ) 
        {
            return sign.has<Bar>();
        });

        REQUIRE( vecFound.size() == 20 );



        auto allEntsFinder = [](auto _) {
            return true;
        };

        auto entCount = world.findEntities(allEntsFinder).size();
        REQUIRE(entCount == 20);


        for(auto id : vecFooBar)
            world.destroyEntity(id);

        entCount = world.findEntities(allEntsFinder).size();
        REQUIRE(entCount == 10);


        for(auto id : vecBarBaz)
            world.destroyEntity(id);

        entCount = world.findEntities(allEntsFinder).size();
        REQUIRE(entCount == 0);
    }
}




TEST_CASE("Entity iterator test")
{
    CEntityWorld world;

    std::vector<entityslot_t> vecEntsFoo;
    for (size_t i = 0; i < 4; i++)
    {
        auto ent = world.createEntity();
        world.createComponent<Foo>(ent);
        vecEntsFoo.push_back(ent);
    }

    std::vector<entityslot_t> vecEntsBarBaz;
    for (size_t i = 0; i < 4; i++)
    {
        auto ent = world.createEntity();
        world.createComponent<Bar>(ent);
        world.createComponent<Baz>(ent);
        vecEntsBarBaz.push_back(ent);
    }

    world.destroyEntity(vecEntsFoo[0]);
    world.destroyEntity(vecEntsFoo[3]);
    world.destroyEntity(vecEntsBarBaz[1]);
    world.destroyEntity(vecEntsBarBaz[2]);

    SECTION("CEntityIterator")
    {
        auto begin = world.entityIterator.begin();
        auto end = world.entityIterator.end();

        auto it = begin;
        REQUIRE(it.id() == vecEntsFoo[1]);
        REQUIRE(it.contains<Foo>());
        REQUIRE(it.signature() == makeEntitySignature<Foo>());
        REQUIRE_NOTHROW(it.get<Foo>());

        it++;
        REQUIRE(it.id() == vecEntsFoo[2]);
        REQUIRE(it.contains<Foo>());
        REQUIRE(it.signature() == makeEntitySignature<Foo>());
        REQUIRE_NOTHROW(it.get<Foo>());

        it++;
        REQUIRE(it.id() == vecEntsBarBaz[0]);
        REQUIRE(it.contains<Bar>());
        REQUIRE(it.contains<Baz>());
        REQUIRE(it.signature() == makeEntitySignature<Bar, Baz>());
        REQUIRE_NOTHROW(it.get<Bar>());
        REQUIRE_NOTHROW(it.get<Baz>());

        it++;
        REQUIRE(it.id() == vecEntsBarBaz[3]);
        REQUIRE(it.contains<Bar>());
        REQUIRE(it.contains<Baz>());
        REQUIRE(it.signature() == makeEntitySignature<Bar, Baz>());
        REQUIRE_NOTHROW(it.get<Bar>());
        REQUIRE_NOTHROW(it.get<Baz>());

        it++;
        REQUIRE(it == end);

        it--;
        REQUIRE(it.id() == vecEntsBarBaz[3]);

        it--;
        REQUIRE(it.id() == vecEntsBarBaz[0]);

        it--;
        REQUIRE(it.id() == vecEntsFoo[2]);

        it--;
        REQUIRE(it.id() == vecEntsFoo[1]);
        REQUIRE(it == begin);
    }

    SECTION("CEntityConstIterator")
    {
        auto begin = world.entityIterator.cbegin();
        auto end = world.entityIterator.cend();

        auto it = begin;
        REQUIRE(it.id() == vecEntsFoo[1]);
        REQUIRE(it.contains<Foo>());
        REQUIRE(it.signature() == makeEntitySignature<Foo>());
        REQUIRE_NOTHROW(it.get<Foo>());

        it++;
        REQUIRE(it.id() == vecEntsFoo[2]);
        REQUIRE(it.contains<Foo>());
        REQUIRE(it.signature() == makeEntitySignature<Foo>());
        REQUIRE_NOTHROW(it.get<Foo>());

        it++;
        REQUIRE(it.id() == vecEntsBarBaz[0]);
        REQUIRE(it.contains<Bar>());
        REQUIRE(it.contains<Baz>());
        REQUIRE(it.signature() == makeEntitySignature<Bar, Baz>());
        REQUIRE_NOTHROW(it.get<Bar>());
        REQUIRE_NOTHROW(it.get<Baz>());

        it++;
        REQUIRE(it.id() == vecEntsBarBaz[3]);
        REQUIRE(it.contains<Bar>());
        REQUIRE(it.contains<Baz>());
        REQUIRE(it.signature() == makeEntitySignature<Bar, Baz>());
        REQUIRE_NOTHROW(it.get<Bar>());
        REQUIRE_NOTHROW(it.get<Baz>());

        it++;
        REQUIRE(it == end);

        it--;
        REQUIRE(it.id() == vecEntsBarBaz[3]);

        it--;
        REQUIRE(it.id() == vecEntsBarBaz[0]);

        it--;
        REQUIRE(it.id() == vecEntsFoo[2]);

        it--;
        REQUIRE(it.id() == vecEntsFoo[1]);
        REQUIRE(it == begin);
    }
}




TEST_CASE( "Entity world test - benchmarks", "[benchmark]" )
{
    const entityslot_t ENTITY_COUNT = 10000;
    CEntityWorld world;

    BENCHMARK( "Creating a lot of similair entities" )
    {
        CComponentHandle<Foo> foo;
        CComponentHandle<Bar> bar;
        CComponentHandle<Baz> baz;

        for (entityslot_t i = 0; i < ENTITY_COUNT; i++)
        {
            entityslot_t ent = world.createEntity();

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

        for (entityslot_t i = 0; i < ENTITY_COUNT; i++)
        {
            entityslot_t ent = world.createEntity();

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
