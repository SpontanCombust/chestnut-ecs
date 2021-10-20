#include <catch2/catch.hpp>

#include "../include/chestnut/ecs/entity_registry.hpp"

using namespace chestnut::ecs;
using namespace chestnut::ecs::internal;

TEST_CASE( "Entity registry test" )
{
    CEntityRegistry registry;

    SECTION( "Simple registering and checking" )
    {
        //! New (not recycled) entity IDs should be distributed in linear growing fashion, starting from ENTITY_ID_MINIMAL
        //! Registry should recycle IDs starting from the latest unregistered entity ID

        REQUIRE( registry.getEntityCount() == 0 );

        registry.registerNewEntity(false);
        registry.registerNewEntity(false);
        registry.registerNewEntity(false);
        registry.registerNewEntity(false);

        REQUIRE( registry.getEntityCount() == 4 );

        REQUIRE_FALSE( registry.hasEntity( ENTITY_ID_INVALID ) );
        REQUIRE( registry.hasEntity( ENTITY_ID_MINIMAL + 0) );
        REQUIRE( registry.hasEntity( ENTITY_ID_MINIMAL + 1) );
        REQUIRE( registry.hasEntity( ENTITY_ID_MINIMAL + 2) );
        REQUIRE( registry.hasEntity( ENTITY_ID_MINIMAL + 3) );
        REQUIRE_FALSE( registry.hasEntity( ENTITY_ID_MINIMAL + 4) );
    }

    SECTION( "Registering with signature and checking the signature" )
    {
        CEntitySignature signature;
        signature.add<int>();
        signature.add<char>();

        entityid id1 = registry.registerNewEntity( false );
        entityid id2 = registry.registerNewEntity( false, signature );

        REQUIRE( registry.hasEntity( id1 ) );
        REQUIRE_NOTHROW( signature = registry.getEntitySignature( id1 ) );
        REQUIRE( signature.isEmpty() );

        REQUIRE( registry.hasEntity( id2 ) );
        REQUIRE_NOTHROW( signature = registry.getEntitySignature( id2 ) );
        REQUIRE( ( signature.has<int>() && signature.has<char>() ) );
    }

    SECTION( "Test exception throw when fetching signature of unregistered entity" )
    {
        REQUIRE_THROWS( registry.getEntitySignature( ENTITY_ID_INVALID ) );
        REQUIRE_THROWS( registry.getEntitySignature( ENTITY_ID_MINIMAL ) ); // none were registered yet
    }

    SECTION( "Updating registry entity" )
    {
        CEntitySignature signature;

        entityid id = registry.registerNewEntity( false );

        signature = registry.getEntitySignature( id );
        REQUIRE( signature.isEmpty() ); 

        signature.add<int>();
        registry.updateEntity( id, signature );

        signature = registry.getEntitySignature( id );
        REQUIRE_FALSE( signature.isEmpty() );
        REQUIRE( signature.has<int>() );
    }

    SECTION( "Removing entities" )
    {
        entityid id1 = registry.registerNewEntity( false );
        entityid id2 = registry.registerNewEntity( false );
        entityid id3 = registry.registerNewEntity( false );
        entityid id4 = registry.registerNewEntity( false );

        REQUIRE( registry.hasEntity(id1) );
        REQUIRE( registry.hasEntity(id2) );
        REQUIRE( registry.hasEntity(id3) );
        REQUIRE( registry.hasEntity(id4) );
        
        registry.unregisterEntity(id2);
        registry.unregisterEntity(id3);
        registry.unregisterEntity(id4);

        REQUIRE( registry.hasEntity(id1) );
        REQUIRE_FALSE( registry.hasEntity(id2) );
        REQUIRE_FALSE( registry.hasEntity(id3) );
        REQUIRE_FALSE( registry.hasEntity(id4) );
    }

    SECTION( "Entity ID recycling" )
    {
        entityid id1 = registry.registerNewEntity( false );
        entityid id2 = registry.registerNewEntity( false );
        entityid id3 = registry.registerNewEntity( false );
        entityid id4 = registry.registerNewEntity( false );

        registry.unregisterEntity( id2 );
        registry.unregisterEntity( id3 );

        REQUIRE( registry.hasEntity( id1 ) );
        REQUIRE_FALSE( registry.hasEntity( id2 ) );
        REQUIRE_FALSE( registry.hasEntity( id3 ) );
        REQUIRE( registry.hasEntity( id4 ) );


        entityid id5 = registry.registerNewEntity( false );
        entityid id6 = registry.registerNewEntity( false );

        REQUIRE( id5 == id3 );
        REQUIRE( id6 == id2 );

        REQUIRE( registry.hasEntity( id1 ) );
        REQUIRE( registry.hasEntity( id5 ) );
        REQUIRE( registry.hasEntity( id6 ) );
        REQUIRE( registry.hasEntity( id4 ) );
    }

    SECTION( "Checking for template entities" )
    {
        entityid id1 = registry.registerNewEntity(false);
        entityid id2 = registry.registerNewEntity(true);
        entityid id3 = registry.registerNewEntity(false);
        entityid id4 = registry.registerNewEntity(false);
        entityid id5 = registry.registerNewEntity(true);


        REQUIRE( registry.hasEntity( id1, true ) );
        REQUIRE( registry.hasEntity( id1, false ) );
        REQUIRE_FALSE( registry.hasTemplateEntity( id1 ) );

        REQUIRE( registry.hasEntity( id2, true ) );
        REQUIRE_FALSE( registry.hasEntity( id2, false ) );
        REQUIRE( registry.hasTemplateEntity( id2 ) );

        REQUIRE( registry.hasEntity( id4, true ) );
        REQUIRE( registry.hasEntity( id4, false ) );
        REQUIRE_FALSE( registry.hasTemplateEntity( id4 ) );

        REQUIRE( registry.hasEntity( id5, true ) );
        REQUIRE_FALSE( registry.hasEntity( id5, false ) );
        REQUIRE( registry.hasTemplateEntity( id5 ) );


        REQUIRE( registry.getEntityCount() == 5 );


        registry.unregisterEntity( id2 );
        REQUIRE_FALSE( registry.hasEntity( id2, true ) );
        REQUIRE_FALSE( registry.hasEntity( id2, false ) );
    }
}