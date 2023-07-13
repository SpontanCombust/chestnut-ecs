#include <catch2/catch.hpp>

#include "../include/chestnut/ecs/entity_query_supplier.hpp"

using namespace chestnut::ecs;
using namespace chestnut::ecs::internal;

struct Foo {};
struct Bar {};
struct Baz {};
struct Fuzz {};

TEST_CASE("Entity query supplier test")
{
    SECTION("Signature validation")
    {
        REQUIRE_NOTHROW(CEntityQuerySupplier(
            CEntitySignature::from<Foo, Bar>(),
            CEntitySignature()
        ));

        //empty require
        REQUIRE_NOTHROW(CEntityQuerySupplier(
            CEntitySignature(),
            CEntitySignature::from<Baz, Fuzz>()
        ));

        //'reject' with required type
        REQUIRE_THROWS(CEntityQuerySupplier(
            CEntitySignature::from<Foo, Bar>(),
            CEntitySignature::from<Foo, Fuzz>()
        ));
    }

    SECTION("Require and reject signature equality")
    {
        auto req1 = CEntitySignature::from<Foo, Bar>();
        auto rej1 = CEntitySignature::from<Baz, Fuzz>();

        CEntityQuerySupplier supplier1(req1, rej1);

        REQUIRE(supplier1.requireSignature() == req1);
        REQUIRE(supplier1.rejectSignature() == rej1);


        auto req2 = CEntitySignature::from<Fuzz>();
        auto rej2 = CEntitySignature();

        CEntityQuerySupplier supplier2(req1, rej1);

        REQUIRE(supplier2.requireSignature() == req1);
        REQUIRE(supplier2.rejectSignature() == rej1);
    }

    SECTION("Signature testing")
    {
        CEntityQuerySupplier supplier1(
            CEntitySignature::from<Foo, Bar>(), 
            CEntitySignature::from<Baz, Fuzz>()
        );

        REQUIRE_FALSE(supplier1.testSignature(CEntitySignature()));
        REQUIRE_FALSE(supplier1.testSignature(CEntitySignature::from<Foo>()));
        REQUIRE(supplier1.testSignature(CEntitySignature::from<Foo, Bar>()));
        REQUIRE(supplier1.testSignature(CEntitySignature::from<Foo, Bar, int>()));
        REQUIRE_FALSE(supplier1.testSignature(CEntitySignature::from<Foo, Bar, Baz>()));
        REQUIRE_FALSE(supplier1.testSignature(CEntitySignature::from<Bar, Fuzz>()));


        CEntityQuerySupplier supplier2(
            CEntitySignature::from<Foo, Bar, Baz, Fuzz>(), 
            CEntitySignature()
        );

        REQUIRE_FALSE(supplier2.testSignature(CEntitySignature()));
        REQUIRE_FALSE(supplier2.testSignature(CEntitySignature::from<Foo>()));
        REQUIRE_FALSE(supplier2.testSignature(CEntitySignature::from<Bar>()));
        REQUIRE_FALSE(supplier2.testSignature(CEntitySignature::from<Baz>()));
        REQUIRE_FALSE(supplier2.testSignature(CEntitySignature::from<Fuzz>()));
        REQUIRE_FALSE(supplier2.testSignature(CEntitySignature::from<Foo, Bar>()));
        REQUIRE_FALSE(supplier2.testSignature(CEntitySignature::from<Foo, Bar, Baz>()));
        REQUIRE_FALSE(supplier2.testSignature(CEntitySignature::from<Foo, Bar, Baz, int>()));
        REQUIRE(supplier2.testSignature(CEntitySignature::from<Foo, Bar, Baz, Fuzz>()));
        REQUIRE(supplier2.testSignature(CEntitySignature::from<Foo, Bar, Baz, Fuzz, short>()));
        REQUIRE_FALSE(supplier2.testSignature(CEntitySignature::from<Bar, Fuzz>()));


        CEntityQuerySupplier supplier3(
            CEntitySignature::from<Fuzz>(), 
            CEntitySignature::from<Foo, Bar, Baz>()
        );

        REQUIRE_FALSE(supplier3.testSignature(CEntitySignature()));
        REQUIRE_FALSE(supplier3.testSignature(CEntitySignature::from<Foo>()));
        REQUIRE_FALSE(supplier3.testSignature(CEntitySignature::from<Bar>()));
        REQUIRE_FALSE(supplier3.testSignature(CEntitySignature::from<Baz>()));
        REQUIRE(supplier3.testSignature(CEntitySignature::from<Fuzz>()));
        REQUIRE_FALSE(supplier3.testSignature(CEntitySignature::from<Foo, Bar>()));
        REQUIRE_FALSE(supplier3.testSignature(CEntitySignature::from<Foo, Bar, Baz>()));
        REQUIRE_FALSE(supplier3.testSignature(CEntitySignature::from<Foo, Bar, Baz, int>()));
        REQUIRE_FALSE(supplier3.testSignature(CEntitySignature::from<Foo, Bar, Baz, Fuzz>()));
        REQUIRE_FALSE(supplier3.testSignature(CEntitySignature::from<Foo, Bar, Baz, Fuzz, short>()));
        REQUIRE_FALSE(supplier3.testSignature(CEntitySignature::from<Bar, Fuzz>()));
        REQUIRE(supplier3.testSignature(CEntitySignature::from<short, Fuzz>()));


        // should accept everything
        CEntityQuerySupplier supplier4(
            CEntitySignature{}, 
            CEntitySignature{}
        );

        REQUIRE(supplier4.testSignature(CEntitySignature()));
        REQUIRE(supplier4.testSignature(CEntitySignature::from<Foo>()));
        REQUIRE(supplier4.testSignature(CEntitySignature::from<Bar>()));
        REQUIRE(supplier4.testSignature(CEntitySignature::from<Baz>()));
        REQUIRE(supplier4.testSignature(CEntitySignature::from<Fuzz>()));
        REQUIRE(supplier4.testSignature(CEntitySignature::from<Foo, Bar>()));
        REQUIRE(supplier4.testSignature(CEntitySignature::from<Foo, Bar, Baz>()));
        REQUIRE(supplier4.testSignature(CEntitySignature::from<Foo, Bar, Baz, int>()));
        REQUIRE(supplier4.testSignature(CEntitySignature::from<Foo, Bar, Baz, Fuzz>()));
        REQUIRE(supplier4.testSignature(CEntitySignature::from<Foo, Bar, Baz, Fuzz, short>()));
        REQUIRE(supplier4.testSignature(CEntitySignature::from<Bar, Fuzz>()));
        REQUIRE(supplier4.testSignature(CEntitySignature::from<short, Fuzz>()));
    }

    SECTION("Queueuing")
    {
        CEntityQuerySupplier supplier(CEntitySignature::from<Foo>(), CEntitySignature());
        std::vector<entityslot_t> incoming;
        std::vector<entityslot_t> outgoing;
        std::vector<entityslot_t> current;


        REQUIRE_FALSE(supplier.hasPendingEntities());

        auto sign0 = CEntitySignature();
        auto sign1 = CEntitySignature::from<Foo>();
        supplier.proposeEntity(1, sign0, sign1);
        supplier.proposeEntity(2, sign0, sign1);
        supplier.proposeEntity(3, sign0, sign1);
        supplier.proposeEntity(4, sign0, sign1);
        REQUIRE(supplier.hasPendingEntities());

        supplier.processPendingEntities(current, incoming, outgoing);
        REQUIRE_FALSE(supplier.hasPendingEntities());

        std::sort(current.begin(), current.end());
        std::sort(incoming.begin(), incoming.end());
        std::sort(outgoing.begin(), outgoing.end());
        REQUIRE(current == std::vector<entityslot_t>({1, 2, 3, 4}));
        REQUIRE(incoming == std::vector<entityslot_t>({1, 2, 3, 4}));
        REQUIRE(outgoing == std::vector<entityslot_t>({}));


        auto sign2 = CEntitySignature::from<Foo, Bar>();
        supplier.proposeEntity(2, sign1, sign2);
        supplier.proposeEntity(3, sign1, sign2);

        supplier.processPendingEntities(current, incoming, outgoing);
        std::sort(current.begin(), current.end());
        std::sort(incoming.begin(), incoming.end());
        std::sort(outgoing.begin(), outgoing.end());
        REQUIRE(current == std::vector<entityslot_t>({1, 2, 3, 4}));
        REQUIRE(incoming == std::vector<entityslot_t>({}));
        REQUIRE(outgoing == std::vector<entityslot_t>({}));


        auto sign3 = CEntitySignature::from<Bar>();
        supplier.proposeEntity(1, sign2, sign3);
        supplier.proposeEntity(5, sign2, sign3);
        supplier.proposeEntity(6, sign2, sign3);
        supplier.proposeEntity(7, sign2, sign3);
        supplier.proposeEntity(6, sign2, sign3);
        supplier.proposeEntity(7, sign2, sign3);

        supplier.processPendingEntities(current, incoming, outgoing);
        std::sort(current.begin(), current.end());
        std::sort(incoming.begin(), incoming.end());
        std::sort(outgoing.begin(), outgoing.end());
        REQUIRE(current == std::vector<entityslot_t>({2, 3, 4}));
        REQUIRE(incoming == std::vector<entityslot_t>({}));
        REQUIRE(outgoing == std::vector<entityslot_t>({1}));


        auto sign4 = CEntitySignature::from<Bar, Foo>();
        supplier.proposeEntity(6, sign3, sign4);        
        supplier.proposeEntity(7, sign3, sign4);        
        supplier.proposeEntity(8, sign3, sign4);        
        supplier.proposeEntity(9, sign3, sign4);
        supplier.proposeEntity(10, sign3, sign4);   
     
        supplier.proposeEntity(9, sign4, sign3);       
        supplier.proposeEntity(10, sign4, sign3);        
        supplier.proposeEntity(8, sign4, sign3);

        supplier.processPendingEntities(current, incoming, outgoing);
        std::sort(current.begin(), current.end());
        std::sort(incoming.begin(), incoming.end());
        std::sort(outgoing.begin(), outgoing.end());
        REQUIRE(current == std::vector<entityslot_t>({2, 3, 4, 6, 7}));
        REQUIRE(incoming == std::vector<entityslot_t>({6, 7}));
        REQUIRE(outgoing == std::vector<entityslot_t>({}));
    }
}