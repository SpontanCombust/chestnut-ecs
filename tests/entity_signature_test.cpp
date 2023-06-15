#include <catch2/catch.hpp>

#include "../include/chestnut/ecs/entity_signature.hpp"

using namespace chestnut::ecs;

struct Foo {};
struct Bar {};
struct Baz {};

TEST_CASE( "Entity signature test" )
{
    SECTION( "Adding types to the signature" )
    {
        CEntitySignature sign1, sign2;


        sign1.add<Foo,Bar>();
        REQUIRE( sign1.has<Foo, Bar>() );
        REQUIRE_FALSE( sign1.has<Baz>() );

        sign2.add( typeid(Foo) );
        REQUIRE( sign2.has( typeid(Foo) ) );
        REQUIRE_FALSE( sign2.has<Foo, Bar>() );
        REQUIRE_FALSE( sign2.has( typeid(Bar) ) );
        sign2.add( typeid(Bar) );
        REQUIRE( sign2.has( typeid(Foo) ) );
        REQUIRE( sign2.has( typeid(Bar) ) );
        REQUIRE( sign2.has<Foo, Bar>() );
        REQUIRE_FALSE( sign2.has<Baz>() );

        REQUIRE( sign1 == sign2 );
    }

    SECTION( "Removing types from the signature" )
    {
        CEntitySignature signature;

        signature.add<Foo,Baz>();
        signature.remove<Foo>();

        REQUIRE_FALSE( signature.has<Foo>() );
        REQUIRE( signature.has<Baz>() );

        signature.remove( typeid(Baz) );
        REQUIRE_FALSE( signature.has( typeid(Foo) ) );
        REQUIRE_FALSE( signature.has( typeid(Baz) ) );
    }

    SECTION( "Signature is empty" )
    {
        CEntitySignature signature;

        REQUIRE( signature.isEmpty() );
        
        signature.add<Foo>();
        REQUIRE_FALSE( signature.isEmpty() );
    }

    SECTION( "Clear signature" )
    {
        CEntitySignature signature;

        signature.add<Foo,Bar>();

        signature.clear();

        REQUIRE( signature.isEmpty() );
    }

    SECTION( "Signature size" )
    {
        CEntitySignature signature;

        signature.add<Foo, Bar, Bar, Baz>(); // Bar repeat
        REQUIRE( signature.getSize() == 3 );

        signature.remove<Bar>();
        REQUIRE( signature.getSize() == 2 );

        signature.clear();
        REQUIRE( signature.getSize() == 0 );
    }

    SECTION( "Signature sum" )
    {
        CEntitySignature sign1, sign2, sign3;
        CEntitySignature signSum;

        sign1.add<Foo>();

        sign2.add<Foo, Bar>();

        sign3.add<Bar, Baz>();

        signSum = sign1 + sign2;
        REQUIRE( signSum.has<Foo, Bar>() );

        signSum += sign3;
        REQUIRE( signSum.has<Foo, Bar, Baz>() );
    }

    SECTION( "Signature difference" )
    {
        CEntitySignature sign1, sign2, sign3;
        CEntitySignature signDiff;

        sign1.add<Foo, Bar>();

        sign2.add<Bar, Baz>();

        sign3.add<Foo>();

        signDiff = sign1 - sign2;
        REQUIRE( ( signDiff.has<Foo>() && !signDiff.has<Bar>() && !signDiff.has<Baz>() ) );

        signDiff -= sign3;
        REQUIRE( ( !signDiff.has<Foo>() && !signDiff.has<Bar>() && !signDiff.has<Baz>() ) );
        REQUIRE( ( signDiff.isEmpty() ) );
    }

    SECTION( "Signature being a part of other signature" )
    {
        CEntitySignature sign1, sign2;

        sign1.add( typeid(Foo) );
        sign1.add( typeid(Bar) );

        sign2.add( typeid(Foo) );
        sign2.add( typeid(Bar) );
        sign2.add( typeid(Baz) );

        REQUIRE( sign2.hasAllFrom( sign1 ) );
        REQUIRE_FALSE( sign1.hasAllFrom( sign2 ) );
    }

    SECTION("Make signature in-place")
    {
        CEntitySignature sign1, sign2;

        sign1.add(typeid(Foo));
        sign1.add(typeid(Bar));

        // sign2 is empty

        CEntitySignature sign1t1 = makeEntitySignature<Foo, Bar>();
        CEntitySignature sign2t1 = makeEntitySignature();
        REQUIRE(sign1 == sign1t1);
        REQUIRE(sign2 == sign2t1);

        CEntitySignature sign1t2 = CEntitySignature::from<Foo, Bar>();
        CEntitySignature sign2t2 = CEntitySignature::from();
        REQUIRE(sign1 == sign1t2);
        REQUIRE(sign2 == sign2t2);
    }
}