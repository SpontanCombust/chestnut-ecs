#include <catch2/catch.hpp>

#include "../include/chestnut/ecs/component_storage_segment.hpp"

using namespace chestnut::ecs;

class Foo : public CComponent {
public:
    int i;
};

TEST_CASE( "Component storage segment test" )
{
    SECTION( "Assert if segment accepts only CComponent derived types" )
    {
        // v Uncomment before testing
        // CComponentStorageSegment<int> seg(0,10);

        SUCCEED("CComponentStorageSegment didn't accept int as its type.");
    }


    CComponentStorageSegment<Foo> seg(0,10);

    SECTION( "Initial segment state" )
    {
        REQUIRE( seg.getID() == 0 );
        REQUIRE( seg.getSize() == 10 );
        REQUIRE( seg.isEmpty() );
        REQUIRE_FALSE( seg.isFull() );
    }

    SECTION( "Taking up slots and checking" )
    {
        // Fill up the segment, additionally make sure it won't throw an exception
        // have to start from 1 as 0 is for an invalid entity
        for (entityid i = 1; i < 11; i++)
        {
            REQUIRE_NOTHROW( seg.tryTakeUpSlot(i) );
            REQUIRE( seg.getTakenSlotCount() == i );
        }

        WARN( seg.toString() );
    
        // Check for component presence
        REQUIRE_FALSE( seg.hasSlottedComponent(0) );
        REQUIRE( seg.hasSlottedComponent(1) );
        REQUIRE( seg.hasSlottedComponent(10) );
        REQUIRE_FALSE( seg.hasSlottedComponent(11) );
        
        // Check if segment doesn't change its state if entity component is already slotted
        REQUIRE_NOTHROW( seg.tryTakeUpSlot(10) );

        // Check for exeption throwing of a full segment
        REQUIRE( seg.getTakenSlotCount() == seg.getSize() );
        REQUIRE( seg.isFull() );

        REQUIRE_THROWS( seg.tryTakeUpSlot(12) );
    }

    SECTION( "Getting components through different methods and modifying them" )
    {
        segsize idx1, idx2;
        Foo *foo;

        idx1 = seg.tryTakeUpSlot(1);
        
        // Component is properly slotted and returned with byEntity method
        foo = seg.getComponentByEntity(1);
        REQUIRE( foo != nullptr );
        REQUIRE( foo->owner == 1 );
        foo->i = 2137;

        // Indices returned at slotting and getting stages are the same
        idx2 = seg.getIndexByEntity(1);
        REQUIRE( idx1 == idx2 );

        // Get component byIndex, check if it's the same component as before
        REQUIRE_NOTHROW( foo = seg.getComponentByIndex( idx2 ) );
        REQUIRE( foo->i == 2137 );

        // Get component using brackets operator, check if it's the same component as before
        REQUIRE_NOTHROW( foo = seg[ idx2 ] );
        REQUIRE( foo->i == 2137 );

        // range check for index that exceeds range
        REQUIRE_THROWS( seg.getComponentByIndex(11) );
        // range check for index for nonexisting entity
        REQUIRE_THROWS( seg.getIndexByEntity(2) );
    }

    SECTION( "Erasing components/freeing slots" )
    {
        seg.tryTakeUpSlot(0);
        seg.tryTakeUpSlot(1);
        segsize idx1 = seg.tryTakeUpSlot(2);
        seg.tryTakeUpSlot(3);
        segsize idx2 = seg.tryTakeUpSlot(4);


        // Freeing slot by entity
        REQUIRE( seg.getTakenSlotCount() == 5 );

        seg.freeSlotByEntity(1);
        seg.freeSlotByEntity(2);

        REQUIRE( seg.getTakenSlotCount() == 3 );
        REQUIRE_FALSE( seg.hasSlottedComponent(1) );
        REQUIRE_FALSE( seg.hasSlottedComponent(2) );


        // Freeing slot by index
        // this one has already been freed so should not affect the segment
        seg.freeSlotByIndex(idx1);
        REQUIRE( seg.getTakenSlotCount() == 3 );

        seg.freeSlotByIndex(idx2);
        REQUIRE( seg.getTakenSlotCount() == 2 );
        REQUIRE_FALSE( seg.hasSlottedComponent(4) );

        REQUIRE_THROWS( seg.freeSlotByIndex(10) );


        // Clearing all slots
        seg.clearSlots();

        REQUIRE_FALSE( seg.hasSlottedComponent(3) );
        REQUIRE( seg.isEmpty() );
    }
}