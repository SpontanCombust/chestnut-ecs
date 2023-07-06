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
           .createEntity()
           .createEntity(Foo{1}, Bar{2, 3})
           .getCommandQueue().execute(world);

        REQUIRE(world.findEntities([](auto sign) { return true; }).size() == 5);
        REQUIRE(world.findEntities([](auto sign) { return sign.has<Foo>(); }).size() == 3);
        REQUIRE(world.findEntities([](auto sign) { return sign.has<Bar>(); }).size() == 2);
        REQUIRE(world.findEntities([](auto sign) { return sign.has<Foo, Bar>(); }).size() == 2);
    }

    SECTION("Destroy entities")
    {
        CEntity ent1 = world.createEntity();
        CEntity ent2 = world.createEntity();
        CEntity ent3 = world.createEntity();
        
        cmd.destroyEntity(ent1)
           .createEntity()
           .destroyEntity(ent3)
           .createEntity()
           .getCommandQueue().execute(world);

        // ID recycling was not used
        REQUIRE_FALSE(world.isEntityAlive(ent1));
        REQUIRE(world.isEntityAlive(ent2));
        REQUIRE_FALSE(world.isEntityAlive(ent3));
        REQUIRE(world.findEntities([](auto sign) { return true; }).size() == 3);
    }

    SECTION("Insert components")
    {
        CEntity ent1 = world.createEntity(Foo{1}, Bar{2, 3});
        CEntity ent2 = world.createEntity();
        CEntity ent3 = world.createEntity(Foo{4});

        cmd.insertComponent(ent1, Foo{6})
           .insertComponent(ent2, Bar{7, 8})
           .getCommandQueue().execute(world);

        REQUIRE(world.getComponent<Foo>(ent1).has_value());
        auto foo = world.getComponent<Foo>(ent1).value();
        REQUIRE(foo);
        REQUIRE(foo->a == 6);

        REQUIRE(world.getComponent<Bar>(ent1).has_value());
        auto bar = world.getComponent<Bar>(ent1).value();
        REQUIRE(bar);
        REQUIRE((bar->a == 2 && bar->b == 3));

        REQUIRE_FALSE(world.getComponent<Foo>(ent2).has_value());

        REQUIRE(world.getComponent<Bar>(ent2).has_value());
        bar = world.getComponent<Bar>(ent2).value();
        REQUIRE(bar);
        REQUIRE((bar->a == 7 && bar->b == 8));

        REQUIRE(world.getComponent<Foo>(ent3).has_value());
        foo = world.getComponent<Foo>(ent3).value();
        REQUIRE(foo);
        REQUIRE(foo->a == 4);

        REQUIRE_FALSE(world.getComponent<Bar>(ent3).has_value());
    }

    SECTION("Update components")
    {
        CEntity ent1 = world.createEntity(Foo{1}, Bar{2, 3});
        CEntity ent2 = world.createEntity(Bar{4, 5});

        cmd.updateComponent<Foo>(ent1, [](Foo& foo) { foo.a = 11; })
           .updateComponent<Foo>(ent2, [](Foo& foo) { foo.a = 22; })
           .updateComponent<Bar>(ent2, [](Bar& bar) { bar.a = 33; })
           .getCommandQueue().execute(world);

        REQUIRE(world.getComponent<Foo>(ent1).has_value());
        auto foo = world.getComponent<Foo>(ent1).value();
        REQUIRE(foo);
        REQUIRE(foo->a == 11);

        REQUIRE_FALSE(world.getComponent<Foo>(ent2).has_value());

        REQUIRE(world.getComponent<Bar>(ent2).has_value());
        CComponentHandle<Bar> bar = world.getComponent<Bar>(ent2).value();
        REQUIRE(bar);
        REQUIRE((bar->a == 33 && bar->b == 5));
    }

    SECTION("Destroy components")
    {
        CEntity ent1 = world.createEntity();
        CEntity ent2 = world.createEntity(Foo{1});
        CEntity ent3 = world.createEntity(std::make_tuple(Foo{2}, Bar{3, 4}));

        cmd.destroyComponent<Foo>(ent1)
           .destroyComponent<Bar>(ent1)
           .destroyComponent<Foo>(ent2)
           .insertComponent<Foo>(ent2, Foo{5})
           .destroyComponent<Foo>(ent3)
           .destroyComponent<Bar>(ent3)
           .getCommandQueue().execute(world);

        REQUIRE_FALSE(world.getComponent<Foo>(ent1).has_value());

        REQUIRE_FALSE(world.getComponent<Bar>(ent1).has_value());
        // auto bar = world.getComponent<Bar>(ent1).value();
        // REQUIRE_FALSE(bar);
        
        REQUIRE(world.getComponent<Foo>(ent2).has_value());
        auto foo = world.getComponent<Foo>(ent2).value();
        REQUIRE(foo);
        REQUIRE(foo->a == 5);

        REQUIRE_FALSE(world.getComponent<Bar>(ent2).has_value());

        REQUIRE_FALSE(world.getComponent<Foo>(ent3).has_value());
        REQUIRE_FALSE(world.getComponent<Bar>(ent3).has_value());
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