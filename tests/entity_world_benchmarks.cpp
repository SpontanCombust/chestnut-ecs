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


TEST_CASE( "Entity world benchmarks - entity creation", "[benchmark]" )
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
