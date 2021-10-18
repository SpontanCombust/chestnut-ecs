#include <catch2/catch.hpp>

#include "../include/chestnut/ecs/component_batch_guard.hpp"

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

TEST_CASE( "Component batch test" )
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

        CComponentBatchGuard batchGuard = CComponentBatchGuard( sign, &storageMap );

        for (entityid i = 1; i < 16; i++)
        {
            batchGuard.fetchAndAddEntityWithComponents(i);
        }
        batchGuard.updateBatch();

        const SComponentBatch *batch = batchGuard.getBatchPtr();

        const auto& vecEnts = batch->vecEntityIDs;
        const auto& vecFoo = batch->mapCompTypeToCompVec.at( fooType );

        REQUIRE( vecEnts.size() == 15 );
        for (entityid i = 0; i < vecEnts.size(); i++)
        {
            SComponentWrapper<Foo> *foo = dynamic_cast< SComponentWrapper<Foo> * >( vecFoo[i] );
            REQUIRE( foo );
        }
    }

    SECTION( "Test entities with Foo and Baz" )
    {
        CEntitySignature sign;
        sign.add<Foo>();
        sign.add<Baz>();

        CComponentBatchGuard batchGuard = CComponentBatchGuard( sign, &storageMap );

        for (entityid i = 11; i < 16; i++)
        {
            batchGuard.fetchAndAddEntityWithComponents(i);
        }
        batchGuard.updateBatch();

        const SComponentBatch *batch = batchGuard.getBatchPtr();

        const auto& vecEnts = batch->vecEntityIDs;
        const auto& vecFoo = batch->mapCompTypeToCompVec.at( fooType );
        const auto& vecBaz = batch->mapCompTypeToCompVec.at( bazType );

        REQUIRE( vecEnts.size() == 5 );
        for (entityid i = 0; i < vecEnts.size(); i++)
        {
            SComponentWrapper<Foo> *foo = dynamic_cast< SComponentWrapper<Foo> * >( vecFoo[i] );
            SComponentWrapper<Baz> *baz = dynamic_cast< SComponentWrapper<Baz> * >( vecBaz[i] );
            REQUIRE( foo );
            REQUIRE( baz );
        }
    }

    SECTION( "Removing components" )
    {
        CEntitySignature sign;
        sign.add<Bar>();
        sign.add<Baz>();

        CComponentBatchGuard batchGuard = CComponentBatchGuard( sign, &storageMap );

        for (entityid i = 11; i < 21; i++)
        {
            batchGuard.fetchAndAddEntityWithComponents(i);
        }
        batchGuard.updateBatch();

        for (entityid i = 11; i < 16; i++)
        {
            batchGuard.removeEntityWithComponents( i );
        }
        batchGuard.updateBatch();

        const SComponentBatch *batch = batchGuard.getBatchPtr();

        const auto& vecEnts = batch->vecEntityIDs;
        const auto& vecBar = batch->mapCompTypeToCompVec.at( barType );
        const auto& vecBaz = batch->mapCompTypeToCompVec.at( bazType );

        REQUIRE( vecEnts.size() == 5 );
        for (entityid i = 0; i < vecEnts.size(); i++)
        {
            SComponentWrapper<Bar> *bar = dynamic_cast< SComponentWrapper<Bar> * >( vecBar[i] );
            SComponentWrapper<Baz> *baz = dynamic_cast< SComponentWrapper<Baz> * >( vecBaz[i] );
            REQUIRE( bar );
            REQUIRE( baz );
        }
    }

    SECTION( "Fetching and instantly removing components" )
    {
        CEntitySignature sign;
        sign.add<Foo>();
        sign.add<Bar>();

        CComponentBatchGuard batchGuard = CComponentBatchGuard( sign, &storageMap );

        for (entityid i = 6; i < 16; i++)
        {
            batchGuard.fetchAndAddEntityWithComponents(i);
        }
        for (entityid i = 11; i < 16; i++)
        {
            batchGuard.removeEntityWithComponents( i );
        }

        batchGuard.updateBatch();

        const SComponentBatch *batch = batchGuard.getBatchPtr();

        const auto& vecEnts = batch->vecEntityIDs;
        const auto& vecFoo = batch->mapCompTypeToCompVec.at( fooType );
        const auto& vecBar = batch->mapCompTypeToCompVec.at( barType );

        REQUIRE( vecEnts.size() == 5 );
        for (entityid i = 0; i < vecEnts.size(); i++)
        {
            SComponentWrapper<Foo> *foo = dynamic_cast< SComponentWrapper<Foo> * >( vecFoo[i] );
            SComponentWrapper<Bar> *bar = dynamic_cast< SComponentWrapper<Bar> * >( vecBar[i] );
            REQUIRE( foo );
            REQUIRE( bar );
        }
    }

    SECTION( "Fetching and after that removing and fetching the same" )
    {
        CEntitySignature sign;
        sign.add<Foo>();

        CComponentBatchGuard batchGuard = CComponentBatchGuard( sign, &storageMap );

        for (entityid i = 1; i < 16; i++)
        {
            batchGuard.fetchAndAddEntityWithComponents(i);
        }
        batchGuard.updateBatch();

        for (entityid i = 1; i < 6; i++)
        {
            batchGuard.removeEntityWithComponents( i );
        }
        for (entityid i = 11; i < 16; i++)
        {
            batchGuard.removeEntityWithComponents( i );
        }
        for (entityid i = 1; i < 6; i++)
        {
            batchGuard.fetchAndAddEntityWithComponents(i);
        }
        batchGuard.updateBatch();

        const SComponentBatch *batch = batchGuard.getBatchPtr();

        const auto& vecEnts = batch->vecEntityIDs;
        const auto& vecFoo = batch->mapCompTypeToCompVec.at( fooType );

        REQUIRE( vecEnts.size() == 10 );
        for (entityid i = 0; i < vecEnts.size(); i++)
        {
            SComponentWrapper<Foo> *foo = dynamic_cast< SComponentWrapper<Foo> * >( vecFoo[i] );
            REQUIRE( foo );
        }
    }
}