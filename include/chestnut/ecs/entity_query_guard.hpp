#ifndef __CHESTNUT_ECS_ENTITY_QUERY_GUARD_H__
#define __CHESTNUT_ECS_ENTITY_QUERY_GUARD_H__

#include "component_wrapper.hpp"
#include "component_storage.hpp"
#include "entity_query.hpp"

#include <unordered_set>

namespace chestnut::ecs::internal
{
    /**
     * @brief A helper class for managing queries
     * 
     * @details
     * With access to CComponentStorageTypeMap this class can manage the actual query they store
     * by fetching appropriate sets of components for different entities and laying them out in the batch
     * in the coherent manner, such that you can traverse vectors in mapCompTypeToCompVec as if they are
     * arrays in a typical SOA structure, except here it's not a actual struct, it's just a map of types.
     * Works as a buffer between component storage and actual queries to prevent instant changes if query
     * results in components being created or destroyed - queries are not cleaned completely on request,
     * they're only updated with new components or have their components erased.
     */
    class CEntityQueryGuard
    {
    private:
        CComponentStorageTypeMap *m_storageMapPtr;

        // used to add entity index information to target query
        // order matters, because we'll use this vector's indices to modify vectors in the map below
        std::vector< entityid > m_pendingIn_vecEntityIDs;
        // used to add component data to target query
        std::unordered_map< std::type_index, std::vector< IComponentWrapper * > > m_pendingIn_mapCompTypeToVecComp;

        // used to remove component data from target query; we remove whole component row by querying indices with these owner IDs
        // it's a set so we can find IDs quicker; order doesn't matter here
        std::unordered_set< entityid > m_pendingOut_setEntityIDs;


        CEntitySignature m_requireSignature;

        CEntitySignature m_rejectSignature;

        CEntityQuery m_targetQuery;


    public:
        CEntityQueryGuard( queryid id, const CEntitySignature& requireSignature, const CEntitySignature& rejectSignature, CComponentStorageTypeMap *storageMapPtr );


        // Can throw std::out_of_range if requested entity doesn't have all the required components
        // Doesn't check for duplicates
        void fetchAndAddEntityWithComponents( entityid entityID );

        void removeEntityWithComponents( entityid entityID );


        // Returns whether the underlying query has any components stored inside it after the update
        bool updateQuery();


        bool testQuery( const CEntitySignature& signature ) const;

        const CEntityQuery& getQuery() const;
    };

} // namespace chestnut::ecs::internal

#endif // __CHESTNUT_ECS_ENTITY_QUERY_GUARD_H__