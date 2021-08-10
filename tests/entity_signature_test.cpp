#include <catch2/catch.hpp>

#include "../include/chestnut/ecs/entity_signature.hpp"

struct Foo {};
struct Bar {};
struct Baz {};

TEST_CASE( "Entity signature test" )
{
    using namespace chestnut;

    SECTION( "Adding types to the signature" )
    {
        CEntitySignature singature;
        singature.add<Foo>(); // template method
        singature.add( typeid(Bar) ); // type index method

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
        REQUIRE( !signature.isEmpty() );
    }

    SECTION( "Clear signature" )
    {
        CEntitySignature signature;

        signature.add<Foo>();
        signature.add<Bar>();

        signature.clear();

        REQUIRE( signature.isEmpty() );
    }

    SECTION( "Signature size" )
    {
        CEntitySignature signature;

        signature.add<Foo>();
        signature.add<Bar>();
        signature.add<Bar>(); // repeat
        signature.add<Baz>();
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

    SECTION( "Signature being a part of other signature" )
    {
        CEntitySignature sign1, sign2;

        sign1.add( typeid(Foo) );
        sign1.add( typeid(Bar) );

        sign2.add( typeid(Foo) );
        sign2.add( typeid(Bar) );
        sign2.add( typeid(Baz) );

        REQUIRE( sign2.has( sign1 ) );
        REQUIRE_FALSE( sign1.has( sign2 ) );
    }
}