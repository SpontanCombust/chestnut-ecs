#include <catch2/catch.hpp>

#include "../include/chestnut/ecs/component_storage_segment.hpp"

using namespace chestnut::ecs;
using namespace chestnut::ecs::internal;

class Foo {
public:
    int i;
};

TEST_CASE( "Component storage segment test" )
{
    CComponentStorageSegment<Foo> seg(10);

    SECTION( "Initial segment state" )
    {
        REQUIRE( seg.getSize() == 10 );
        REQUIRE( seg.isEmpty() );
        REQUIRE_FALSE( seg.isFull() );
    }

    SECTION( "Taking up slots and checking if they're there" )
    { 
        REQUIRE( seg.getTakenSlotCount() == 0 );

        REQUIRE( seg.tryTakeUpSlot( ENTITY_ID_MINIMAL + 0 ) != nullptr );
        REQUIRE( seg.tryTakeUpSlot( ENTITY_ID_MINIMAL + 1 ) != nullptr );
        REQUIRE( seg.tryTakeUpSlot( ENTITY_ID_MINIMAL + 2 ) != nullptr );
        REQUIRE( seg.tryTakeUpSlot( ENTITY_ID_MINIMAL + 4 ) != nullptr );

        REQUIRE( seg.getTakenSlotCount() == 4 );
    
        // Check for component presence
        REQUIRE_FALSE( seg.hasSlottedComponent( ENTITY_ID_INVALID ) );
        REQUIRE( seg.hasSlottedComponent( ENTITY_ID_MINIMAL + 0 ) );
        REQUIRE( seg.hasSlottedComponent( ENTITY_ID_MINIMAL + 2 ) );
        REQUIRE_FALSE( seg.hasSlottedComponent( ENTITY_ID_MINIMAL + 3 ) );
    }

    SECTION( "Test special or illegal cases for taking up slots" )
    {
        // Fill up the segment to the full
        for (entityid i = ENTITY_ID_MINIMAL; i < ENTITY_ID_MINIMAL + 10; i++)
        {
            seg.tryTakeUpSlot(i);
        }
        REQUIRE( seg.getTakenSlotCount() == seg.getSize() );

        // Check if segment doesn't change its state if entity component is already slotted
        REQUIRE( seg.tryTakeUpSlot( ENTITY_ID_MINIMAL + 9 ) != nullptr );
        REQUIRE( seg.getTakenSlotCount() == seg.getSize() );

        // Check for situation when you try to take up in a full segment
        REQUIRE( seg.isFull() );
        REQUIRE( seg.tryTakeUpSlot( ENTITY_ID_MINIMAL + 10 ) == nullptr );   
    }

    SECTION( "Getting component" )
    {
        segsize idx;
        SComponentWrapper<Foo> *foo;

        foo = seg.tryTakeUpSlot( ENTITY_ID_MINIMAL );
        REQUIRE( foo != nullptr );
        foo->data.i = 2137;

        foo = seg.getSlottedComponent( ENTITY_ID_MINIMAL );
        REQUIRE( foo != nullptr );
        REQUIRE( foo->data.i == 2137 );        
    }

    SECTION( "Freeing slots" )
    {
        seg.tryTakeUpSlot( ENTITY_ID_MINIMAL + 0 );
        seg.tryTakeUpSlot( ENTITY_ID_MINIMAL + 1 );
        seg.tryTakeUpSlot( ENTITY_ID_MINIMAL + 2 );
        seg.tryTakeUpSlot( ENTITY_ID_MINIMAL + 3 );
        seg.tryTakeUpSlot( ENTITY_ID_MINIMAL + 4 );

        REQUIRE( seg.getTakenSlotCount() == 5 );


        seg.freeSlot( ENTITY_ID_MINIMAL + 0 );
        seg.freeSlot( ENTITY_ID_MINIMAL + 1 );

        REQUIRE( seg.getTakenSlotCount() == 3 );
        REQUIRE_FALSE( seg.hasSlottedComponent( ENTITY_ID_MINIMAL + 0 ) );
        REQUIRE_FALSE( seg.hasSlottedComponent( ENTITY_ID_MINIMAL + 1 ) );


        // Clearing all slots
        seg.clearSlots();

        REQUIRE_FALSE( seg.hasSlottedComponent( ENTITY_ID_MINIMAL + 2 ) );
        REQUIRE( seg.isEmpty() );
    }
}