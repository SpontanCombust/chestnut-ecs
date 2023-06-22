#include "constants.hpp"

#include <algorithm>

namespace chestnut::ecs::internal
{
    inline CEntityRegistry::CEntityRegistry(const CComponentStorage *componentStorage) noexcept
    : m_componentStoragePtr(componentStorage), m_entityIdCounter(ENTITY_SLOT_MINIMAL)
    {

    }

    inline entityslot_t CEntityRegistry::registerNewEntity(bool canRecycleId) noexcept
    {
        entityslot_t id;

        if( canRecycleId && !m_vecRecycledEntityIDs.empty() )
        {
            id = m_vecRecycledEntityIDs.back();
            m_vecRecycledEntityIDs.pop_back();
        }
        else
        {
            id = m_entityIdCounter++;
        }

        return id;
    }

    inline bool CEntityRegistry::isEntityRegistered(entityslot_t id) const noexcept
    {
        if( id == ENTITY_SLOT_INVALID )
        {
            return false;
        }
        if(id < m_entityIdCounter)
        {
            auto it = std::find_if(m_vecRecycledEntityIDs.begin(), m_vecRecycledEntityIDs.end(),
                [id](entityslot_t recycledId) {
                    return id == recycledId;
                }
            );

            if(it == m_vecRecycledEntityIDs.end())
            {
                return true;
            }
        }

        return false;
    }

    inline void CEntityRegistry::unregisterEntity(entityslot_t id) noexcept
    {
        if(isEntityRegistered(id))
        {
            m_vecRecycledEntityIDs.push_back(id);
        }
    }

    inline entityslot_t CEntityRegistry::getHighestIdRegistered() const noexcept
    {
        return m_entityIdCounter;
    }

    inline entityslot_t CEntityRegistry::getEntityCount() const noexcept
    {
        return (entityslot_t)(m_entityIdCounter - ENTITY_SLOT_MINIMAL - m_vecRecycledEntityIDs.size());
    }

    inline entityslot_t CEntityRegistry::getEntityCountOfExactSignature(const CEntitySignature& requiredSignature) const noexcept
    {
        auto ents = this->findEntities([&requiredSignature](const CEntitySignature& sign) -> bool {
            return sign == requiredSignature;
        });

        return (entityslot_t)ents.size();
    }

    inline entityslot_t CEntityRegistry::getEntityCountOfPartialSignature(const CEntitySignature& requiredSignaturePart) const noexcept
    {
        auto ents = this->findEntities([&requiredSignaturePart](const CEntitySignature& sign) -> bool {
            return sign.hasAllFrom(requiredSignaturePart);
        });

        return (entityslot_t)ents.size();
    }

    inline CEntitySignature CEntityRegistry::getEntitySignature(entityslot_t id) const noexcept
    {
        if(isEntityRegistered(id))
        {
            return m_componentStoragePtr->signature(id);
        }

        return CEntitySignature();
    }
    
    inline std::vector<entityslot_t> CEntityRegistry::findEntities(std::function<bool(const CEntitySignature&)> predicate) const noexcept
    {
        std::vector<entityslot_t> ids;

        for(entityslot_t id = ENTITY_SLOT_MINIMAL; id <= m_entityIdCounter; id += 1)
        {
            if(isEntityRegistered(id) && predicate(m_componentStoragePtr->signature(id)))
            {
                ids.push_back(id);
            }
        }

        return ids;
    }

} // namespace chestnut::ecs::internal
