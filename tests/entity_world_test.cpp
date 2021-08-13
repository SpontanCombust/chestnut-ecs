#include <catch2/catch.hpp>

#include "../include/chestnut/ecs/entity_world.hpp"

using namespace chestnut::ecs;

class Foo : public CComponent
{
public:
    int x;
};

class Bar : public CComponent
{
public:
    long y;
};

class Baz : public CComponent
{
public:
    char z;
    short w;
};


TEST_CASE( "Entity world test - general" )
{
    CEntityWorld world;

    SECTION( "Creating and checking empty entities" )
    {
        entityid e1, e2, e3;

        e1 = world.createEntity();
        e2 = world.createEntity();
        e3 = world.createEntity();

        REQUIRE_FALSE( world.hasEntity( ENTITY_ID_INVALID ) );
        REQUIRE( world.hasEntity(e1) );
        REQUIRE( world.hasEntity(e2) );
        REQUIRE( world.hasEntity(e3) );
    }

    SECTION( "Destroying empty entities" )
    {
        entityid e1, e2, e3;

        e1 = world.createEntity();
        e2 = world.createEntity();
        e3 = world.createEntity();

        REQUIRE( world.hasEntity(e1) );
        REQUIRE( world.hasEntity(e2) );
        REQUIRE( world.hasEntity(e3) );

        world.destroyEntity( e2 );

        REQUIRE( world.hasEntity(e1) );
        REQUIRE_FALSE( world.hasEntity(e2) );
        REQUIRE( world.hasEntity(e3) );
    }

    SECTION( "Destroying multiple empty entities at once" )
    {
        std::vector< entityid > ids1, ids2;

        ids1 = world.createEntities(10);
        ids2 = world.createEntities(10);
        

        world.destroyEntities( ids2 );

        for( entityid id : ids1 )
        {
            REQUIRE( world.hasEntity( id ) );
        }
        for( entityid id : ids2 )
        {
            REQUIRE_FALSE( world.hasEntity( id ) );
        }
    }

    SECTION( "ID recycling" )
    {
        entityid ent1, ent2, ent3, ent4;

        ent1 = world.createEntity();
        ent2 = world.createEntity();
        
        world.destroyEntity( ent1 );
        world.destroyEntity( ent2 );

        ent3 = world.createEntity();
        ent4 = world.createEntity();

        REQUIRE( ent3 == ent2 );
        REQUIRE( ent4 == ent1 );
    }


    SECTION( "Creating components" )
    {
        // try create component for nonexistant entity
        REQUIRE_FALSE( world.createComponent<Foo>(1) );


        entityid ent = world.createEntity();
        Foo *foo;

        foo = world.createComponent<Foo>( ent );
        REQUIRE( foo->owner == ent );

        foo->x = 1;

        // check if returns already created component on repeated create
        foo = world.createComponent<Foo>( ent );
        REQUIRE( foo->x == 1 );

        REQUIRE( world.hasComponent<Foo>( ent ) );
    }

    SECTION( "Getting components" )
    {
        entityid ent1 = world.createEntity();
        entityid ent2 = world.createEntity();
        entityid ent3 = world.createEntity();

        Foo *foo;
        Bar *bar;
        Baz *baz;


        foo = world.createComponent<Foo>( ent1 );
        foo->x = 1;

        foo = world.createComponent<Foo>( ent2 );
        foo->x = 2;
        baz = world.createComponent<Baz>( ent2 );
        baz->z = 3;
        baz->w = 4;

        bar = world.createComponent<Bar>( ent3 );
        bar->y = 5;
        baz = world.createComponent<Baz>( ent3 );
        baz->z = 6;
        baz->w = 7;


        REQUIRE( world.hasComponent<Foo>( ent1 ) );
        REQUIRE_FALSE( world.hasComponent<Bar>( ent1 ) );
        REQUIRE_FALSE( world.hasComponent<Baz>( ent1 ) );

        REQUIRE( world.hasComponent<Foo>( ent2 ) );
        REQUIRE_FALSE( world.hasComponent<Bar>( ent2 ) );
        REQUIRE( world.hasComponent<Baz>( ent2 ) );

        REQUIRE_FALSE( world.hasComponent<Foo>( ent3 ) );
        REQUIRE( world.hasComponent<Bar>( ent3 ) );
        REQUIRE( world.hasComponent<Baz>( ent3 ) );


        foo = world.getComponent<Foo>( ent1 );
        REQUIRE( foo->x == 1 );
        bar = world.getComponent<Bar>( ent1 );
        REQUIRE_FALSE( bar );
        baz = world.getComponent<Baz>( ent1 );

        foo = world.getComponent<Foo>( ent2 );
        REQUIRE( foo->x == 2 );
        bar = world.getComponent<Bar>( ent2 );
        REQUIRE_FALSE( bar );
        baz = world.getComponent<Baz>( ent2 );
        REQUIRE( baz->z == 3 );
        REQUIRE( baz->w == 4 );

        foo = world.getComponent<Foo>( ent3 );
        REQUIRE_FALSE( foo );
        bar = world.getComponent<Bar>( ent3 );
        REQUIRE( bar->y == 5 );
        baz = world.getComponent<Baz>( ent3 );
        REQUIRE( baz->z == 6 );
        REQUIRE( baz->w == 7 );
    }

    SECTION( "Destroying components" )
    {
        entityid ent1, ent2;

        ent1 = world.createEntity();
        ent2 = world.createEntity();


        world.createComponent<Foo>( ent1 );
        world.createComponent<Bar>( ent1 );

        world.createComponent<Bar>( ent2 );
        world.createComponent<Baz>( ent2 );


        world.destroyComponent<Bar>( ent1 );
        REQUIRE_FALSE( world.hasComponent<Bar>( ent1 ) );
        REQUIRE( world.hasComponent<Foo>( ent1 ) );

        world.destroyComponent<Bar>( ent2 );
        world.destroyComponent<Baz>( ent2 );
        REQUIRE_FALSE( world.hasComponent<Bar>( ent2 ) );
        REQUIRE_FALSE( world.hasComponent<Baz>( ent2 ) );
    }

    SECTION( "Destroying entities with components" )
    {
        entityid ent = world.createEntity();

        world.createComponent<Foo>( ent );
        world.createComponent<Bar>( ent );
        world.createComponent<Baz>( ent );

        world.destroyEntity( ent );


        REQUIRE_FALSE( world.hasComponent<Foo>( ent ) );
        REQUIRE_FALSE( world.hasComponent<Bar>( ent ) );
        REQUIRE_FALSE( world.hasComponent<Baz>( ent ) );
        REQUIRE_FALSE( world.hasEntity( ent ) );
    }

}







