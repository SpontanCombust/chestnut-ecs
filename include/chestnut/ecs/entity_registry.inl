#include "constants.hpp"

#include <algorithm>

namespace chestnut::ecs::internal
{
    inline CEntityRegistry::CEntityRegistry() noexcept
    : m_entitySlotAccumulator(ENTITY_SLOT_MINIMAL)
    {

    }

    inline CEntity CEntityRegistry::registerNewEntity() noexcept
    {
        entityslot_t slot;

        if (!m_vecRecycledSlots.empty())
        {
            slot = m_vecRecycledSlots.back();
            m_vecRecycledSlots.pop_back();
        }
        else
        {
            slot = m_entitySlotAccumulator++;
        }

        CUniqueIdentifier uuid;
        m_mapEntityUuidToSlot[uuid] = slot;

        return CEntity(uuid, slot);
    }

    inline bool CEntityRegistry::isEntityRegistered(CUniqueIdentifier uuid) const noexcept
    {
        return m_mapEntityUuidToSlot.find(uuid) != m_mapEntityUuidToSlot.end();
    }

    inline void CEntityRegistry::unregisterEntity(CUniqueIdentifier uuid) noexcept
    {
        auto slotIt = m_mapEntityUuidToSlot.find(uuid);
        if (slotIt != m_mapEntityUuidToSlot.end())
        {
            m_vecRecycledSlots.push_back(slotIt->second);
            m_mapEntityUuidToSlot.erase(slotIt);
        }
    }

    inline tl::optional<entityslot_t> CEntityRegistry::getSlotForEntity(CUniqueIdentifier uuid) const noexcept
    {
        auto slotIt = m_mapEntityUuidToSlot.find(uuid);
        if (slotIt != m_mapEntityUuidToSlot.end())
        {
            return slotIt->second;
        }
        else
        {
            return tl::nullopt;
        }
    }

    inline size_t CEntityRegistry::getEntityCount() const noexcept
    {
        return m_mapEntityUuidToSlot.size();
    }

} // namespace chestnut::ecs::internal
