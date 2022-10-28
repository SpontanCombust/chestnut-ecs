#include "constants.hpp"

#include <algorithm>

namespace chestnut::ecs::internal
{
    inline CEntityRegistry::CEntityRegistry(const CComponentStorage *componentStorage) noexcept
    : m_componentStoragePtr(componentStorage), m_entityIdCounter(ENTITY_ID_MINIMAL)
    {

    }

    inline entityid_t CEntityRegistry::registerNewEntity(bool canRecycleId) noexcept
    {
        entityid_t id;

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

    inline bool CEntityRegistry::isEntityRegistered(entityid_t id) const noexcept
    {
        if( id == ENTITY_ID_INVALID )
        {
            return false;
        }
        if(id < m_entityIdCounter)
        {
            auto it = std::find_if(m_vecRecycledEntityIDs.begin(), m_vecRecycledEntityIDs.end(),
                [id](entityid_t recycledId) {
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

    inline void CEntityRegistry::unregisterEntity(entityid_t id) noexcept
    {
        if(isEntityRegistered(id))
        {
            m_vecRecycledEntityIDs.push_back(id);
        }
    }

    inline entitysize_t CEntityRegistry::getEntityCount() const noexcept
    {
        return (entitysize_t)(m_entityIdCounter - ENTITY_ID_MINIMAL - m_vecRecycledEntityIDs.size());
    }

    inline entitysize_t CEntityRegistry::getEntityCountOfExactSignature(const CEntitySignature& requiredSignature) const noexcept
    {
        auto ents = this->findEntities([&requiredSignature](const CEntitySignature& sign) -> bool {
            return sign == requiredSignature;
        });

        return (entitysize_t)ents.size();
    }

    inline entitysize_t CEntityRegistry::getEntityCountOfPartialSignature(const CEntitySignature& requiredSignaturePart) const noexcept
    {
        auto ents = this->findEntities([&requiredSignaturePart](const CEntitySignature& sign) -> bool {
            return sign.hasAllFrom(requiredSignaturePart);
        });

        return (entitysize_t)ents.size();
    }

    inline CEntitySignature CEntityRegistry::getEntitySignature(entityid_t id) const noexcept
    {
        if(isEntityRegistered(id))
        {
            return m_componentStoragePtr->signature(id);
        }

        return CEntitySignature();
    }
    
    inline std::vector<entityid_t> CEntityRegistry::findEntities(std::function<bool(const CEntitySignature&)> predicate) const noexcept
    {
        std::vector<entityid_t> ids;

        for(auto it = m_componentStoragePtr->cbegin(); it != m_componentStoragePtr->cend(); it++)
        {
            if(predicate(it.signature()))
            {
                ids.push_back(it.id());
            }
        }

        return ids;
    }

} // namespace chestnut::ecs::internal
