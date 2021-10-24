#ifndef __CHESTNUT_ECS_ENTITY_QUERY_H__
#define __CHESTNUT_ECS_ENTITY_QUERY_H__

#include "types.hpp"
#include "component_wrapper.hpp"
#include "entity_signature.hpp"

#include <functional>
#include <vector>

namespace chestnut::ecs
{
    namespace internal
    {
        class CEntityQueryGuard; // forward declaration

    } // namespace internal
    
    
    class CEntityQuery
    {
        friend class internal::CEntityQueryGuard;

    private:
        typedef std::unordered_map< std::type_index, std::vector< internal::IComponentWrapper * > > CComponentWrapperTypeMap;

        queryid m_id;

        std::vector< entityid > m_vecEntityIDs;
        CComponentWrapperTypeMap m_mapCompTypeToVecComp;        

    public:
        CEntityQuery( queryid id );

        CEntityQuery( const CEntityQuery& ) = delete;
        CEntityQuery& operator=( const CEntityQuery& ) = delete;


        queryid getID() const;

        entitysize getEntityCount() const;


        // Can throw std::out_of_range if requested type isn't stored in the query
        template< class C1 >
        void forEachEntityWith( std::function< void( C1& ) > func ) const;

        // Can throw std::out_of_range if requested types combination isn't stored in the query
        template< class C1, class C2 >
        void forEachEntityWith( std::function< void( C1&, C2& ) > func ) const;

        // Can throw std::out_of_range if requested types combination isn't stored in the query
        template< class C1, class C2, class C3 >
        void forEachEntityWith( std::function< void( C1&, C2&, C3& ) > func ) const;

        // Can throw std::out_of_range if requested types combination isn't stored in the query
        template< class C1, class C2, class C3, class C4 >
        void forEachEntityWith( std::function< void( C1&, C2&, C3&, C4& ) > func ) const;



        void forEachEntityWith( std::function< void( entityid ) > func ) const;

        // Can throw std::out_of_range if requested type isn't stored in the query
        template< class C1 >
        void forEachEntityWith( std::function< void( entityid, C1& ) > func ) const;

        // Can throw std::out_of_range if requested types combination isn't stored in the query
        template< class C1, class C2 >
        void forEachEntityWith( std::function< void( entityid, C1&, C2& ) > func ) const; 

        // Can throw std::out_of_range if requested types combination isn't stored in the query
        template< class C1, class C2, class C3 >
        void forEachEntityWith( std::function< void( entityid, C1&, C2&, C3& ) > func ) const;

        // Can throw std::out_of_range if requested types combination isn't stored in the query
        template< class C1, class C2, class C3, class C4 >
        void forEachEntityWith( std::function< void( entityid, C1&, C2&, C3&, C4& ) > func ) const;
    };

} // namespace chestnut::ecs


#include "entity_query.tpp"


#endif // __CHESTNUT_ECS_ENTITY_QUERY_H__