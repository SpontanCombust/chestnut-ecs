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

    SECTION( "Destroying multiple empty entities at once" )
    {
        std::vector< entityid_t > ids1, ids2;

        ids1 = world.createEntities(10);
        ids2 = world.createEntities(10);
        

        world.destroyEntities( ids2 );

        for( entityid_t id : ids1 )
        {
            REQUIRE( world.hasEntity( id ) );
        }
        for( entityid_t id : ids2 )
        {
            REQUIRE_FALSE( world.hasEntity( id ) );
        }
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

}





TEST_CASE( "Entity world test - check valid component type" )
{
    // uncomment sections to try compiling

    /*
    CEntityWorld world;


    // test for integral type

    //world.setupComponentType<int>();    


    // test for default constructible type
    struct Foo
    {
        Foo( int i )
        {

        }
    };

    //world.setupComponentType<Foo>();


    // test for copy assignable type
    struct Bar
    {
        Bar& operator=( const Bar& ) = delete;
    };

    //world.setupComponentType<Bar>();
    */
}







TEST_CASE( "Entity world test - querying" )
{
    CEntityWorld world;

    // 10 entities with Foo
    for (size_t i = 0; i < 10; i++)
    {
        entityid_t ent = world.createEntity();
        world.createComponent<Foo>( ent )->x = ent;
    }
    // 10 with Foo and Bar
    for (size_t i = 0; i < 10; i++)
    {
        entityid_t ent = world.createEntity();
        world.createComponent<Foo>( ent )->x = ent;
        world.createComponent<Bar>( ent )->y = ent + 1;
    }
    // 10 with Bar and Baz
    for (size_t i = 0; i < 10; i++)
    {
        entityid_t ent = world.createEntity();
        world.createComponent<Bar>( ent )->y = ent;
        world.createComponent<Baz>( ent )->z = ent + 1;
    }
    // 10 with Foo and Baz
    for (size_t i = 0; i < 10; i++)
    {
        entityid_t ent = world.createEntity();
        world.createComponent<Foo>( ent )->x = ent;
        world.createComponent<Baz>( ent )->z = ent + 1;
    }
    // 10 with Foo, Bar and Baz
    for (size_t i = 0; i < 10; i++)
    {
        entityid_t ent = world.createEntity();
        world.createComponent<Foo>( ent )->x = ent;
        world.createComponent<Bar>( ent )->y = ent + 1;
        world.createComponent<Baz>( ent )->z = ent + 2;
    }
    

    SECTION( "Create-get-destroy" )
    {
        // check for null
        REQUIRE_FALSE( world.queryEntities(1) );

        queryid_t q = world.createQuery( CEntitySignature(), CEntitySignature() );

        REQUIRE( world.queryEntities(q) );

        world.destroyQuery(q);

        REQUIRE_FALSE( world.queryEntities(q) );        
    }

    SECTION( "Query entities with Foo" )
    {
        queryid_t q = world.createQuery( makeEntitySignature<Foo>(), makeEntitySignature<>() );

        const CEntityQuery* query = world.queryEntities(q);

        REQUIRE( query );
        REQUIRE( query->getEntityCount() == 40 );

        REQUIRE_NOTHROW(
            query->forEachEntityWith<Foo>(
            []( entityid_t id, Foo& foo )
            {
                foo.x *= foo.x;
                REQUIRE( foo.x == id * id );
            })
        );

        world.destroyQuery(q);
    }

    SECTION( "Doing incorrect forEach on query" )
    {
        queryid_t q = world.createQuery( makeEntitySignature<Baz>(), makeEntitySignature<Foo>() );

        const CEntityQuery* query = world.queryEntities(q);

        REQUIRE( query->getEntityCount() == 10 );

        REQUIRE_THROWS( 
            query->forEachEntityWith<Foo, Baz>(
            []( Foo& foo, Baz& baz )
            {
                foo.x = baz.z * baz.z;
            })
        );

        world.destroyQuery(q);
    }

    SECTION( "Query entities with Foo, Bar and Baz" )
    {
        queryid_t q = world.createQuery( makeEntitySignature<Foo, Bar, Baz>(), makeEntitySignature<>() );

        const CEntityQuery* query = world.queryEntities(q);

        REQUIRE( query->getEntityCount() == 10 );

        REQUIRE_NOTHROW( 
            query->forEachEntityWith<Foo, Bar, Baz>(
            []( Foo& foo, Bar& bar, Baz& baz )
            {
                baz.w *= foo.x + bar.y;
            })
        );

        world.destroyQuery(q);
    }

    SECTION( "Query entities with Baz but no Foo" )
    {
        queryid_t q = world.createQuery( makeEntitySignature<Baz>(), makeEntitySignature<Foo>() );

        const CEntityQuery* query = world.queryEntities(q);

        REQUIRE( query->getEntityCount() == 10 );

        REQUIRE_NOTHROW( 
            query->forEachEntityWith<Baz>(
            []( Baz& baz )
            {
                baz.w *= baz.z;
            })
        );

        world.destroyQuery(q);
    }

    SECTION( "Query non-existing entities" )
    {
        queryid_t q = world.createQuery( makeEntitySignature<Bar>(), makeEntitySignature<Foo, Baz>() );

        const CEntityQuery* query = world.queryEntities(q);

        REQUIRE( query->getEntityCount() == 0 );

        world.destroyQuery(q);
    }
}





