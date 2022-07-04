#pragma once

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

        queryid_t m_id;

        std::vector< entityid_t > m_vecEntityIDs;
        CComponentWrapperTypeMap m_mapCompTypeToVecComp;        

        // Sorting helper vectors as query members to avoid allocating and deallocating memory for them on every sort call
        std::vector< entityid_t > m_sort_orderedIndices;
        std::vector< entityid_t > m_sort_reorderedEntityIDs;
        std::vector< internal::IComponentWrapper* > m_sort_reorderedComponents;

    public:
        CEntityQuery( queryid_t id );

        CEntityQuery( const CEntityQuery& ) = delete;
        CEntityQuery& operator=( const CEntityQuery& ) = delete;


        queryid_t getID() const;

        entitysize_t getEntityCount() const;


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



        void forEachEntityWith( std::function< void( entityid_t ) > func ) const;

        // Can throw std::out_of_range if requested type isn't stored in the query
        template< class C1 >
        void forEachEntityWith( std::function< void( entityid_t, C1& ) > func ) const;

        // Can throw std::out_of_range if requested types combination isn't stored in the query
        template< class C1, class C2 >
        void forEachEntityWith( std::function< void( entityid_t, C1&, C2& ) > func ) const; 

        // Can throw std::out_of_range if requested types combination isn't stored in the query
        template< class C1, class C2, class C3 >
        void forEachEntityWith( std::function< void( entityid_t, C1&, C2&, C3& ) > func ) const;

        // Can throw std::out_of_range if requested types combination isn't stored in the query
        template< class C1, class C2, class C3, class C4 >
        void forEachEntityWith( std::function< void( entityid_t, C1&, C2&, C3&, C4& ) > func ) const;



        // Can throw std::out_of_range if requested type isn't stored in the query
        template< class C1 >
        void forEachEntityPairWith( std::function< void( C1&, C1& ) > func ) const;

        // Can throw std::out_of_range if requested types combination isn't stored in the query
        template< class C1, class C2 >
        void forEachEntityPairWith( std::function< void( C1&, C2&, C1&, C2& ) > func ) const;

        // Can throw std::out_of_range if requested types combination isn't stored in the query
        template< class C1, class C2, class C3 >
        void forEachEntityPairWith( std::function< void( C1&, C2&, C3&, C1&, C2&, C3& ) > func ) const;

        // Can throw std::out_of_range if requested types combination isn't stored in the query
        template< class C1, class C2, class C3, class C4 >
        void forEachEntityPairWith( std::function< void( C1&, C2&, C3&, C4&, C1&, C2&, C3&, C4& ) > func ) const;



        void forEachEntityPairWith( std::function< void( entityid_t, entityid_t ) > func ) const;

        // Can throw std::out_of_range if requested type isn't stored in the query
        template< class C1 >
        void forEachEntityPairWith( std::function< void( entityid_t, C1&, entityid_t, C1& ) > func ) const;

        // Can throw std::out_of_range if requested types combination isn't stored in the query
        template< class C1, class C2 >
        void forEachEntityPairWith( std::function< void( entityid_t, C1&, C2&, entityid_t, C1&, C2& ) > func ) const; 

        // Can throw std::out_of_range if requested types combination isn't stored in the query
        template< class C1, class C2, class C3 >
        void forEachEntityPairWith( std::function< void( entityid_t, C1&, C2&, C3&, entityid_t, C1&, C2&, C3& ) > func ) const;

        // Can throw std::out_of_range if requested types combination isn't stored in the query
        template< class C1, class C2, class C3, class C4 >
        void forEachEntityPairWith( std::function< void( entityid_t, C1&, C2&, C3&, C4&, entityid_t, C1&, C2&, C3&, C4& ) > func ) const;        



        // Can throw std::out_of_range if requested type isn't stored in the query
        template< class C1 >
        void sort( std::function< bool( const C1&, const C1& )> compare );

        // Can throw std::out_of_range if requested types combination isn't stored in the query
        template< class C1, class C2 >
        void sort( std::function< bool( const C1&, const C2&, const C1&, const C2& )> compare );

        // Can throw std::out_of_range if requested types combination isn't stored in the query
        template< class C1, class C2, class C3 >
        void sort( std::function< bool( const C1&, const C2&, const C3&, const C1&, const C2&, const C3& )> compare );

        // Can throw std::out_of_range if requested types combination isn't stored in the query
        template< class C1, class C2, class C3, class C4 >
        void sort( std::function< bool( const C1&, const C2&, const C3&, const C4&, const C1&, const C2&, const C3&, const C4& )> compare );


    private:
        void reorderData();
    };

} // namespace chestnut::ecs


#include "entity_query.inl"
