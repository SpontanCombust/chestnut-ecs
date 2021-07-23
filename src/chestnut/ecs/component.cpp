#include "chestnut/ecs/component.hpp"

namespace chestnut
{    
    CComponent::CComponent() 
    {
        owner = ENTITY_ID_INVALID;
    }

} // namespace chestnut
