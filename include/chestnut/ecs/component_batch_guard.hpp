#ifndef __CHESTNUT_ECS_COMPONENT_BATCH_GUARD_H__
#define __CHESTNUT_ECS_COMPONENT_BATCH_GUARD_H__

#include "component_batch.hpp"
#include "component_storage.hpp"

#include <unordered_set>

namespace chestnut::ecs::internal
{
    /**
     * @brief A helper class for managing component batches
     * 
     * @details
     * With access to CComponentStorageTypeMap this class can manage the actual component batch they store
     * by fetching appropriate sets of components for different entities and laying them out in the batch
     * in the coherent manner, such that you can traverse vectors in mapCompTypeToCompVec as if they are
     * arrays in a typical SOA structure, except here it's not a actual struct, it's just a map of types
     */
    class CComponentBatchGuard
    {
    private:
        // used to add entity index information to target batch
        // order matters, because we'll use this vector's indices to modify vectors in the map below
        std::vector< entityid > m_pendingIn_vecEntityIDs;
        // used to add component data to target batch
        std::unordered_map< std::type_index, std::vector< CComponent * > > m_pendingIn_mapCompTypeToVecComp;

        // used to remove component data from target batch; we remove whole component row by querying indices with these owner IDs
        // it's a set so we can find IDs quicker; order doesn't matter here
        std::unordered_set< entityid > m_pendingOut_setEntityIDs;


        SComponentBatch m_targetBatch;

    public:
        CComponentBatchGuard( const CEntitySignature& signature );


        // You have to make sure the entity in question has all required components
        // i.e. components specified by batch signature
        // Doesn't check for duplicates
        void fetchAndAddEntityWithComponents( entityid entityID, const CComponentStorageTypeMap& storageMap );

        void removeEntityWithComponents( entityid entityID );

        // Returns whether the underlying batch has any components stored inside it after the update
        bool updateBatch();


        // Returns whether the underlying batch has any components stored inside it
        // Note that it won't take into consideration any components that are currently pending in/out to it
        bool hasAnyComponentsInBatch() const;

        const CEntitySignature& getBatchSignature() const;

        const SComponentBatch *getBatchPtr() const;
    };

} // namespace chestnut::ecs::internal

#endif // __CHESTNUT_ECS_COMPONENT_BATCH_GUARD_H__