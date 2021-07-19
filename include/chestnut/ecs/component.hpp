#ifndef __CHESTNUT_COMPONENT_H__
#define __CHESTNUT_COMPONENT_H__

#include "types.hpp"

namespace chestnut
{
namespace ecs
{ 
    class CComponent
    {
    public:
        entityid owner;
    };

} // namespace ecs

} // namespace chestnut

#endif // __CHESTNUT_COMPONENT_H__