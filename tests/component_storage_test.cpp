#include <catch2/catch.hpp>

#include "../include/chestnut/ecs/component_storage.hpp"

using namespace chestnut::ecs;
using namespace chestnut::ecs::internal;

class Foo
{
public:
    int i;
};

TEST_CASE( "Component storage test" )
{
    SECTION( "Initial state for initial capacity equal zero" )
    {
        CComponentStorage<Foo> storage = CComponentStorage<Foo>( 10, 0 );

        REQUIRE( storage.getSize() == 0 );
        REQUIRE( storage.getCapacity() == 0 );
        REQUIRE( storage.getEmptySegmentTotalSize() == 0 );
    }

    SECTION( "Initial state for initial capacity lesser than segment size" )
    {
        CComponentStorage<Foo> storage = CComponentStorage<Foo>( 10, 9 );

        REQUIRE( storage.getSize() == 0 );
        REQUIRE( storage.getCapacity() == 10 );
        REQUIRE( storage.getEmptySegmentTotalSize() == 10 );
    }

    SECTION( "Initial state for initial capacity greater than segment size" )
    {
        CComponentStorage<Foo> storage = CComponentStorage<Foo>( 10, 21 );

        REQUIRE( storage.getSize() == 0 );
        REQUIRE( storage.getCapacity() == 30 );
        REQUIRE( storage.getEmptySegmentTotalSize() == 30 );
    }
    


    CComponentStorage<Foo> storage = CComponentStorage<Foo>( 5, 10 );

    REQUIRE( storage.getSize() == 0 );
    REQUIRE( storage.getCapacity() == 10 );
    REQUIRE( storage.getEmptySegmentTotalSize() == 10 );

    SECTION( "Creating components and checking storage size" )
    {
        REQUIRE( storage.getSize() == 0 );
        REQUIRE( storage.getCapacity() == 10 );
        REQUIRE( storage.getEmptySegmentTotalSize() == 10 );


        // filling up 1st segment
        for (size_t i = 1; i < 6; i++)
        {
            storage.storeComponent(i);
        }

        REQUIRE( storage.getSize() == 5 );
        REQUIRE( storage.getCapacity() == 10 );
        REQUIRE( storage.getEmptySegmentTotalSize() == 5 );

        // filling up 2nd segment
        for (size_t i = 6; i < 11; i++)
        {
            storage.storeComponent(i);
        }

        REQUIRE( storage.getSize() == 10 );
        REQUIRE( storage.getCapacity() == 10 );
        REQUIRE( storage.getEmptySegmentTotalSize() == 0 );

        // creating 2 new segments
        // filling 1st one fully and 2nd one partially
        for (size_t i = 11; i < 19; i++)
        {
            storage.storeComponent(i);
        }

        REQUIRE( storage.getSize() == 18 );
        REQUIRE( storage.getCapacity() == 20 );
        REQUIRE( storage.getEmptySegmentTotalSize() == 0 );

        // try creating components with previously used IDs
        // should change nothing
        for (size_t i = 1; i < 6; i++)
        {
            storage.storeComponent(i);
        }

        REQUIRE( storage.getSize() == 18 );
        REQUIRE( storage.getCapacity() == 20 );
        REQUIRE( storage.getEmptySegmentTotalSize() == 0 );
    }

    SECTION( "Checking and fetching components" )
    {
        SComponentWrapper<Foo> *foo;
        for (size_t i = 1; i < 7; i++)
        {
            foo = (SComponentWrapper<Foo> *)storage.storeComponent(i);
            foo->data.i = i;

            REQUIRE( storage.hasComponent(i) );

            foo = (SComponentWrapper<Foo> *)storage.getComponent(i);
            REQUIRE( foo != nullptr );
            REQUIRE( foo->data.i == i );

            foo = (SComponentWrapper<Foo> *)storage.storeComponent(i);
            REQUIRE( foo->data.i == i );
        }

        REQUIRE_FALSE( storage.hasComponent(0) );
        REQUIRE_FALSE( storage.hasComponent(7) ); //!FIXME
    }

    SECTION( "Erasing components" )
    {
        // fill storage with 20 components (ids 1-20)

        for (size_t i = 1; i < 21; i++)
        {
            storage.storeComponent(i);
        }

        REQUIRE( storage.getSize() == 20 );
        REQUIRE( storage.getCapacity() == 20 );
        REQUIRE( storage.getEmptySegmentTotalSize() == 0 );
        

        // erase 5 components from the same segment (ids 6-10)

        for (size_t i = 6; i < 11; i++)
        {
            storage.eraseComponent(i);

            REQUIRE_FALSE( storage.hasComponent(i) );
        }
        
        REQUIRE( storage.getSize() == 15 );
        REQUIRE( storage.getCapacity() == 20 );
        REQUIRE( storage.getEmptySegmentTotalSize() == 5 );


        // erase another 5 components, but from different segments (ids 3-5 and 11-12)

        for (size_t i = 3; i < 6; i++)
        {
            REQUIRE_NOTHROW( storage.eraseComponent(i) );

            REQUIRE_FALSE( storage.hasComponent(i) );
        }
        for (size_t i = 11; i < 13; i++)
        {
            REQUIRE_NOTHROW( storage.eraseComponent(i) );

            REQUIRE_FALSE( storage.hasComponent(i) );
        }
        
        REQUIRE( storage.getSize() == 10 );
        REQUIRE( storage.getCapacity() == 20 );
        REQUIRE( storage.getEmptySegmentTotalSize() == 5 );


        // try erasing already erased components

        storage.eraseComponent(4);
        storage.eraseComponent(7);
        storage.eraseComponent(12);

        REQUIRE( storage.getSize() == 10 );
        REQUIRE( storage.getCapacity() == 20 );
        REQUIRE( storage.getEmptySegmentTotalSize() == 5 );


        // remove empty segments

        storage.removeEmptySegments();

        REQUIRE( storage.getSize() == 10 );
        REQUIRE( storage.getCapacity() == 15 );
        REQUIRE( storage.getEmptySegmentTotalSize() == 0 );


        // clear storage completely

        storage.clearComponents();

        REQUIRE( storage.getSize() == 0 );
        REQUIRE( storage.getCapacity() == 15 );
        REQUIRE( storage.getEmptySegmentTotalSize() == 15 );


        storage.removeEmptySegments();

        REQUIRE( storage.getSize() == 0 );
        REQUIRE( storage.getCapacity() == 0 );
        REQUIRE( storage.getEmptySegmentTotalSize() == 0 );
    }

    SECTION( "Modifying capacity" )
    {
        // first check if it won't change anything if totalSize is smaller than current capacity

        storage.reserve(8);

        REQUIRE( storage.getSize() == 0 );
        REQUIRE( storage.getCapacity() == 10 );
        REQUIRE( storage.getEmptySegmentTotalSize() == 10 );


        // now increase capacity

        storage.reserve(14);

        REQUIRE( storage.getSize() == 0 );
        REQUIRE( storage.getCapacity() == 15 );
        REQUIRE( storage.getEmptySegmentTotalSize() == 15 );


        storage.reserveAdditional(1);

        REQUIRE( storage.getSize() == 0 );
        REQUIRE( storage.getCapacity() == 20 );
        REQUIRE( storage.getEmptySegmentTotalSize() == 20 );


        // resize to bigger size

        storage.resize(25);

        REQUIRE( storage.getSize() == 0 );
        REQUIRE( storage.getCapacity() == 25 );
        REQUIRE( storage.getEmptySegmentTotalSize() == 25 );


        // try to resize again to the same capacity

        storage.resize(24);

        REQUIRE( storage.getSize() == 0 );
        REQUIRE( storage.getCapacity() == 25 );
        REQUIRE( storage.getEmptySegmentTotalSize() == 25 );


        // resize to smaller size

        storage.resize(11);

        REQUIRE( storage.getSize() == 0 );
        REQUIRE( storage.getCapacity() == 15 );
        REQUIRE( storage.getEmptySegmentTotalSize() == 15 );


        // try downsizing when there are components in segments

        for (size_t i = 1; i < 16; i++)
        {
            storage.storeComponent(i);
        }
        storage.eraseComponent(1);
        storage.eraseComponent(2);
        storage.eraseComponent(3);
        storage.eraseComponent(4);

        storage.eraseComponent(7);
        storage.eraseComponent(8);
        storage.eraseComponent(9);
        storage.eraseComponent(10);

        storage.eraseComponent(11);
        storage.eraseComponent(12);
        storage.eraseComponent(13);
        storage.eraseComponent(14);
        storage.eraseComponent(15);

        // only one segment left empty
        // first 2 segments have 1 component in each of them

        storage.resize(5);

        REQUIRE( storage.getSize() == 2 );
        REQUIRE( storage.getCapacity() == 10 );
        REQUIRE( storage.getEmptySegmentTotalSize() == 0 );
    }
}