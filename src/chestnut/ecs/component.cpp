#include "chestnut/ecs/component.hpp"

namespace chestnut::ecs
{    
    CComponent::CComponent() 
    {
        owner = ENTITY_ID_INVALID;
    }

} // namespace chestnut::ecs