TEST_CASE( "Entity world test - find entities" )
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







TEST_CASE( "Entity world test - entity templates" )
{
    CEntityWorld world;

    SECTION( "Simple create-has-destroy check" )
    {
        entityid_t ent1 = world.createTemplateEntity();
        entityid_t ent2 = world.createTemplateEntity();
        entityid_t ent3 = world.createTemplateEntity();
        entityid_t ent4 = world.createTemplateEntity();
        entityid_t ent5 = world.createTemplateEntity();

        REQUIRE( world.hasTemplateEntity( ent1 ) );
        REQUIRE( world.hasTemplateEntity( ent2 ) );
        REQUIRE( world.hasTemplateEntity( ent3 ) );
        REQUIRE( world.hasTemplateEntity( ent4 ) );
        REQUIRE( world.hasTemplateEntity( ent5 ) );

        world.destroyTemplateEntity( ent1 );
        world.destroyTemplateEntity( ent2 );
        world.destroyTemplateEntity( ent3 );
        world.destroyTemplateEntity( ent4 );
        world.destroyTemplateEntity( ent5 );

        REQUIRE_FALSE( world.hasTemplateEntity( ent1 ) );
        REQUIRE_FALSE( world.hasTemplateEntity( ent2 ) );
        REQUIRE_FALSE( world.hasTemplateEntity( ent3 ) );
        REQUIRE_FALSE( world.hasTemplateEntity( ent4 ) );
        REQUIRE_FALSE( world.hasTemplateEntity( ent5 ) );
    }

    SECTION( "Destinguishing between regular and template entity" )
    {
        entityid_t regularEnt = world.createEntity();
        entityid_t templateEnt = world.createTemplateEntity();

        REQUIRE( world.hasEntity( regularEnt ) );
        REQUIRE_FALSE( world.hasTemplateEntity( regularEnt ) );

        REQUIRE_FALSE( world.hasEntity( templateEnt ) );
        REQUIRE( world.hasTemplateEntity( templateEnt ) );
    }

    SECTION( "Applying components to template entity" )
    {
        entityid_t templEnt = world.createTemplateEntity();

        auto foo = world.createComponent<Foo>( templEnt );
        REQUIRE( foo );
        foo->x = 2;
        auto bar = world.createComponent<Bar>( templEnt );
        REQUIRE( bar );
        bar->y = 3;

        REQUIRE( world.hasComponent<Foo>( templEnt ) );
        REQUIRE( world.hasComponent<Bar>( templEnt ) );
        REQUIRE_FALSE( world.hasComponent<Baz>( templEnt ) );


        foo = world.getComponent<Foo>( templEnt );
        REQUIRE( foo );
        REQUIRE( foo->x == 2 );
        bar = world.getComponent<Bar>( templEnt );
        REQUIRE( bar );
        REQUIRE( bar->y == 3 );


        world.destroyComponent<Foo>( templEnt );
        REQUIRE_FALSE( world.hasComponent<Foo>( templEnt ) );
        REQUIRE( world.hasComponent<Bar>( templEnt ) );
    }

    SECTION( "Creating entities using templates" )
    {
        entityid_t templEnt1 = world.createTemplateEntity();
        entityid_t templEnt2 = world.createTemplateEntity();
        entityid_t regularEnt = world.createEntity();

        auto foo = world.createComponent<Foo>( templEnt1 );
        foo->x = 1;
        auto bar = world.createComponent<Bar>( templEnt1 );
        bar->y = 2;

        bar = world.createComponent<Bar>( templEnt2 );
        bar->y = 3;
        auto baz = world.createComponent<Baz>( templEnt2 );
        baz->z = 4;


        // try creating copy with non-template entity
        entityid_t entFromTemplateInvalid = world.createEntityFromTemplate( regularEnt );
        REQUIRE_FALSE( world.hasEntity( entFromTemplateInvalid ) );

        
        entityid_t ent1 = world.createEntityFromTemplate( templEnt1 );
        entityid_t ent2 = world.createEntityFromTemplate( templEnt2 );



        REQUIRE( world.hasEntity( ent1 ) );

        REQUIRE( world.hasComponent<Foo>( ent1 ) );
        foo = world.getComponent<Foo>( ent1 );
        REQUIRE( foo->x == 1 );

        REQUIRE( world.hasComponent<Bar>( ent1 ) );
        bar = world.getComponent<Bar>( ent1 );
        REQUIRE( bar->y == 2 );


        REQUIRE( world.hasEntity( ent2 ) );

        REQUIRE( world.hasComponent<Bar>( ent2 ) );
        bar = world.getComponent<Bar>( ent2 );
        REQUIRE( bar->y == 3 );

        REQUIRE( world.hasComponent<Baz>( ent2 ) );
        baz = world.getComponent<Baz>( ent2 );
        REQUIRE( baz->z == 4 );
    }

    SECTION( "Creating multiple entities using a template and querying" )
    {
        entityid_t templ1 = world.createTemplateEntity();
        entityid_t templ2 = world.createTemplateEntity();

        auto foo = world.createComponent<Foo>( templ1 );
        foo->x = 1;
        auto bar = world.createComponent<Bar>( templ1 );
        bar->y = 2;

        foo = world.createComponent<Foo>( templ2 );
        foo->x = 3;
        auto baz = world.createComponent<Baz>( templ2 );
        baz->z = 4;


        auto vecEnt1 = world.createEntitiesFromTemplate( templ1, 10 );
        REQUIRE( vecEnt1.size() == 10 );
        auto vecEnt2 = world.createEntitiesFromTemplate( templ2, 20 );
        REQUIRE( vecEnt2.size() == 20 );


        const CEntityQuery* query;

        queryid_t q1 = world.createQuery( makeEntitySignature<Foo>(), makeEntitySignature() );
        query = world.queryEntities(q1);
        REQUIRE( query->getEntityCount() == 30 );

        queryid_t q2 = world.createQuery( makeEntitySignature<Foo, Bar>(), CEntitySignature() );
        query = world.queryEntities(q2);
        REQUIRE( query->getEntityCount() == 10 );

        world.destroyQuery(q1);
        world.destroyQuery(q2);
    }
}



