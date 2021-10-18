#ifndef __CHESTNUT_ECS_ENTITY_QUERY_H__
#define __CHESTNUT_ECS_ENTITY_QUERY_H__

#include "entity_signature.hpp"
#include "component_batch.hpp"

#include <functional>
#include <vector>

namespace chestnut::ecs
{
    struct SEntityQuery
    {
        std::function< bool( const CEntitySignature& ) > entitySignCond;
        std::vector< const SComponentBatch * > vecBatches;
    };


    // Can throw std::out_of_range if requested type isn't fully compatible with the query
    template< class C1 >
    void forEachEntityInQuery( const SEntityQuery& query, std::function< void( C1& ) > func );

    // Can throw std::out_of_range if requested types combination isn't fully compatible with the query
    template< class C1, class C2 >
    void forEachEntityInQuery( const SEntityQuery& query, std::function< void( C1&, C2& ) > func );

    // Can throw std::out_of_range if requested types combination isn't fully compatible with the query
    template< class C1, class C2, class C3 >
    void forEachEntityInQuery( const SEntityQuery& query, std::function< void( C1&, C2&, C3& ) > func );

    // Can throw std::out_of_range if requested types combination isn't fully compatible with the query
    template< class C1, class C2, class C3, class C4 >
    void forEachEntityInQuery( const SEntityQuery& query, std::function< void( C1&, C2&, C3&, C4& ) > func );



    void forEachEntityInQuery( const SEntityQuery& query, std::function< void( entityid ) > func );

    // Can throw std::out_of_range if requested type isn't fully compatible with the query
    template< class C1 >
    void forEachEntityInQuery( const SEntityQuery& query, std::function< void( entityid, C1& ) > func );

    // Can throw std::out_of_range if requested types combination isn't fully compatible with the query
    template< class C1, class C2 >
    void forEachEntityInQuery( const SEntityQuery& query, std::function< void( entityid, C1&, C2& ) > func ); 

    // Can throw std::out_of_range if requested types combination isn't fully compatible with the query
    template< class C1, class C2, class C3 >
    void forEachEntityInQuery( const SEntityQuery& query, std::function< void( entityid, C1&, C2&, C3& ) > func );

    // Can throw std::out_of_range if requested types combination isn't fully compatible with the query
    template< class C1, class C2, class C3, class C4 >
    void forEachEntityInQuery( const SEntityQuery& query, std::function< void( entityid, C1&, C2&, C3&, C4& ) > func );

} // namespace chestnut::ecs


#include "entity_query.tpp"


#endif // __CHESTNUT_ECS_ENTITY_QUERY_H__