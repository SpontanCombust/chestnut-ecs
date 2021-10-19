#ifndef __CHESTNUT_ECS_TYPES_H__
#define __CHESTNUT_ECS_TYPES_H__

#include <cstdint> // uint16_t, uint32_t

namespace chestnut::ecs
{
    typedef uint32_t entityid;
    typedef entityid entitysize;

    typedef uint16_t segid;
    typedef entitysize segsize; // a single segment can have the max amount of components equal to max amount of available entities

} // namespace chestnut::ecs

#endif // __CHESTNUT_ECS_TYPES_H__