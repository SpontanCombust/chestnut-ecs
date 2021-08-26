#ifndef __CHESTNUT_ECS_COMPONENT_TRAITS_H__
#define __CHESTNUT_ECS_COMPONENT_TRAITS_H__

#include "types.hpp"

namespace chestnut::ecs
{
#ifndef CHESTNUT_DEFAULT_STORAGE_SEGMENT_SIZE
    #define CHESTNUT_DEFAULT_STORAGE_SEGMENT_SIZE 64 //arbitrary number
#endif

    template< class C >
    struct ComponentTraits
    {
        static constexpr const char *name           = "CComponent";
        static const segsize storageSegmentSize     = CHESTNUT_DEFAULT_STORAGE_SEGMENT_SIZE;
        static const segsize storageInitCapacity    = CHESTNUT_DEFAULT_STORAGE_SEGMENT_SIZE;
    };

} // namespace chestnut::ecs

#endif // __CHESTNUT_ECS_COMPONENT_TRAITS_H__