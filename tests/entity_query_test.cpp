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

TEST_CASE( "Entity query test - forEach" )
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

        for (entityid_t i = 1; i <= 15; i++)
        {
            guard.fetchAndAddEntityWithComponents(i);
        }
        guard.updateQuery();

        CEntityQuery* query = guard.getQuery();

        REQUIRE( query->getEntityCount() == 15 );
        REQUIRE_NOTHROW( 
            query->forEachEntityWith<Foo>( []( entityid_t id, Foo& foo ) 
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

        for (entityid_t i = 11; i <= 15; i++)
        {
            guard.fetchAndAddEntityWithComponents(i);
        }
        guard.updateQuery();

        CEntityQuery* query = guard.getQuery();

        REQUIRE( query->getEntityCount() == 5 );
        REQUIRE_NOTHROW(
            query->forEachEntityWith<Foo, Baz>( []( entityid_t id, Foo& foo, Baz& baz ) 
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

        for (entityid_t i = 1; i <= 10; i++)
        {
            guard.fetchAndAddEntityWithComponents(i);
        }
        guard.updateQuery();

        CEntityQuery* query = guard.getQuery();

        REQUIRE( query->getEntityCount() == 10 );
        REQUIRE_NOTHROW(
            query->forEachEntityWith<Foo>( []( entityid_t id, Foo& foo ) 
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

        for (entityid_t i = 11; i <= 20; i++)
        {
            guard.fetchAndAddEntityWithComponents(i);
        }
        guard.updateQuery();

        for (entityid_t i = 11; i <= 15; i++)
        {
            guard.removeEntityWithComponents( i );
        }
        guard.updateQuery();

        CEntityQuery* query = guard.getQuery();

        REQUIRE( query->getEntityCount() == 5 );
        REQUIRE_NOTHROW(
            query->forEachEntityWith<Bar, Baz>( []( entityid_t id, Bar& bar, Baz& baz ) 
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

        for (entityid_t i = 6; i < 16; i++)
        {
            guard.fetchAndAddEntityWithComponents(i);
        }
        for (entityid_t i = 11; i < 16; i++)
        {
            guard.removeEntityWithComponents(i);
        }

        guard.updateQuery();

        CEntityQuery* query = guard.getQuery();

        REQUIRE( query->getEntityCount() == 5 );
        REQUIRE_NOTHROW(
            query->forEachEntityWith<Foo, Bar>( []( entityid_t id, Foo& foo, Bar& baz ) 
            {
                REQUIRE( (id >= 6 && id <= 10) );
            })
        );
    }

    SECTION( "Fetching (Foo) and after that removing and fetching the same" )
    {
        CEntitySignature sign;
        sign.add<Foo>();

        CEntityQueryGuard guard = CEntityQueryGuard( 0, sign, CEntitySignature(), &storageMap );

        for (entityid_t i = 1; i < 16; i++)
        {
            guard.fetchAndAddEntityWithComponents(i);
        }
        guard.updateQuery();

        for (entityid_t i = 1; i <= 5; i++)
        {
            guard.removeEntityWithComponents( i );
        }
        for (entityid_t i = 11; i <= 15; i++)
        {
            guard.removeEntityWithComponents( i );
        }
        for (entityid_t i = 1; i <= 5; i++)
        {
            guard.fetchAndAddEntityWithComponents(i);
        }
        guard.updateQuery();

        CEntityQuery* query = guard.getQuery();

        REQUIRE( query->getEntityCount() == 10 );
        REQUIRE_NOTHROW(
            query->forEachEntityWith<Foo>( []( entityid_t id, Foo& foo ) 
            {
                REQUIRE( ( id >= 1 && id <= 10 ) );
            })
        );
    }
}


TEST_CASE( "Entity query test - sorting" )
{
    CComponentStorageTypeMap storageMap;

    std::type_index fooType = typeid( Foo );
    std::type_index barType = typeid( Bar );

    auto fooStorage = std::make_unique< CComponentStorage<Foo> >( 10, 10 );
    auto barStorage = std::make_unique< CComponentStorage<Bar> >( 15, 15 );

    storageMap[ fooType ] = fooStorage.get();
    storageMap[ barType ] = barStorage.get();

    ( (SComponentWrapper<Foo> *)fooStorage->storeComponent(1) )->data.x = 235;
    ( (SComponentWrapper<Foo> *)fooStorage->storeComponent(2) )->data.x = 917;
    ( (SComponentWrapper<Foo> *)fooStorage->storeComponent(3) )->data.x = 184;
    ( (SComponentWrapper<Foo> *)fooStorage->storeComponent(4) )->data.x = 281;
    ( (SComponentWrapper<Foo> *)fooStorage->storeComponent(5) )->data.x = 893;

    ( (SComponentWrapper<Bar> *)barStorage->storeComponent(1) )->data.y = 963;
    ( (SComponentWrapper<Bar> *)barStorage->storeComponent(2) )->data.y = 8193;
    ( (SComponentWrapper<Bar> *)barStorage->storeComponent(3) )->data.y = 1782;
    ( (SComponentWrapper<Bar> *)barStorage->storeComponent(4) )->data.y = 1396;
    ( (SComponentWrapper<Bar> *)barStorage->storeComponent(5) )->data.y = 1008;
    

    CEntityQueryGuard guard = CEntityQueryGuard( 0, makeEntitySignature<Foo,Bar>(), makeEntitySignature(), &storageMap );

    for (entityid_t i = 1; i <= 5; i++)
    {
        guard.fetchAndAddEntityWithComponents(i);
    }
    guard.updateQuery();

    CEntityQuery* query = guard.getQuery();
    REQUIRE( query->getEntityCount() == 5 );
    query->sort<Foo>( []( const Foo& f1, const Foo& f2 ) -> bool
    {
        return f1.x < f2.x;
    });

    int i = 0;
    REQUIRE_NOTHROW(
        query->forEachEntityWith<Foo, Bar>( [&i]( entityid_t id, Foo& foo, Bar& bar ) 
        {
            if( i == 0 )
            {
                REQUIRE( id == 3 );
                REQUIRE( foo.x == 184 );
                REQUIRE( bar.y == 1782 );
            }
            else if( i == 1 )
            {
                REQUIRE( id == 1 );
                REQUIRE( foo.x == 235 );
                REQUIRE( bar.y == 963 );
            }
            else if( i == 2 )
            {
                REQUIRE( id == 4 );
                REQUIRE( foo.x == 281 );
                REQUIRE( bar.y == 1396 );
            }
            else if( i == 3 )
            {
                REQUIRE( id == 5 );
                REQUIRE( foo.x == 893 );
                REQUIRE( bar.y == 1008 );
            }
            else if( i == 4 )
            {
                REQUIRE( id == 2 );
                REQUIRE( foo.x == 917 );
                REQUIRE( bar.y == 8193 );
            }
            else
            {
                REQUIRE( false );
            }

            i++;
        })
    );



    query->sort<Bar>( 
    []( const Bar& b1, const Bar& b2 ) -> bool
    {
        return b1.y < b2.y;
    });

    i = 0;

    REQUIRE_NOTHROW(
        query->forEachEntityWith<Foo, Bar>( [&i]( entityid_t id, Foo& foo, Bar& bar ) 
        {
            if( i == 0 )
            {
                REQUIRE( id == 1 );
                REQUIRE( foo.x == 235 );
                REQUIRE( bar.y == 963 );
            }
            else if( i == 1 )
            {
                REQUIRE( id == 5 );
                REQUIRE( foo.x == 893 );
                REQUIRE( bar.y == 1008 );
            }
            else if( i == 2 )
            {
                REQUIRE( id == 4 );
                REQUIRE( foo.x == 281 );
                REQUIRE( bar.y == 1396 );
            }
            else if( i == 3 )
            {
                REQUIRE( id == 3 );
                REQUIRE( foo.x == 184 );
                REQUIRE( bar.y == 1782 );
            }
            else if( i == 4 )
            {
                REQUIRE( id == 2 );
                REQUIRE( foo.x == 917 );
                REQUIRE( bar.y == 8193 );
            }
            else
            {
                REQUIRE( false );
            }

            i++;
        })
    );
}