#include <future>

struct BarTraits
{
    static const segsize_t storageSegmentSize   = 1000;
    static const segsize_t storageInitCapacity  = 1000;
};

//TODO benchmark comp creation with different segment sizes
TEST_CASE( "Entity world test - multithreading" )
{
    CEntityWorld world;


    world.setupComponentType<Bar, BarTraits>();

    std::vector< entityid_t > ents;
    const entitysize_t ENT_COUNT = 100000;

    for (size_t i = 0; i < ENT_COUNT; i++)
    {
        entityid_t ent = world.createEntity();

        auto handle = world.createComponent<Bar>( ent );
        handle->y = i;

        ents.push_back( ent );
    }
    

    auto accumInRange = [&]( entityid_t begin, entityid_t end ) {
        long acc = 0;
        for (entityid_t i = begin; i < end; i++)
        {
            auto lock = world.lockForReading();
            acc += world.getComponent<Bar>( ents[i] )->y;
        }
        
        return acc;
    };

    auto addMoreEnts = [&] {
        for (size_t i = 0; i < 100; i++)
        {
            auto lock = world.lockForWriting();
            entityid_t ent = world.createEntity();
            auto handle = world.createComponent<Bar>( ent );
            handle->y = i;
        }
    };



    // The task is to sum all y values from Bar components in vector ents
    // while also adding a few extra entities to the world

    /// Deferred ///
    long sumDeferred = 0;

    auto start = std::chrono::steady_clock::now();

    sumDeferred += accumInRange( 0, ENT_COUNT/2 );
    addMoreEnts();
    sumDeferred += accumInRange( ENT_COUNT/2, ENT_COUNT );

    auto finish = std::chrono::steady_clock::now();

    auto timeDeferred = finish - start; 



    /// Multithreaded ///
    start = std::chrono::steady_clock::now();

    auto task1Result = std::async( std::launch::async, accumInRange, 0, ENT_COUNT/2 );
    auto taskInterrupter = std::async( std::launch::async, addMoreEnts );
    auto task2Result = std::async( std::launch::async, accumInRange, ENT_COUNT/2, ENT_COUNT );

    long sumAsync = task1Result.get() + task2Result.get();

    finish = std::chrono::steady_clock::now();

    auto timeAsync = finish - start;


    REQUIRE( sumDeferred == sumAsync );

    REQUIRE( timeAsync.count() < timeDeferred.count() );
    
    WARN( "Async time:" + std::to_string(timeAsync.count()) );
    WARN( "Deferred time:" + std::to_string(timeDeferred.count()) );
}






