#ifndef __CHESTNUT_ECS_COMPONENT_BATCH_H__
#define __CHESTNUT_ECS_COMPONENT_BATCH_H__

#include "types.hpp"
#include "entity_signature.hpp"
#include "component.hpp"

#include <typeindex>
#include <unordered_map>
#include <vector>

namespace chestnut::ecs
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
    
} // namespace chestnut::ecs

#endif // __CHESTNUT_ECS_COMPONENT_BATCH_H__