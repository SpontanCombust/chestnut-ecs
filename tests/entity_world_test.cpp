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
        CEntity e1 = world.createEntity();
        CEntity e2 = world.createEntity();
        CEntity e3 = world.createEntity();

        REQUIRE( world.hasEntity(e1) );
        REQUIRE( world.hasEntity(e2) );
        REQUIRE( world.hasEntity(e3) );
    }

    SECTION( "Destroying empty entities" )
    {
        CEntity e1 = world.createEntity();
        CEntity e2 = world.createEntity();
        CEntity e3 = world.createEntity();

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
        CEntity ent1 = world.createEntity();
        CEntity ent2 = world.createEntity();
        
        world.destroyEntity( ent1 );
        world.destroyEntity( ent2 );

        CEntity ent3 = world.createEntity();
        CEntity ent4 = world.createEntity();

        REQUIRE( ent3 != ent2 );
        REQUIRE( ent3.slot == ent2.slot );
        REQUIRE( ent4 != ent1 );
        REQUIRE( ent4.slot == ent1.slot );
    }


    SECTION( "Creating components" )
    {
        CEntity invalidated = world.createEntity();
        world.destroyEntity(invalidated);
        // try create component for nonexistant entity
        REQUIRE_FALSE( world.createComponent<Foo>(invalidated) );


        CEntity ent = world.createEntity();

        REQUIRE(world.createComponent<Foo>( ent ).has_value());
        auto foo = world.createComponent<Foo>( ent ).value();
        REQUIRE( foo );
        REQUIRE( foo.owner == ent );

        foo->x = 1;

        // check if returns already created component on repeated create
        REQUIRE(world.createComponent<Foo>( ent ).has_value());
        foo = world.createComponent<Foo>( ent ).value();
        REQUIRE( foo->x == 1 );

        REQUIRE( world.hasComponent<Foo>( ent ) );


        // create with arg
        auto bar = world.createComponent(ent, Bar {3});
        REQUIRE(bar.has_value());
        REQUIRE(bar.value());
        REQUIRE(bar.value()->y == 3);
    }

    SECTION("Creating or updating components")
    {
        CEntity ent = world.createEntity();
        auto foo = world.createOrUpdateComponent<Foo>(ent, Foo {8});
        REQUIRE(foo.has_value());
        REQUIRE(foo.value());
        REQUIRE(foo.value()->x == 8);

        foo = world.createOrUpdateComponent<Foo>(ent, Foo {11});
        REQUIRE(foo.has_value());
        REQUIRE(foo.value());
        REQUIRE(foo.value()->x == 11);
    }

    SECTION( "Getting invalid components" )
    {
        CEntity invalidated = world.createEntity();
        world.destroyEntity(invalidated);

        auto foo = world.getComponent<Foo>(invalidated);
        REQUIRE_FALSE( foo.has_value() );
    }

    SECTION( "Getting components" )
    {
        CEntity ent1 = world.createEntity();
        CEntity ent2 = world.createEntity();
        CEntity ent3 = world.createEntity();

        auto foo = world.createComponent<Foo>( ent1 ).value();
        foo->x = 1;

        foo = world.createComponent<Foo>( ent2 ).value();
        foo->x = 2;
        auto baz = world.createComponent<Baz>( ent2, Baz {3, 4} ).value();

        // test out inner value assignment operator
        auto bar = world.createComponent<Bar>( ent3, Bar{5} ).value();
        baz = world.createComponent<Baz>( ent3 ).value();
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


        REQUIRE(world.getComponent<Foo>( ent1 ).has_value());
        foo = world.getComponent<Foo>( ent1 ).value();
        REQUIRE( foo );
        REQUIRE( foo->x == 1 );
        REQUIRE_FALSE(world.getComponent<Bar>( ent1 ).has_value());
        REQUIRE_FALSE(world.getComponent<Baz>( ent1 ).has_value());

        REQUIRE(world.getComponent<Foo>( ent2 ).has_value());
        foo = world.getComponent<Foo>( ent2 ).value();
        REQUIRE( foo );
        REQUIRE( foo->x == 2 );
        REQUIRE_FALSE( world.getComponent<Bar>( ent2 ).has_value() );
        REQUIRE(world.getComponent<Baz>( ent2 ).has_value());
        baz = world.getComponent<Baz>( ent2 ).value();
        REQUIRE( baz );
        REQUIRE( baz->z == 3 );
        REQUIRE( baz->w == 4 );

        REQUIRE_FALSE( world.getComponent<Foo>( ent3 ).has_value() );
        REQUIRE( world.getComponent<Bar>( ent3 ).has_value() );
        bar = world.getComponent<Bar>( ent3 ).value();
        REQUIRE( bar );
        REQUIRE( bar->y == 5 );
        REQUIRE(world.getComponent<Baz>( ent3 ).has_value());
        baz = world.getComponent<Baz>( ent3 ).value();
        REQUIRE( baz );
        REQUIRE( baz->z == 6 );
        REQUIRE( baz->w == 7 );
    }

    SECTION("Creating entities with data")
    {
        // one component
        CEntity ent2 = world.createEntityWithComponents(Foo{1});
        REQUIRE(world.getEntitySignature(ent2).has_value());
        CEntitySignature sign2 = world.getEntitySignature(ent2).value();
        REQUIRE(sign2 == CEntitySignature::from<CIdentityComponent, Foo>());
        REQUIRE(world.getComponent<Foo>(ent2).has_value());
        auto foo = world.getComponent<Foo>(ent2).value();
        REQUIRE(foo->x == 1);

        // many components
        CEntity ent3 = world.createEntityWithComponents(std::make_tuple(Foo{1}, Bar{16}, Baz{21, 37}));
        REQUIRE(world.getEntitySignature(ent3).has_value());
        CEntitySignature sign3 = world.getEntitySignature(ent3).value();
        REQUIRE(sign3 == CEntitySignature::from<CIdentityComponent, Foo, Bar, Baz>());
        foo = world.getComponent<Foo>(ent3).value();
        REQUIRE(foo->x == 1);
        auto bar = world.getComponent<Bar>(ent3).value();
        REQUIRE(bar->y == 16);
        auto baz = world.getComponent<Baz>(ent3).value();
        REQUIRE(baz->z == 21);
        REQUIRE(baz->w == 37);
    }

    SECTION( "Destroying components" )
    {
        CEntity ent1 = world.createEntity();
        CEntity ent2 = world.createEntity();


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
        CEntity ent = world.createEntity();
        auto handle = world.createComponent<Foo>(ent).value();
        REQUIRE(handle.get().has_value());

        world.destroyComponent<Foo>(ent);
        REQUIRE_FALSE(handle.get().has_value());
    }

    SECTION( "Destroying entities with components" )
    {
        CEntity ent = world.createEntity();

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
        std::vector<CEntity> vecFooBar;
        std::vector<CEntity> vecBarBaz;
        
        // 10 with Foo and Bar
        for (size_t i = 0; i < 10; i++)
        {
            CEntity ent = world.createEntity();
            world.createComponent<Foo>( ent ).value()->x = (int)ent.slot;
            world.createComponent<Bar>( ent ).value()->y = (long)ent.slot + 1;
            vecFooBar.push_back( ent );
        }
        // 10 with Bar and Baz
        for (size_t i = 0; i < 10; i++)
        {
            CEntity ent = world.createEntity();
            world.createComponent<Bar>( ent ).value()->y = (long)ent.slot;
            world.createComponent<Baz>( ent ).value()->z = (char)ent.slot + 1;
            vecBarBaz.push_back( ent );
        }


        std::vector<CEntity> vecFound = world.findEntities( []( const CEntitySignature& sign ) 
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

    std::vector<CEntity> vecEntsFoo;
    for (size_t i = 0; i < 4; i++)
    {
        auto ent = world.createEntity();
        world.createComponent<Foo>(ent);
        vecEntsFoo.push_back(ent);
    }

    std::vector<CEntity> vecEntsBarBaz;
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
        REQUIRE(it.entity() == vecEntsFoo[1]);
        REQUIRE(it.contains<Foo>());
        REQUIRE(it.signature() == makeEntitySignature<CIdentityComponent, Foo>());
        REQUIRE_NOTHROW(it.get<Foo>());

        it++;
        REQUIRE(it.entity() == vecEntsFoo[2]);
        REQUIRE(it.contains<Foo>());
        REQUIRE(it.signature() == makeEntitySignature<CIdentityComponent, Foo>());
        REQUIRE_NOTHROW(it.get<Foo>());

        it++;
        REQUIRE(it.entity() == vecEntsBarBaz[0]);
        REQUIRE(it.contains<Bar>());
        REQUIRE(it.contains<Baz>());
        REQUIRE(it.signature() == makeEntitySignature<CIdentityComponent, Bar, Baz>());
        REQUIRE_NOTHROW(it.get<Bar>());
        REQUIRE_NOTHROW(it.get<Baz>());

        it++;
        REQUIRE(it.entity() == vecEntsBarBaz[3]);
        REQUIRE(it.contains<Bar>());
        REQUIRE(it.contains<Baz>());
        REQUIRE(it.signature() == makeEntitySignature<CIdentityComponent, Bar, Baz>());
        REQUIRE_NOTHROW(it.get<Bar>());
        REQUIRE_NOTHROW(it.get<Baz>());

        it++;
        REQUIRE(it == end);

        it--;
        REQUIRE(it.entity() == vecEntsBarBaz[3]);

        it--;
        REQUIRE(it.entity() == vecEntsBarBaz[0]);

        it--;
        REQUIRE(it.entity() == vecEntsFoo[2]);

        it--;
        REQUIRE(it.entity() == vecEntsFoo[1]);
        REQUIRE(it == begin);
    }

    SECTION("CEntityConstIterator")
    {
        auto begin = world.entityIterator.cbegin();
        auto end = world.entityIterator.cend();

        auto it = begin;
        REQUIRE(it.entity() == vecEntsFoo[1]);
        REQUIRE(it.contains<Foo>());
        REQUIRE(it.signature() == makeEntitySignature<CIdentityComponent, Foo>());
        REQUIRE_NOTHROW(it.get<Foo>());

        it++;
        REQUIRE(it.entity() == vecEntsFoo[2]);
        REQUIRE(it.contains<Foo>());
        REQUIRE(it.signature() == makeEntitySignature<CIdentityComponent, Foo>());
        REQUIRE_NOTHROW(it.get<Foo>());

        it++;
        REQUIRE(it.entity() == vecEntsBarBaz[0]);
        REQUIRE(it.contains<Bar>());
        REQUIRE(it.contains<Baz>());
        REQUIRE(it.signature() == makeEntitySignature<CIdentityComponent, Bar, Baz>());
        REQUIRE_NOTHROW(it.get<Bar>());
        REQUIRE_NOTHROW(it.get<Baz>());

        it++;
        REQUIRE(it.entity() == vecEntsBarBaz[3]);
        REQUIRE(it.contains<Bar>());
        REQUIRE(it.contains<Baz>());
        REQUIRE(it.signature() == makeEntitySignature<CIdentityComponent, Bar, Baz>());
        REQUIRE_NOTHROW(it.get<Bar>());
        REQUIRE_NOTHROW(it.get<Baz>());

        it++;
        REQUIRE(it == end);

        it--;
        REQUIRE(it.entity() == vecEntsBarBaz[3]);

        it--;
        REQUIRE(it.entity() == vecEntsBarBaz[0]);

        it--;
        REQUIRE(it.entity() == vecEntsFoo[2]);

        it--;
        REQUIRE(it.entity() == vecEntsFoo[1]);
        REQUIRE(it == begin);
    }
}