TEST_CASE( "Entity world test - benchmarks", "[benchmark]" )
{
    const entityid_t ENTITY_COUNT = 100;
    CEntityWorld world;

    BENCHMARK( "Creating a lot of similair entities - naive approach" )
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

    BENCHMARK( "Creating a lot of similair entities - naive approach with usage of getter" )
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

    BENCHMARK( "Creating a lot of similair entities - naive approach with first pre-creating entities" )
    {
        auto vecEnt = world.createEntities( ENTITY_COUNT );
        CComponentHandle<Foo> foo;
        CComponentHandle<Bar> bar;
        CComponentHandle<Baz> baz;

        for (entityid_t i = 0; i < ENTITY_COUNT; i++)
        {
            // every foo will be different
            foo = world.createComponent<Foo>( vecEnt[i] );
            foo->x = i;

            // but every bar and baz will be the same
            bar = world.createComponent<Bar>( vecEnt[i] );
            bar->y = 2137;
            baz = world.createComponent<Baz>( vecEnt[i] );
            baz->z = 123;
        }

        return foo->x; //returning something because catch2 wants it
    };

    BENCHMARK( "Creating a lot of similair entities - template entity approach" )
    {
        // we'll create these entities one by one

        entityid_t templ = world.createTemplateEntity();

        auto foo = world.createComponent<Foo>( templ );
        // every foo will be different, so no point in setting it here

        // but every bar and baz will be the same
        auto bar = world.createComponent<Bar>( templ );
        bar->y = 2137;
        auto baz = world.createComponent<Baz>( templ );
        baz->z = 123;


        for (entityid_t i = 0; i < ENTITY_COUNT; i++)
        {
            entityid_t ent = world.createEntityFromTemplate( templ );

            world.getComponent<Foo>( ent )->x = i;
        }

        return foo->x; //returning something because catch2 wants it
    }; 

    BENCHMARK( "Creating a lot of similair entities - template entity approach with pre-creating entities" )
    {
        // we'll create these entities one by one

        entityid_t templ = world.createTemplateEntity();

        auto foo = world.createComponent<Foo>( templ );
        // every foo will be different, so no point in setting it here

        // but every bar and baz will be the same
        auto bar = world.createComponent<Bar>( templ );
        bar->y = 2137;
        auto baz = world.createComponent<Baz>( templ );
        baz->z = 123;


        auto vecEnt = world.createEntitiesFromTemplate( templ, ENTITY_COUNT );

        for (entityid_t i = 0; i < ENTITY_COUNT; i++)
        {
            world.getComponent<Foo>( vecEnt[i] )->x = i;
        }

        return foo->x; //returning something because catch2 wants it
    }; 
}
