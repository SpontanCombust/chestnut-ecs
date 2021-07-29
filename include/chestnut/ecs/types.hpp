#ifndef __CHESTNUT_TYPES_H__
#define __CHESTNUT_TYPES_H__

#include <cstdint>

namespace chestnut
{
    typedef uint32_t entityid;

    typedef uint16_t segid;
    typedef uint32_t segsize;

    struct SComponentIndex
    {
        segid segmentIndex;
        segsize segmentSlotIndex;
    };

} // namespace chestnut

#endif // __CHESTNUT_TYPES_H__