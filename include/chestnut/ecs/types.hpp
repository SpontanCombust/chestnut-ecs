#ifndef __CHESTNUT_ECS_TYPES_H__
#define __CHESTNUT_ECS_TYPES_H__

#include <cstdint> // uint16_t, uint32_t

namespace chestnut::ecs
{
    typedef uint32_t entityid_t;
    typedef entityid_t entitysize_t;

    typedef uint16_t segid_t;
    typedef entitysize_t segsize_t; // a single segment can have the max amount of components equal to max amount of available entities

    typedef uint16_t queryid_t;

} // namespace chestnut::ecs

#endif // __CHESTNUT_ECS_TYPES_H__