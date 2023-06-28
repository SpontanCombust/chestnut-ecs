#pragma once

#include "types.hpp"
#include "uuid.hpp"

namespace chestnut::ecs
{
    class CEntity
    {
    public:
        CUniqueIdentifier uuid;
        entityslot_t slot;

    public:
        CEntity(CUniqueIdentifier uuid, entityslot_t slot)
        : uuid(uuid), slot(slot)
        {

        }

        inline bool operator==(const CEntity& other) const
        {
            // entity will never have its slot changed, no need to check that then
            return uuid == other.uuid;   
        }

        inline bool operator!=(const CEntity& other) const
        {
            return uuid != other.uuid;
        }
    };

} // namespace chestnut::ecs
