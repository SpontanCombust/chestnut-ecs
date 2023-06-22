#pragma once

#include "types.hpp"

namespace chestnut::ecs
{
    class CEntity
    {
    private:
        entityuuid_t m_uuid;
        entityslot_t m_slot;

    public:
        CEntity(entityuuid_t uuid, entityslot_t slot)
        : m_uuid(uuid), m_slot(slot)
        {

        }

        bool operator==(const CEntity& other)
        {
            // entity will never have its slot changed, no need to check that then
            return m_uuid == other.m_uuid;   
        }

        bool operator!=(const CEntity& other)
        {
            return m_uuid != other.m_uuid;
        }
    };

} // namespace chestnut::ecs
