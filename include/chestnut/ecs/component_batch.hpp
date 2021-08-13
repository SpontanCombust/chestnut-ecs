#ifndef __CHESTNUT_COMPONENT_BATCH_H__
#define __CHESTNUT_COMPONENT_BATCH_H__

#include "types.hpp"
#include "entity_signature.hpp"
#include "component.hpp"
#include "component_storage.hpp"

#include <typeindex>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace chestnut
{
    /**
     * @brief A struct emulating the SOA layout using a map. Contains component pointers belonging to multiple entities having the same component signature
     * 
     * @details
     * A struct housing map of parallel arrays (vectors) of component pointers. A batch is created for a specified component set signature. 
     * A struct created from need of storing unspecified number of components of unspecified number of types. 
     * Allows for convenient access to components that belong to an entity with specific set of components, 
     * which streamlines component systems' process of fetching very specific component configurations if they need more than one 
     * type to perform an operation (for example transform component is needed for a renderable component to be drawn at a specific location). 
     * You may submit component by passing a component set with corresponding signature. 
     * You may get component pointers in form of a vector for specified component type. 
     */
    struct SComponentBatch
    {
        CEntitySignature signature;
        
        std::vector< entityid > vecEntityIDs;
        std::unordered_map< std::type_index, std::vector< CComponent * > > mapCompTypeToCompVec;
    };

namespace internal
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
    
} // namespace internal
    
} // namespace chestnut

#endif // __CHESTNUT_COMPONENT_BATCH_H__