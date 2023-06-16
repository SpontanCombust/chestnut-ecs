#include <catch2/catch.hpp>

#include "../include/chestnut/ecs/commands.hpp"

using namespace chestnut::ecs;
using namespace chestnut::ecs::internal;

struct Foo
{
    int a;
};

struct Bar
{
    int a, b;
};

TEST_CASE("Commands test")
{
    CEntityWorld world;
    CCommands cmd;

    SECTION("Create entities")
    {
        cmd.createEntity()
           .createEntity(Foo{0})
           .createEntity(Foo{1}, Bar{2, 3})
           .createUniqueEntity()
           .createUniqueEntity(Foo{1}, Bar{2, 3})
           .getCommandQueue().execute(world);

        REQUIRE(world.findEntities([](auto sign) { return true; }).size() == 5);
        REQUIRE(world.findEntities([](auto sign) { return sign.has<Foo>(); }).size() == 3);
        REQUIRE(world.findEntities([](auto sign) { return sign.has<Bar>(); }).size() == 2);
        REQUIRE(world.findEntities([](auto sign) { return sign.has<Foo, Bar>(); }).size() == 2);
    }

    SECTION("Destroy entities")
    {
        entityid_t ent1 = world.createEntity();
        entityid_t ent2 = world.createEntity();
        entityid_t ent3 = world.createEntity();
        
        cmd.destroyEntity(ent1)
           .createUniqueEntity()
           .destroyEntity(ent3)
           .createEntity()
           .getCommandQueue().execute(world);

        // ID recycling was not used
        REQUIRE_FALSE(world.hasEntity(ent1));
        REQUIRE(world.hasEntity(ent2));
        // ID recycling was used
        REQUIRE(world.hasEntity(ent3));
        REQUIRE(world.findEntities([](auto sign) { return true; }).size() == 3);
    }

    SECTION("Create or update components")
    {
        entityid_t ent1 = world.createEntityWithComponents(std::make_tuple(Foo{1}, Bar{2, 3}));
        entityid_t ent2 = world.createEntity();
        entityid_t ent3 = world.createEntityWithComponents(Foo{4});

        cmd.createOrUpdateComponent(ent1, Foo{6})
           .createOrUpdateComponent(ent2, Bar{7, 8})
           .getCommandQueue().execute(world);


        CComponentHandle<Foo> foo;
        CComponentHandle<Bar> bar;

        foo = world.getComponent<Foo>(ent1);
        REQUIRE(foo);
        REQUIRE(foo->a == 6);
        bar = world.getComponent<Bar>(ent1);
        REQUIRE(bar);
        REQUIRE((bar->a == 2 && bar->b == 3));

        foo = world.getComponent<Foo>(ent2);
        REQUIRE_FALSE(foo);
        bar = world.getComponent<Bar>(ent2);
        REQUIRE(bar);
        REQUIRE((bar->a == 7 && bar->b == 8));

        foo = world.getComponent<Foo>(ent3);
        REQUIRE(foo);
        REQUIRE(foo->a == 4);
        bar = world.getComponent<Bar>(ent3);
        REQUIRE_FALSE(bar);
    }

    SECTION("Update components")
    {
        entityid_t ent1 = world.createEntityWithComponents(std::make_tuple(Foo{1}, Bar{2, 3}));
        entityid_t ent2 = world.createEntityWithComponents(std::make_tuple(Bar{4, 5}));

        cmd.updateComponent<Foo>(ent1, [](Foo& foo) { foo.a = 11; })
           .updateComponent<Foo>(ent2, [](Foo& foo) { foo.a = 22; })
           .updateComponent<Bar>(ent2, [](Bar& bar) { bar.a = 33; })
           .getCommandQueue().execute(world);

        
        CComponentHandle<Foo> foo;
        CComponentHandle<Bar> bar;

        foo = world.getComponent<Foo>(ent1);
        REQUIRE(foo);
        REQUIRE(foo->a == 11);

        foo = world.getComponent<Foo>(ent2);
        REQUIRE_FALSE(foo);
        bar = world.getComponent<Bar>(ent2);
        REQUIRE(bar);
        REQUIRE((bar->a == 33 && bar->b == 5));
    }

    SECTION("Destroy components")
    {
        entityid_t ent1 = world.createEntity();
        entityid_t ent2 = world.createEntityWithComponents(Foo{1});
        entityid_t ent3 = world.createEntityWithComponents(std::make_tuple(Foo{2}, Bar{3, 4}));

        cmd.destroyComponent<Foo>(ent1)
           .destroyComponent<Bar>(ent1)
           .destroyComponent<Foo>(ent2)
           .createOrUpdateComponent<Foo>(ent2, Foo{5})
           .destroyComponent<Foo>(ent3)
           .destroyComponent<Bar>(ent3)
           .getCommandQueue().execute(world);


        CComponentHandle<Foo> foo;
        CComponentHandle<Bar> bar;

        foo = world.getComponent<Foo>(ent1);
        REQUIRE_FALSE(foo);
        bar = world.getComponent<Bar>(ent1);
        REQUIRE_FALSE(bar);
        
        foo = world.getComponent<Foo>(ent2);
        REQUIRE(foo);
        REQUIRE(foo->a == 5);
        bar = world.getComponent<Bar>(ent2);
        REQUIRE_FALSE(bar);

        foo = world.getComponent<Foo>(ent3);
        REQUIRE_FALSE(foo);
        bar = world.getComponent<Bar>(ent3);
        REQUIRE_FALSE(bar);
    }

    SECTION("Clear queue")
    {
        cmd.createEntity()
           .createEntity(Foo{1})
           .createEntity()
           .createEntity(Bar{2, 3}, Foo{4});

        cmd.clear();
        cmd.getCommandQueue().execute(world);

        REQUIRE(world.findEntities([](auto sign) { return true; }).size() == 0);
    }
}