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
    std::vector<CEntity> vEnts;

    // 10 entities with Foo
    for (int i = 0; i < 10; i++)
    {
        CEntity ent = world.createEntity();
        world.insertComponent(ent, Foo{i});
        vEnts.push_back(ent);
    }
    // 10 with Foo and Bar
    for (int i = 10; i < 20; i++)
    {
        CEntity ent = world.createEntity();
        world.insertComponent(ent, Foo{i});
        world.insertComponent(ent, Bar{i + 1});
        vEnts.push_back(ent);
    }
    // 10 with Bar and Baz
    for (int i = 20; i < 30; i++)
    {
        CEntity ent = world.createEntity();
        world.insertComponent(ent, Bar{i});
        world.insertComponent(ent, Baz{char(i + 1), short(i + 2)});
        vEnts.push_back(ent);
    }
    // 10 with Foo and Baz
    for (int i = 30; i < 40; i++)
    {
        CEntity ent = world.createEntity();
        world.insertComponent(ent, Foo{i});
        world.insertComponent(ent, Baz{char(i + 1), short(i + 2)});
        vEnts.push_back(ent);
    }
    // 10 with Foo, Bar and Baz
    for (int i = 40; i < 50; i++)
    {
        CEntity ent = world.createEntity();
        world.insertComponent(ent, Foo{i});
        world.insertComponent(ent, Bar{i + 1});
        world.insertComponent(ent, Baz{char(i + 2), short(i + 3)});
        vEnts.push_back(ent);
    }
    

    SECTION( "Query entities with Foo" )
    {
        auto q = world.createQuery( CEntitySignature::from<Foo>(), CEntitySignature() );
        q.update();

        REQUIRE( q.getEntityCount() == 40 );

        for(auto it = q.begin<Foo>(); it != q.end<Foo>(); it++)
        {
            std::apply([it](Foo& foo) {
                REQUIRE(( foo.x < 20 || foo.x >= 30 ));
                foo.x *= 2;
            }, *it);
        }

        REQUIRE(world.getComponent<Foo>(vEnts[0]).value()->x == 0);
        REQUIRE(world.getComponent<Foo>(vEnts[10]).value()->x == 20);
        REQUIRE(world.getComponent<Foo>(vEnts[30]).value()->x == 60);

        world.destroyQuery(q);
    }

    SECTION( "Query entities with Foo, Bar and Baz" )
    {
        auto q = world.createQuery( CEntitySignature::from<Foo, Bar, Baz>(), CEntitySignature() );
        q.update();

        REQUIRE( q.getEntityCount() == 10 );

        for(auto it = q.begin<Foo, Bar, Baz>(); it != q.end<Foo, Bar, Baz>(); it++)
        {
            std::apply([it](Foo& foo, Bar& bar, Baz& baz) {
                REQUIRE(( foo.x >= 40 && foo.x < 50 ));
                foo.x *= 2;
                bar.y *= 2;
                baz.w *= 2;
            }, *it);
        }

        REQUIRE(world.getComponent<Foo>(vEnts[40]).value()->x == 80);
        REQUIRE(world.getComponent<Bar>(vEnts[45]).value()->y == 92);
        REQUIRE(world.getComponent<Baz>(vEnts[49]).value()->w == 104);

        world.destroyQuery(q);
    }

    SECTION( "Query entities with Baz but no Foo" )
    {
        auto q = world.createQuery( CEntitySignature::from<Baz>(), CEntitySignature::from<Foo>() );
        q.update();

        REQUIRE( q.getEntityCount() == 10 );

        for(auto it = q.begin<Baz>(); it != q.end<Baz>(); it++)
        {
            std::apply([it](Baz& baz) {
                REQUIRE(( baz.z >= 21 && baz.z < 31 ));
            }, *it);
        }

        world.destroyQuery(q);
    }

    SECTION("Invalid query iterator")
    {
        auto q = world.createQuery( CEntitySignature::from<Foo>(), CEntitySignature::from<Bar>() );
        q.update();

        REQUIRE_NOTHROW(q.begin<Foo>());
        REQUIRE_NOTHROW(q.end<Foo>());

        REQUIRE_THROWS(q.begin<Bar>());
        REQUIRE_THROWS(q.end<Bar>());

        REQUIRE_THROWS(q.begin<Baz>());
        REQUIRE_THROWS(q.end<Baz>());

        REQUIRE_THROWS(q.begin<Foo, Bar>());
        REQUIRE_THROWS(q.end<Foo, Bar>());
    }

    SECTION("Query sequence")
    {
        auto q = world.createQuery( CEntitySignature::from<Baz>(), CEntitySignature::from<Foo>() );

        q.update();


        for (char i = 0; i < 5; i++)
        {
            CEntity ent = world.createEntity();
            world.insertComponent(ent, Bar{i});
            world.insertComponent(ent, Baz{char(i + 1), short(i + 2)});
            vEnts.push_back(ent);
        }

        for(int i = 20; i < 30; i++)
        {
            auto ent = vEnts[i];
            world.destroyComponent<Baz>(ent);
        }

        q.update();
    }

    SECTION( "Query non-existing entities" )
    {
        auto q = world.createQuery( CEntitySignature::from<Bar>(), CEntitySignature::from<Foo, Baz>() );

        q.update();

        REQUIRE( q.getEntityCount() == 0 );

        world.destroyQuery(q);
    }

    SECTION( "Use forEach" )
    {
        auto q = world.createQuery( CEntitySignature::from<Bar>() );
        q.update();

        REQUIRE( q.getEntityCount() == 30 );  

        q.forEach<Bar>(std::function(
            [](Bar& bar) {
                REQUIRE(((bar.y >= 10 && bar.y < 30) || bar.y > 40));
            }
        ));

        world.destroyQuery(q); 
    }

    SECTION( "Sort the query" ) 
    {
        auto q = world.createQuery( CEntitySignature::from<Foo>() );
        q.update();

        REQUIRE( q.getEntityCount() == 40 );

        using Iterator = CEntityQuery::Iterator<Foo>;
        q.sort<Foo>(std::function(
            [](Iterator it1, Iterator it2) -> bool {
                // sort by entity ID descending
                return it1.entity().uuid > it2.entity().uuid;
            }
        ));

        for(auto it = q.begin<Foo>(); it != q.end<Foo>() - 1; it++)
        {
            auto next = it + 1;
            REQUIRE(it.entity().uuid > next.entity().uuid);
        }


        q.sort<Foo>(std::function(
            [](Iterator it1, Iterator it2) -> bool {
                // now ascending
                return it1.entity().uuid < it2.entity().uuid;
            }
        ));

        for(auto it = q.begin<Foo>(); it != q.end<Foo>() - 1; it++)
        {
            auto next = it + 1;
            REQUIRE(it.entity().uuid < next.entity().uuid);
        }

        world.destroyQuery(q);
    }

    SECTION("Acquired and discarded entities")
    {
        //TODO
    }
}
