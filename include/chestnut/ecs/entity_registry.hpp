#pragma once

#include "types.hpp"
#include "entity.hpp"
#include "entity_signature.hpp"

#include <tl/optional.hpp>

#include <unordered_map>
#include <vector>

namespace chestnut::ecs::internal
{
    /**
     * @brief Class that manages distribution of entity IDs
     */
    class CEntityRegistry
    {
    private:
        entityslot_t m_entitySlotAccumulator;
        std::vector<entityslot_t> m_vecRecycledSlots;

        std::unordered_map<CUniqueIdentifier, entityslot_t> m_mapEntityUuidToSlot;


    public:
        /**
         * @brief Constructor
         */
        CEntityRegistry() noexcept;

        CEntity registerNewEntity() noexcept;
        bool isEntityRegistered(CUniqueIdentifier uuid) const noexcept;
        void unregisterEntity(CUniqueIdentifier uuid) noexcept;
        tl::optional<entityslot_t> getSlotForEntity(CUniqueIdentifier uuid) const noexcept;
        
        /**
         * @brief Get the amount of all registered entities
         * 
         * @return entity count
         */
        size_t getEntityCount() const noexcept;
    };
    
} // namespace chestnut::ecs::internal


#include "entity_registry.inl"
