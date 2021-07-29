#include <catch2/catch.hpp>

#include "../include/chestnut/ecs/component_storage_segment.hpp"

using namespace chestnut;
using namespace chestnut::internal;

class Foo : public CComponent {
public:
    int i;
};

TEST_CASE( "Component storage segment test" )
{
    SECTION( "Assert if segment accepts only CComponent derived types" )
    {
        // v Uncomment before testing
        // CComponentStorageSegment<int> seg(10);

        SUCCEED("CComponentStorageSegment didn't accept int as its type.");
    }


    CComponentStorageSegment<Foo> seg(10);

    SECTION( "Initial segment state" )
    {
        REQUIRE( seg.getSize() == 10 );
        REQUIRE( seg.isEmpty() );
        REQUIRE_FALSE( seg.isFull() );
    }

    //!WARNING remember about 0 being used for ENTITY_ID_INVALID

    SECTION( "Taking up slots and checking if they're there" )
    { 
        REQUIRE( seg.getTakenSlotCount() == 0 );

        REQUIRE( seg.tryTakeUpSlot(1) != nullptr );
        REQUIRE( seg.tryTakeUpSlot(2) != nullptr );
        REQUIRE( seg.tryTakeUpSlot(3) != nullptr );
        REQUIRE( seg.tryTakeUpSlot(5) != nullptr );

        REQUIRE( seg.getTakenSlotCount() == 4 );

        WARN( seg.toString() );
    
        // Check for component presence
        REQUIRE_FALSE( seg.hasSlottedComponent(0) );
        REQUIRE( seg.hasSlottedComponent(1) );
        REQUIRE( seg.hasSlottedComponent(3) );
        REQUIRE_FALSE( seg.hasSlottedComponent(4) );
    }

    SECTION( "Test special or illegal cases for taking up slots" )
    {
        // Try passing ENTITY_ID_INVALID
        REQUIRE( seg.tryTakeUpSlot( ENTITY_ID_INVALID ) == nullptr );

        // Fill up the segment to the full
        for (entityid i = 1; i < 11; i++)
        {
            seg.tryTakeUpSlot(i);
        }
        REQUIRE( seg.getTakenSlotCount() == seg.getSize() );

        // Check if segment doesn't change its state if entity component is already slotted
        REQUIRE( seg.tryTakeUpSlot(10) != nullptr );
        REQUIRE( seg.getTakenSlotCount() == seg.getSize() );

        // Check for situation when you try to take up in a full segment
        REQUIRE( seg.isFull() );
        REQUIRE( seg.tryTakeUpSlot(12) == nullptr );   
    }

    SECTION( "Getting component by different means" )
    {
        segsize idx;
        Foo *foo;

        foo = seg.tryTakeUpSlot(1);
        REQUIRE( foo != nullptr );
        REQUIRE( foo->owner == 1 );
        foo->i = 2137;


        SECTION( "...by entity" )
        {
            foo = seg.getComponentByEntity(1);
            REQUIRE( foo != nullptr );
            REQUIRE( foo->owner == 1 );
            REQUIRE( foo->i == 2137 );
        }

        SECTION( "...by index" )
        {
            idx = seg.getIndexByEntity(1);
            REQUIRE_NOTHROW( foo = seg.getComponentByIndex( idx ) );
            REQUIRE( foo != nullptr );
            REQUIRE( foo->owner == 1 );
            REQUIRE( foo->i == 2137 );

            // Get component using brackets operator, check if it's the same component as before
            REQUIRE_NOTHROW( foo = seg[ idx ] );
            REQUIRE( foo->i == 2137 );
        }

        SECTION( "exceptions" )
        {
            // range check for index that exceeds range
            REQUIRE_THROWS( seg.getComponentByIndex(10) );
            // range check for index for nonexisting entity
            REQUIRE_THROWS( seg.getIndexByEntity(2) );
        }
    }

    SECTION( "Freeing slots" )
    {
        seg.tryTakeUpSlot(1);
        seg.tryTakeUpSlot(2);
        seg.tryTakeUpSlot(3); segsize idx1 = seg.getIndexByEntity(3);
        seg.tryTakeUpSlot(4);
        seg.tryTakeUpSlot(5); segsize idx2 = seg.getIndexByEntity(5);

        REQUIRE( seg.getTakenSlotCount() == 5 );


        SECTION( "...by entity" )
        {
            seg.freeSlotByEntity(1);
            seg.freeSlotByEntity(2);

            REQUIRE( seg.getTakenSlotCount() == 3 );
            REQUIRE_FALSE( seg.hasSlottedComponent(1) );
            REQUIRE_FALSE( seg.hasSlottedComponent(2) );
        }

        SECTION( "...by index" )
        {
            seg.freeSlotByIndex(idx1);
            REQUIRE( seg.getTakenSlotCount() == 4 );
            REQUIRE_FALSE( seg.hasSlottedComponent(3) );


            // testing for already freed slot
            seg.freeSlotByEntity(5);
            REQUIRE( seg.getTakenSlotCount() == 3 );
            REQUIRE_FALSE( seg.hasSlottedComponent(3) );

            seg.freeSlotByIndex(idx2);
            REQUIRE( seg.getTakenSlotCount() == 3 );
            REQUIRE_FALSE( seg.hasSlottedComponent(3) );


            // testing exception for exceeding index range
            REQUIRE_THROWS( seg.freeSlotByIndex(10) );
        }

        SECTION( "clearing all" )
        {
            // Clearing all slots
            seg.clearSlots();

            REQUIRE_FALSE( seg.hasSlottedComponent(3) );
            REQUIRE( seg.isEmpty() );
        }
    }

    // SECTION()
}