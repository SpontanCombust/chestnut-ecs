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

        entityid_t id1 = registry.registerNewEntity( false );
        entityid_t id2 = registry.registerNewEntity( false, signature );

        const CEntitySignature* signaturePtr;
        REQUIRE( registry.hasEntity( id1 ) );
        REQUIRE( ( signaturePtr = registry.getEntitySignature( id1 ) ) );
        REQUIRE( signaturePtr->isEmpty() );

        REQUIRE( registry.hasEntity( id2 ) );
        REQUIRE( ( signaturePtr = registry.getEntitySignature( id2 ) ) );
        REQUIRE( ( signaturePtr->has<int>() && signaturePtr->has<char>() ) );
    }

    SECTION( "Test exception throw when fetching signature of unregistered entity" )
    {
        REQUIRE_FALSE( registry.getEntitySignature( ENTITY_ID_INVALID ) );
        REQUIRE_FALSE( registry.getEntitySignature( ENTITY_ID_MINIMAL ) ); // none were registered yet
    }

    SECTION( "Updating registry entity" )
    {
        CEntitySignature signature;
        const CEntitySignature* signaturePtr;

        entityid_t id = registry.registerNewEntity( false );

        signaturePtr = registry.getEntitySignature( id );
        REQUIRE( signaturePtr->isEmpty() ); 

        signature.add<int>();
        registry.updateEntity( id, signature );

        signaturePtr = registry.getEntitySignature( id );
        REQUIRE_FALSE( signaturePtr->isEmpty() );
        REQUIRE( signaturePtr->has<int>() );
    }

    SECTION( "Removing entities" )
    {
        entityid_t id1 = registry.registerNewEntity( false );
        entityid_t id2 = registry.registerNewEntity( false );
        entityid_t id3 = registry.registerNewEntity( false );
        entityid_t id4 = registry.registerNewEntity( false );

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
        entityid_t id1 = registry.registerNewEntity( false );
        entityid_t id2 = registry.registerNewEntity( false );
        entityid_t id3 = registry.registerNewEntity( false );
        entityid_t id4 = registry.registerNewEntity( false );

        registry.unregisterEntity( id2 );
        registry.unregisterEntity( id3 );

        REQUIRE( registry.hasEntity( id1 ) );
        REQUIRE_FALSE( registry.hasEntity( id2 ) );
        REQUIRE_FALSE( registry.hasEntity( id3 ) );
        REQUIRE( registry.hasEntity( id4 ) );


        entityid_t id5 = registry.registerNewEntity( false );
        entityid_t id6 = registry.registerNewEntity( false );

        REQUIRE( id5 == id3 );
        REQUIRE( id6 == id2 );

        REQUIRE( registry.hasEntity( id1 ) );
        REQUIRE( registry.hasEntity( id5 ) );
        REQUIRE( registry.hasEntity( id6 ) );
        REQUIRE( registry.hasEntity( id4 ) );
    }

    SECTION( "Checking for template entities" )
    {
        entityid_t id1 = registry.registerNewEntity(false);
        entityid_t id2 = registry.registerNewEntity(true);
        entityid_t id3 = registry.registerNewEntity(false);
        entityid_t id4 = registry.registerNewEntity(false);
        entityid_t id5 = registry.registerNewEntity(true);


        REQUIRE( registry.hasEntity( id1, CEntityRegistry::CAN_BE_REGULAR_ENTITY ) );
        REQUIRE_FALSE( registry.hasEntity( id1, CEntityRegistry::CAN_BE_TEMPLATE_ENTITY ) );
        REQUIRE( registry.hasEntity( id1, CEntityRegistry::CAN_BE_REGULAR_ENTITY | CEntityRegistry::CAN_BE_TEMPLATE_ENTITY ) );

        REQUIRE_FALSE( registry.hasEntity( id2, CEntityRegistry::CAN_BE_REGULAR_ENTITY ) );
        REQUIRE( registry.hasEntity( id2, CEntityRegistry::CAN_BE_TEMPLATE_ENTITY ) );
        REQUIRE( registry.hasEntity( id2, CEntityRegistry::CAN_BE_REGULAR_ENTITY | CEntityRegistry::CAN_BE_TEMPLATE_ENTITY ) );

        REQUIRE( registry.hasEntity( id4, CEntityRegistry::CAN_BE_REGULAR_ENTITY ) );
        REQUIRE_FALSE( registry.hasEntity( id4, CEntityRegistry::CAN_BE_TEMPLATE_ENTITY ) );
        REQUIRE( registry.hasEntity( id4, CEntityRegistry::CAN_BE_REGULAR_ENTITY | CEntityRegistry::CAN_BE_TEMPLATE_ENTITY ) );

        REQUIRE( registry.getEntityCount() == 5 );

        registry.unregisterEntity( id2 );
        REQUIRE_FALSE( registry.hasEntity( id2, CEntityRegistry::CAN_BE_REGULAR_ENTITY | CEntityRegistry::CAN_BE_TEMPLATE_ENTITY ) );
    }
}