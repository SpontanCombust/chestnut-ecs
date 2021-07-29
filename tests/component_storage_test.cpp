#include <catch2/catch.hpp>

#include "../include/chestnut/ecs/component_storage.hpp"

using namespace chestnut;

class Foo : public CComponent
{
public:
    int i;
};

TEST_CASE( "Component storage test" )
{
    SECTION( "Assert template type derived from  CComponent" )
    {
        // uncomment when trying to compile
        // CComponentStorage<int> storage = CComponentStorage<int>( 10, 10 );
    }

    SECTION( "Initial state for initial capacity equal zero" )
    {
        CComponentStorage<Foo> storage = CComponentStorage<Foo>( 10 );

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
        // test the invalid entity check
        REQUIRE( storage.createComponent(0) == nullptr );

        REQUIRE( storage.getSize() == 0 );
        REQUIRE( storage.getCapacity() == 10 );
        REQUIRE( storage.getEmptySegmentTotalSize() == 10 );


        // filling up 1st segment
        for (size_t i = 1; i < 6; i++)
        {
            storage.createComponent(i);
        }

        REQUIRE( storage.getSize() == 5 );
        REQUIRE( storage.getCapacity() == 10 );
        REQUIRE( storage.getEmptySegmentTotalSize() == 5 );

        // filling up 2nd segment
        for (size_t i = 6; i < 11; i++)
        {
            storage.createComponent(i);
        }

        REQUIRE( storage.getSize() == 10 );
        REQUIRE( storage.getCapacity() == 10 );
        REQUIRE( storage.getEmptySegmentTotalSize() == 0 );

        // creating 2 new segments
        // filling 1st one fully and 2nd one partially
        for (size_t i = 11; i < 19; i++)
        {
            storage.createComponent(i);
        }

        REQUIRE( storage.getSize() == 18 );
        REQUIRE( storage.getCapacity() == 20 );
        REQUIRE( storage.getEmptySegmentTotalSize() == 0 );

        // try creating components with previously used IDs
        // should change nothing
        for (size_t i = 1; i < 6; i++)
        {
            storage.createComponent(i);
        }

        REQUIRE( storage.getSize() == 18 );
        REQUIRE( storage.getCapacity() == 20 );
        REQUIRE( storage.getEmptySegmentTotalSize() == 0 );
    }

    SECTION( "Checking and fetching components by entity" )
    {
        Foo *foo;
        for (size_t i = 1; i < 7; i++)
        {
            foo = (Foo *)storage.createComponent(i);
            foo->i = i;

            REQUIRE( storage.hasComponent(i) );

            foo = (Foo *)storage.getComponentByEntity(i);
            REQUIRE( foo != nullptr );
            REQUIRE( foo->i == i );

            foo = (Foo *)storage.createComponent(i);
            REQUIRE( foo->i == i );
        }

        REQUIRE_FALSE( storage.hasComponent(0) );
        REQUIRE_FALSE( storage.hasComponent(7) );
    }

    SECTION( "Fetching components by index" )
    {
        Foo *foo;
        SComponentIndex index;
        for (size_t i = 1; i < 12; i++)
        {
            foo = (Foo *)storage.createComponent(i);
            foo->i = i;

            REQUIRE_NOTHROW( index = storage.getComponentIndexByEntity(i) );

            REQUIRE_NOTHROW( foo = (Foo *)storage.getComponentByIndex( index ) );
            REQUIRE( foo->i == i );

            REQUIRE_NOTHROW( foo = (Foo *)storage[ index ] );
            REQUIRE( foo->i == i );
        }

        REQUIRE_THROWS( storage.getComponentIndexByEntity(0) );
        REQUIRE_THROWS( storage.getComponentIndexByEntity(12) );

        index.segmentIndex = 9707;
        index.segmentSlotIndex = 0;
        REQUIRE_THROWS( storage.getComponentByIndex( index ) );

        index.segmentIndex = 0;
        index.segmentSlotIndex = 178241;
        REQUIRE_THROWS( storage.getComponentByIndex( index ) );
    }

    SECTION( "Erasing components" )
    {
        // fill storage with 20 components (ids 1-20)

        for (size_t i = 1; i < 21; i++)
        {
            storage.createComponent(i);
        }

        REQUIRE( storage.getSize() == 20 );
        REQUIRE( storage.getCapacity() == 20 );
        REQUIRE( storage.getEmptySegmentTotalSize() == 0 );
        

        // erase 5 components from the same segment (ids 6-10)

        for (size_t i = 6; i < 11; i++)
        {
            storage.eraseComponentByEntity(i);

            REQUIRE_FALSE( storage.hasComponent(i) );
        }
        
        REQUIRE( storage.getSize() == 15 );
        REQUIRE( storage.getCapacity() == 20 );
        REQUIRE( storage.getEmptySegmentTotalSize() == 5 );


        // erase another 5 components, but from different segments (ids 3-5 and 11-12)

        for (size_t i = 3; i < 6; i++)
        {
            SComponentIndex index = storage.getComponentIndexByEntity(i);

            REQUIRE_NOTHROW( storage.eraseComponentByIndex( index ) );

            REQUIRE_FALSE( storage.hasComponent(i) );
        }
        for (size_t i = 11; i < 13; i++)
        {
            SComponentIndex index = storage.getComponentIndexByEntity(i);

            REQUIRE_NOTHROW( storage.eraseComponentByIndex( index ) );

            REQUIRE_FALSE( storage.hasComponent(i) );
        }
        
        REQUIRE( storage.getSize() == 10 );
        REQUIRE( storage.getCapacity() == 20 );
        REQUIRE( storage.getEmptySegmentTotalSize() == 5 );


        // try erasing using invalid index

        SComponentIndex index;

        index.segmentIndex = 23386;
        index.segmentSlotIndex = 0;
        REQUIRE_THROWS( storage.eraseComponentByIndex( index ) );

        index.segmentIndex = 0;
        index.segmentSlotIndex = 12946;
        REQUIRE_THROWS( storage.eraseComponentByIndex( index ) );

        REQUIRE( storage.getSize() == 10 );
        REQUIRE( storage.getCapacity() == 20 );
        REQUIRE( storage.getEmptySegmentTotalSize() == 5 );


        // try erasing already erased components

        storage.eraseComponentByEntity(4);
        storage.eraseComponentByEntity(7);
        storage.eraseComponentByEntity(12);

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
            storage.createComponent(i);
        }
        storage.eraseComponentByEntity(1);
        storage.eraseComponentByEntity(2);
        storage.eraseComponentByEntity(3);
        storage.eraseComponentByEntity(4);

        storage.eraseComponentByEntity(7);
        storage.eraseComponentByEntity(8);
        storage.eraseComponentByEntity(9);
        storage.eraseComponentByEntity(10);

        storage.eraseComponentByEntity(11);
        storage.eraseComponentByEntity(12);
        storage.eraseComponentByEntity(13);
        storage.eraseComponentByEntity(14);
        storage.eraseComponentByEntity(15);

        // only one segment left empty
        // first 2 segments have 1 component in each of them

        storage.resize(5);

        REQUIRE( storage.getSize() == 2 );
        REQUIRE( storage.getCapacity() == 10 );
        REQUIRE( storage.getEmptySegmentTotalSize() == 0 );
    }
}