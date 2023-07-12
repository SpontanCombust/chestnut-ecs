#include <catch2/catch.hpp>

#include "../include/chestnut/ecs/entity_signature.hpp"

#include <algorithm>
#include <set>
#include <vector>


using namespace chestnut::ecs;

struct Foo {};
struct Bar {};
struct Baz {};

TEST_CASE( "Entity signature test" )
{
    SECTION( "Adding types" )
    {
        CEntitySignature sign1, sign2;


        sign1.add<Foo,Bar>();
        REQUIRE( sign1.has<Foo, Bar>() );
        REQUIRE_FALSE( sign1.has<Baz>() );
        sign1.add<Foo>(); // repeat type
        REQUIRE( sign1.size() == 2);

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

    SECTION( "Removing types" )
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

        REQUIRE( signature.empty() );
        
        signature.add<Foo>();
        REQUIRE_FALSE( signature.empty() );
    }

    SECTION( "Clear" )
    {
        CEntitySignature signature;

        signature.add<Foo,Bar>();

        signature.clear();

        REQUIRE( signature.empty() );
    }

    SECTION( "Size" )
    {
        CEntitySignature signature;

        signature.add<Foo, Bar, Bar, Baz>(); // Bar repeat
        REQUIRE( signature.size() == 3 );

        signature.remove<Bar>();
        REQUIRE( signature.size() == 2 );

        signature.clear();
        REQUIRE( signature.size() == 0 );
    }

    SECTION( "Sum" )
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

    SECTION( "Difference" )
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
        REQUIRE( ( signDiff.empty() ) );
    }

    SECTION( "Intersection" )
    {
        CEntitySignature sign1, sign2, sign3, sign4;

        sign1.add( typeid(Foo) );
        sign1.add( typeid(Bar) );

        sign2.add( typeid(Foo) );
        sign2.add( typeid(Bar) );
        sign2.add( typeid(Baz) );

        sign3.add( typeid(Bar) );
        sign3.add( typeid(Baz) );

        // sign4 is empty

        REQUIRE( (sign1 & sign2) == sign1 );
        REQUIRE( (sign1 & sign2 & sign3) == CEntitySignature::from<Bar>() );
        REQUIRE( (sign1 & sign2 & sign3 & sign4).empty() );
    }

    SECTION("Make signature in-place")
    {
        CEntitySignature sign1, sign2;

        sign1.add(typeid(Foo));
        sign1.add(typeid(Bar));

        sign2.add(typeid(Baz));

        CEntitySignature sign1t1 = CEntitySignature::from<Foo, Bar>();
        CEntitySignature sign2t1 = CEntitySignature::from<Baz>();
        REQUIRE(sign1 == sign1t1);
        REQUIRE(sign2 == sign2t1);

        CEntitySignature sign1t2 = CEntitySignature::from<Foo, Bar>();
        CEntitySignature sign2t2 = CEntitySignature::from<Baz>();
        REQUIRE(sign1 == sign1t2);
        REQUIRE(sign2 == sign2t2);
    }

    SECTION("iterator")
    {
        std::set<std::type_index> orderedSet;

        orderedSet.insert(typeid(Foo));
        orderedSet.insert(typeid(Bar));
        orderedSet.insert(typeid(short));
        orderedSet.insert(typeid(Baz));
        orderedSet.insert(typeid(int));

        auto sign = CEntitySignature::from<Foo, Baz, int, short, Bar>();


        auto setIt = orderedSet.begin();
        auto signIt = sign.begin();

        for(int i = 0; i < 5; ++i, ++setIt, ++signIt)
        {
            REQUIRE(*setIt == *signIt);
        }
        REQUIRE(signIt == sign.end());
    }

    SECTION("add in different order")
    {
        auto sign1 = CEntitySignature::from<Foo, Bar, Baz, short>();
        auto sign2 = CEntitySignature::from<Baz, short, Bar, Foo, Baz>();

        REQUIRE(sign1 == sign2);
    }

    SECTION("hash")
    {
        std::vector<CEntitySignature> signs {{
            CEntitySignature(),
            CEntitySignature::from<Foo>(),
            CEntitySignature::from<Foo, Bar>(),
            CEntitySignature::from<Foo, Bar, Baz>(),
            CEntitySignature::from<Foo, Baz>(),
            CEntitySignature::from<Bar, Baz>(),
            CEntitySignature::from<Bar>(),
            CEntitySignature::from<Baz>(),
            CEntitySignature::from<Baz, int, char>(),
            CEntitySignature::from<int, char>(),
            CEntitySignature::from<Foo, Bar, Baz, char, unsigned char, int, long, short>(),
            CEntitySignature::from<std::vector<CEntitySignature>>(),
        }};

        std::vector<size_t> hashes; 
        std::transform(signs.cbegin(), signs.cend(), std::back_inserter(hashes), [](const CEntitySignature& sign) {
            return std::hash<CEntitySignature>()(sign);
        });
        std::sort(hashes.begin(), hashes.end());

        // should not find any two equal values
        REQUIRE(std::adjacent_find(hashes.begin(), hashes.end()) == hashes.end());
    }
}