TEST_CASE( "Entity world test - batching" )
{
    CEntityWorld world;

    // 10 entities with Foo
    for (size_t i = 0; i < 10; i++)
    {
        entityid ent = world.createEntity();
        world.createComponent<Foo>( ent )->x = ent;
    }
    // 10 with Foo and Bar
    for (size_t i = 0; i < 10; i++)
    {
        entityid ent = world.createEntity();
        world.createComponent<Foo>( ent )->x = ent;
        world.createComponent<Bar>( ent )->y = ent + 1;
    }
    // 10 with Bar and Baz
    for (size_t i = 0; i < 10; i++)
    {
        entityid ent = world.createEntity();
        world.createComponent<Bar>( ent )->y = ent;
        world.createComponent<Baz>( ent )->z = ent + 1;
    }
    // 10 with Foo and Baz
    for (size_t i = 0; i < 10; i++)
    {
        entityid ent = world.createEntity();
        world.createComponent<Foo>( ent )->x = ent;
        world.createComponent<Baz>( ent )->z = ent + 1;
    }
    // 10 with Foo, Bar and Baz
    for (size_t i = 0; i < 10; i++)
    {
        entityid ent = world.createEntity();
        world.createComponent<Foo>( ent )->x = ent;
        world.createComponent<Bar>( ent )->y = ent + 1;
        world.createComponent<Baz>( ent )->z = ent + 2;
    }
    

    SEntityQuery query;

    SECTION( "Query entities with Foo" )
    {
        query.entitySignCond = []( const CEntitySignature& sign ) -> bool
        {
            return sign.has<Foo>();
        };

        REQUIRE( world.queryEntities( query ) == 4 );

        REQUIRE_NOTHROW(
            forEachEntityInQuery<Foo>( query, 
            []( Foo& foo ) -> void
            {
                foo.x *= foo.x;
                REQUIRE( foo.x == foo.owner * foo.owner );
            })
        );
    }

    SECTION( "Doing incorrect forEach on query" )
    {
        query.entitySignCond = []( const CEntitySignature& sign ) -> bool
        {
            return sign.has<Baz>() && ( sign.has<Foo>() || sign.has<Bar>() );
        };

        REQUIRE( world.queryEntities( query ) == 3 );

        REQUIRE_THROWS( 
            forEachEntityInQuery<Foo, Bar>( query,
            []( Foo *foo, Bar *bar ) 
            {
                foo->x = bar->y * bar->y;
            })
        );
    }

    SECTION( "Query entities with Foo, Bar and Baz" )
    {
        query.entitySignCond = []( const CEntitySignature& sign ) -> bool
        {
            return sign.has<Foo>() && sign.has<Bar>() && sign.has<Baz>();
        };

        REQUIRE( world.queryEntities( query ) == 1 );

        REQUIRE_NOTHROW(
            forEachEntityInQuery<Foo, Bar, Baz>( query, 
            []( Foo *foo, Bar *bar, Baz *baz ) 
            {
                baz->w *= foo->x + bar->y;
            })
        );
    }

    SECTION( "Query entities with Baz but no Foo" )
    {
        query.entitySignCond = []( const CEntitySignature& sign ) -> bool
        {
            return sign.has<Baz>() && !sign.has<Foo>();
        };

        REQUIRE( world.queryEntities( query ) == 1 );

        REQUIRE_NOTHROW( 
            forEachEntityInQuery<Baz>( query,
            []( Baz& baz )
            {
                baz.w *= baz.z;
            })
        );
    }

    SECTION( "Query non-existing entities" )
    {
        query.entitySignCond = []( const CEntitySignature& sign ) -> bool
        {
            return sign.has<Bar>() && !sign.has<Foo>() && !sign.has<Baz>();
        };

        REQUIRE( world.queryEntities( query ) == 0 );
    }
}







