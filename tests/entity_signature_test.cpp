#include <catch2/catch.hpp>

#include "../include/chestnut/ecs/entity_signature.hpp"

struct Foo {};
struct Bar {};
struct Baz {};

TEST_CASE( "Entity signature test" )
{
    using namespace chestnut::ecs;

    SECTION( "Adding types to the signature" )
    {
        CEntitySignature singature;
        singature.add<Foo>();
        singature.add<Bar>();

        REQUIRE( singature.has<Foo>() );
        REQUIRE( singature.has<Bar>() );
        REQUIRE( !singature.has<Baz>() );
    }

    SECTION( "Removing types from the signature" )
    {
        CEntitySignature signature;

        signature.add<Foo>();
        signature.add<Baz>();

        signature.remove<Foo>();

        REQUIRE( !signature.has<Foo>() );
        REQUIRE( signature.has<Baz>() );
    }

    SECTION( "Signature is empty" )
    {
        CEntitySignature signature;

        REQUIRE( signature.isEmpty() );
        
        signature.add<Foo>();
        REQUIRE( !signature.isEmpty() );
    }

    SECTION( "Signature sum" )
    {
        CEntitySignature sign1, sign2, sign3;
        CEntitySignature signSum;

        sign1.add<Foo>();

        sign2.add<Foo>();
        sign2.add<Bar>();

        sign3.add<Bar>();
        sign3.add<Baz>();

        signSum = sign1 + sign2;
        REQUIRE( ( signSum.has<Foo>() && signSum.has<Bar>() ) );

        signSum += sign3;
        REQUIRE( ( signSum.has<Foo>() && signSum.has<Bar>() && signSum.has<Baz>() ) );
    }

    SECTION( "Signature difference" )
    {
        CEntitySignature sign1, sign2, sign3;
        CEntitySignature signDiff;

        sign1.add<Foo>();
        sign1.add<Bar>();

        sign2.add<Bar>();
        sign2.add<Baz>();

        sign3.add<Foo>();

        signDiff = sign1 - sign2;
        REQUIRE( ( signDiff.has<Foo>() && !signDiff.has<Bar>() && !signDiff.has<Baz>() ) );

        signDiff -= sign3;
        REQUIRE( ( !signDiff.has<Foo>() && !signDiff.has<Bar>() && !signDiff.has<Baz>() ) );
        REQUIRE( ( signDiff.isEmpty() ) );
    }
}