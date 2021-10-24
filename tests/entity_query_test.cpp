#include <catch2/catch.hpp>

#include "../include/chestnut/ecs/entity_query_guard.hpp"

#include <memory> // unique_ptr

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

TEST_CASE( "Entity query test" )
{
    // first setup the component storage map using a few component types

    CComponentStorageTypeMap storageMap;

    std::type_index fooType = typeid( Foo );
    std::type_index barType = typeid( Bar );
    std::type_index bazType = typeid( Baz );

    auto fooStorage = std::make_unique< CComponentStorage<Foo> >( 10, 10 );
    auto barStorage = std::make_unique< CComponentStorage<Bar> >( 15, 15 );
    auto bazStorage = std::make_unique< CComponentStorage<Baz> >( 20, 20 );

    storageMap[ fooType ] = fooStorage.get();
    storageMap[ barType ] = barStorage.get();
    storageMap[ bazType ] = bazStorage.get();


    // entities 1-5 have Foo
    fooStorage->storeComponent(1);
    fooStorage->storeComponent(2);
    fooStorage->storeComponent(3);
    fooStorage->storeComponent(4);
    fooStorage->storeComponent(5);

    // entities 6-10 have Foo and Bar
    fooStorage->storeComponent(6);
    barStorage->storeComponent(6);    
    fooStorage->storeComponent(7);
    barStorage->storeComponent(7);    
    fooStorage->storeComponent(8);
    barStorage->storeComponent(8);    
    fooStorage->storeComponent(9);
    barStorage->storeComponent(9);    
    fooStorage->storeComponent(10);
    barStorage->storeComponent(10);

    // entities 11-15 have Foo, Bar and Baz
    fooStorage->storeComponent(11);
    barStorage->storeComponent(11);
    bazStorage->storeComponent(11);    
    fooStorage->storeComponent(12);
    barStorage->storeComponent(12);
    bazStorage->storeComponent(12);    
    fooStorage->storeComponent(13);
    barStorage->storeComponent(13);
    bazStorage->storeComponent(13);    
    fooStorage->storeComponent(14);
    barStorage->storeComponent(14);
    bazStorage->storeComponent(14);    
    fooStorage->storeComponent(15);
    barStorage->storeComponent(15);
    bazStorage->storeComponent(15);

    // entities 16-20 have Bar and Baz
    barStorage->storeComponent(16);
    bazStorage->storeComponent(16);
    barStorage->storeComponent(17);
    bazStorage->storeComponent(17);
    barStorage->storeComponent(18);
    bazStorage->storeComponent(18);
    barStorage->storeComponent(19);
    bazStorage->storeComponent(19);
    barStorage->storeComponent(20);
    bazStorage->storeComponent(20);


    SECTION( "Test entities with Foo" )
    {
        CEntitySignature sign;
        sign.add<Foo>();

        CEntityQueryGuard guard = CEntityQueryGuard( 0, sign, CEntitySignature(), &storageMap );

        for (entityid i = 1; i <= 15; i++)
        {
            guard.fetchAndAddEntityWithComponents(i);
        }
        guard.updateQuery();

        const CEntityQuery& query = guard.getQuery();

        REQUIRE( query.getEntityCount() == 15 );
        REQUIRE_NOTHROW( 
            query.forEachEntityWith<Foo>( []( entityid id, Foo& foo ) 
            {
                id >= 1 && id <= 15;
            })
        );
    }

    SECTION( "Test entities with Foo and Baz" )
    {
        CEntitySignature sign;
        sign.add<Foo>();
        sign.add<Baz>();

        CEntityQueryGuard guard = CEntityQueryGuard( 0, sign, CEntitySignature(), &storageMap );

        for (entityid i = 11; i <= 15; i++)
        {
            guard.fetchAndAddEntityWithComponents(i);
        }
        guard.updateQuery();

        const CEntityQuery& query = guard.getQuery();

        REQUIRE( query.getEntityCount() == 5 );
        REQUIRE_NOTHROW(
            query.forEachEntityWith<Foo, Baz>( []( entityid id, Foo& foo, Baz& baz ) 
            {
                id >= 11 && id <= 15;
            })
        );
    }

    SECTION( "Test entities with Foo, but no Baz" )
    {
        CEntitySignature signRequest;
        signRequest.add<Foo>();

        CEntitySignature signReject;
        signReject.add<Baz>();

        CEntityQueryGuard guard = CEntityQueryGuard( 0, signRequest, signReject, &storageMap );

        for (entityid i = 1; i <= 10; i++)
        {
            guard.fetchAndAddEntityWithComponents(i);
        }
        guard.updateQuery();

        const CEntityQuery& query = guard.getQuery();

        REQUIRE( query.getEntityCount() == 10 );
        REQUIRE_NOTHROW(
            query.forEachEntityWith<Foo>( []( entityid id, Foo& foo ) 
            {
                REQUIRE( ( id >= 1 && id <= 10 ) );
            })
        );
    }

    SECTION( "Removing (Bar and Baz) components" )
    {
        CEntitySignature sign;
        sign.add<Bar>();
        sign.add<Baz>();

        CEntityQueryGuard guard = CEntityQueryGuard( 0, sign, CEntitySignature(), &storageMap );

        for (entityid i = 11; i <= 20; i++)
        {
            guard.fetchAndAddEntityWithComponents(i);
        }
        guard.updateQuery();

        for (entityid i = 11; i <= 15; i++)
        {
            guard.removeEntityWithComponents( i );
        }
        guard.updateQuery();

        const CEntityQuery& query = guard.getQuery();

        REQUIRE( query.getEntityCount() == 5 );
        REQUIRE_NOTHROW(
            query.forEachEntityWith<Bar, Baz>( []( entityid id, Bar& bar, Baz& baz ) 
            {
                id >= 16 && id <= 20;
            })
        );
    }

    SECTION( "Fetching and instantly removing (Foo and Bar) components" )
    {
        CEntitySignature sign;
        sign.add<Foo>();
        sign.add<Bar>();

        CEntityQueryGuard guard = CEntityQueryGuard( 0, sign, CEntitySignature(), &storageMap );

        for (entityid i = 6; i < 16; i++)
        {
            guard.fetchAndAddEntityWithComponents(i);
        }
        for (entityid i = 11; i < 16; i++)
        {
            guard.removeEntityWithComponents(i);
        }

        guard.updateQuery();

        const CEntityQuery& query = guard.getQuery();

        REQUIRE( query.getEntityCount() == 5 );
        REQUIRE_NOTHROW(
            query.forEachEntityWith<Foo, Bar>( []( entityid id, Foo& foo, Bar& baz ) 
            {
                id >= 6 && id <= 10;
            })
        );
    }

    SECTION( "Fetching (Foo) and after that removing and fetching the same" )
    {
        CEntitySignature sign;
        sign.add<Foo>();

        CEntityQueryGuard guard = CEntityQueryGuard( 0, sign, CEntitySignature(), &storageMap );

        for (entityid i = 1; i < 16; i++)
        {
            guard.fetchAndAddEntityWithComponents(i);
        }
        guard.updateQuery();

        for (entityid i = 1; i <= 5; i++)
        {
            guard.removeEntityWithComponents( i );
        }
        for (entityid i = 11; i <= 15; i++)
        {
            guard.removeEntityWithComponents( i );
        }
        for (entityid i = 1; i <= 5; i++)
        {
            guard.fetchAndAddEntityWithComponents(i);
        }
        guard.updateQuery();

        const CEntityQuery& query = guard.getQuery();

        REQUIRE( query.getEntityCount() == 10 );
        REQUIRE_NOTHROW(
            query.forEachEntityWith<Foo>( []( entityid id, Foo& foo ) 
            {
                id >= 1 && id <= 10;
            })
        );
    }
}