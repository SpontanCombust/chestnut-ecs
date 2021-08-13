#include <catch2/catch.hpp>

#include "../include/chestnut/ecs/component_storage_segment.hpp"

using namespace chestnut::ecs;
using namespace chestnut::ecs::internal;

class Foo : public CComponent {
public:
    int i;
};

TEST_CASE( "Component storage segment test" )
{
    // v Uncomment before testing
    /*
    SECTION( "Compile time assert if segment accepts only valid classes" )
    {
        class Bar
        {
        public:
            long j;
        };

        class Baz : public CComponent
        {
        public:
            short k;

            Baz( short _k ) { _k = k; }
        };

        // test if doesn't compile non CComponent inherited class
        CComponentStorageSegment<Bar> seg1(10);

        // test if doesn't compile non default constructible class
        CComponentStorageSegment<Baz> seg2(20);
    }
    */


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
    
        // Check for component presence
        REQUIRE_FALSE( seg.hasSlottedComponent(0) );
        REQUIRE( seg.hasSlottedComponent(1) );
        REQUIRE( seg.hasSlottedComponent(3) );
        REQUIRE_FALSE( seg.hasSlottedComponent(4) );
    }

    // uncomment when wanting to test
    // SECTION( "Test assert on invalid entity" )
    // {
    //     seg.tryTakeUpSlot( ENTITY_ID_INVALID );
    // }

    SECTION( "Test special or illegal cases for taking up slots" )
    {
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

    SECTION( "Getting component" )
    {
        segsize idx;
        Foo *foo;

        foo = seg.tryTakeUpSlot(1);
        REQUIRE( foo != nullptr );
        REQUIRE( foo->owner == 1 );
        foo->i = 2137;

        foo = seg.getSlottedComponent(1);
        REQUIRE( foo != nullptr );
        REQUIRE( foo->owner == 1 );
        REQUIRE( foo->i == 2137 );        
    }

    SECTION( "Freeing slots" )
    {
        seg.tryTakeUpSlot(1);
        seg.tryTakeUpSlot(2);
        seg.tryTakeUpSlot(3);
        seg.tryTakeUpSlot(4);
        seg.tryTakeUpSlot(5);

        REQUIRE( seg.getTakenSlotCount() == 5 );


        seg.freeSlot(1);
        seg.freeSlot(2);

        REQUIRE( seg.getTakenSlotCount() == 3 );
        REQUIRE_FALSE( seg.hasSlottedComponent(1) );
        REQUIRE_FALSE( seg.hasSlottedComponent(2) );


        // Clearing all slots
        seg.clearSlots();

        REQUIRE_FALSE( seg.hasSlottedComponent(3) );
        REQUIRE( seg.isEmpty() );
    }

    // SECTION()
}