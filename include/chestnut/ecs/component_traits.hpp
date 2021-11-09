#ifndef __CHESTNUT_ECS_COMPONENT_TRAITS_H__
#define __CHESTNUT_ECS_COMPONENT_TRAITS_H__

#include "types.hpp"
#include "macros.hpp"

namespace chestnut::ecs
{
    template< class C >
    struct ComponentTraits
    {
        static constexpr const char *name           = "CComponent";
        static const segsize_t storageSegmentSize     = CHESTNUT_DEFAULT_STORAGE_SEGMENT_SIZE;
        static const segsize_t storageInitCapacity    = CHESTNUT_DEFAULT_STORAGE_SEGMENT_SIZE;
    };

} // namespace chestnut::ecs

#endif // __CHESTNUT_ECS_COMPONENT_TRAITS_H__