TEST_CASE( "Entity world test - benchmarks", "[benchmark]" )
{
    const int ENTITY_COUNT = 10000;
    CEntityWorld world;

    BENCHMARK( "Creating a lot of similair entities" )
    {
        for (int i = 0; i < ENTITY_COUNT; i++)
        {
            CEntity ent = world.createEntity();

            // every foo will be different
            auto foo = world.createComponent<Foo>( ent ).value();
            foo->x = i;

            // but every bar and baz will be the same
            auto bar = world.createComponent<Bar>( ent ).value();
            bar->y = 2137;
            auto baz = world.createComponent<Baz>( ent ).value();
            baz->z = 123;
        }

        return 2137; //returning something because catch2 wants it
    };

    BENCHMARK( "Creating a lot of similair entities and use getter" )
    {
        for (int i = 0; i < ENTITY_COUNT; i++)
        {
            CEntity ent = world.createEntity();

            // every foo will be different
            world.createComponent<Foo>( ent );
            auto foo = world.getComponent<Foo>( ent ).value();
            foo->x = i;

            // but every bar and baz will be the same
            world.createComponent<Bar>( ent );
            auto bar = world.getComponent<Bar>( ent ).value();
            bar->y = 2137;
            world.createComponent<Baz>( ent );
            auto baz = world.getComponent<Baz>( ent ).value();
            baz->z = 123;
        }

        return 2137; //returning something because catch2 wants it
    };
}
