#ifndef __CHESTNUT_TYPES_H__
#define __CHESTNUT_TYPES_H__

#include <cstdint>

namespace chestnut::ecs
{
    typedef uint32_t entityid;

    typedef uint16_t segid;
    typedef uint16_t segsize;

    struct SComponentIndex
    {
        segid segmentID;
        segsize indexInSegment;
    };

} // namespace chestnut::ecs

#endif // __CHESTNUT_TYPES_H__