TEST_CASE( "Entity world test - entity templates" )
{
    CEntityWorld world;

    SECTION( "Simple create-has-destroy check" )
    {
        entityid ent1 = world.createTemplateEntity();
        entityid ent2 = world.createTemplateEntity();
        entityid ent3 = world.createTemplateEntity();
        entityid ent4 = world.createTemplateEntity();
        entityid ent5 = world.createTemplateEntity();

        REQUIRE( world.hasTemplateEntity( ent1 ) );
        REQUIRE( world.hasTemplateEntity( ent2 ) );
        REQUIRE( world.hasTemplateEntity( ent3 ) );
        REQUIRE( world.hasTemplateEntity( ent4 ) );
        REQUIRE( world.hasTemplateEntity( ent5 ) );

        world.destroyTemplateEntity( ent1 );
        world.destroyTemplateEntity( ent2 );
        world.destroyTemplateEntity( ent3 );
        world.destroyTemplateEntity( ent4 );
        world.destroyTemplateEntity( ent5 );

        REQUIRE_FALSE( world.hasTemplateEntity( ent1 ) );
        REQUIRE_FALSE( world.hasTemplateEntity( ent2 ) );
        REQUIRE_FALSE( world.hasTemplateEntity( ent3 ) );
        REQUIRE_FALSE( world.hasTemplateEntity( ent4 ) );
        REQUIRE_FALSE( world.hasTemplateEntity( ent5 ) );
    }

    SECTION( "Destinguishing between regular and template entity" )
    {
        entityid regularEnt = world.createEntity();
        entityid templateEnt = world.createTemplateEntity();

        REQUIRE( world.hasEntity( regularEnt ) );
        REQUIRE_FALSE( world.hasTemplateEntity( regularEnt ) );

        REQUIRE_FALSE( world.hasEntity( templateEnt ) );
        REQUIRE( world.hasTemplateEntity( templateEnt ) );
    }

    SECTION( "Applying components to template entity" )
    {
        entityid templEnt = world.createTemplateEntity();

        Foo *foo = world.createComponent<Foo>( templEnt );
        REQUIRE( foo );
        foo->x = 2;
        Bar *bar = world.createComponent<Bar>( templEnt );
        REQUIRE( bar );
        bar->y = 3;

        REQUIRE( world.hasComponent<Foo>( templEnt ) );
        REQUIRE( world.hasComponent<Bar>( templEnt ) );
        REQUIRE_FALSE( world.hasComponent<Baz>( templEnt ) );


        foo = world.getComponent<Foo>( templEnt );
        REQUIRE( foo );
        REQUIRE( foo->x == 2 );
        bar = world.getComponent<Bar>( templEnt );
        REQUIRE( bar );
        REQUIRE( bar->y == 3 );


        world.destroyComponent<Foo>( templEnt );
        REQUIRE_FALSE( world.hasComponent<Foo>( templEnt ) );
        REQUIRE( world.hasComponent<Bar>( templEnt ) );
    }

    SECTION( "Creating entities using templates" )
    {
        entityid templEnt1 = world.createTemplateEntity();
        entityid templEnt2 = world.createTemplateEntity();
        entityid regularEnt = world.createEntity();

        Foo *foo; Bar *bar; Baz *baz;

        foo = world.createComponent<Foo>( templEnt1 );
        foo->x = 1;
        bar = world.createComponent<Bar>( templEnt1 );
        bar->y = 2;

        bar = world.createComponent<Bar>( templEnt2 );
        bar->y = 3;
        baz = world.createComponent<Baz>( templEnt2 );
        baz->z = 4;


        // try creating copy with non-template entity
        entityid entFromTemplateInvalid = world.createEntityFromTemplate( regularEnt );
        REQUIRE_FALSE( world.hasEntity( entFromTemplateInvalid ) );

        
        entityid ent1 = world.createEntityFromTemplate( templEnt1 );
        entityid ent2 = world.createEntityFromTemplate( templEnt2 );



        REQUIRE( world.hasEntity( ent1 ) );

        REQUIRE( world.hasComponent<Foo>( ent1 ) );
        foo = world.getComponent<Foo>( ent1 );
        REQUIRE( foo->x == 1 );

        REQUIRE( world.hasComponent<Bar>( ent1 ) );
        bar = world.getComponent<Bar>( ent1 );
        REQUIRE( bar->y == 2 );


        REQUIRE( world.hasEntity( ent2 ) );

        REQUIRE( world.hasComponent<Bar>( ent2 ) );
        bar = world.getComponent<Bar>( ent2 );
        REQUIRE( bar->y == 3 );

        REQUIRE( world.hasComponent<Baz>( ent2 ) );
        baz = world.getComponent<Baz>( ent2 );
        REQUIRE( baz->z == 4 );
    }

    SECTION( "Creating multiple entities using a template and querying" )
    {
        entityid templ1 = world.createTemplateEntity();
        entityid templ2 = world.createTemplateEntity();

        Foo *foo; Bar *bar; Baz *baz;

        foo = world.createComponent<Foo>( templ1 );
        foo->x = 1;
        bar = world.createComponent<Bar>( templ1 );
        bar->y = 2;

        foo = world.createComponent<Foo>( templ2 );
        foo->x = 3;
        baz = world.createComponent<Baz>( templ2 );
        baz->z = 4;


        auto vecEnt1 = world.createEntitiesFromTemplate( templ1, 10 );
        REQUIRE( vecEnt1.size() == 10 );
        auto vecEnt2 = world.createEntitiesFromTemplate( templ2, 20 );
        REQUIRE( vecEnt2.size() == 20 );



        SEntityQuery query1;
        query1.entitySignCond = []( const CEntitySignature& signature ) -> bool
        {
            return signature.has<Foo>();
        };

        REQUIRE( world.queryEntities( query1 ) == 2 );
        
        entityid entCount = 0;
        for( const SComponentBatch *batch : query1.vecBatches )
        {
            entCount += batch->vecEntityIDs.size();
        }

        REQUIRE( entCount == 30 );



        SEntityQuery query2;
        query2.entitySignCond = []( const CEntitySignature& signature ) -> bool
        {
            return signature.has<Foo>() && signature.has<Bar>(); 
        };

        REQUIRE( world.queryEntities( query2 ) == 1 );
        
        entCount = 0;
        for( const SComponentBatch *batch : query2.vecBatches )
        {
            entCount += batch->vecEntityIDs.size();
        }

        REQUIRE( entCount == 10 );
    }
}