#pragma once

#include "entity_signature.hpp"
#include "sparse_set.hpp"
#include "types.hpp"

#include <tl/optional.hpp>

#include <unordered_set>
#include <vector>

namespace chestnut::ecs::internal
{
    /**
     * @brief A helper class for managing queries
     * 
     * @details
     * Acts as a buffer object between the storage and proper queries so that query object can't be mutated
     * during its usage.
     */
    class CEntityQuerySupplier
    {
    private:
        CEntitySignature m_requireSignature;
        CEntitySignature m_rejectSignature;

        std::unordered_set<entityslot_t> m_pendingInEntitySlots;
        std::unordered_set<entityslot_t> m_pendingOutEntitySlots;
        std::unordered_set<entityslot_t> m_currentEntitySlots;

    public:
        CEntityQuerySupplier(const CEntitySignature& requireSignature, const CEntitySignature& rejectSignature);

        const CEntitySignature& requireSignature() const;
        const CEntitySignature& rejectSignature() const;

        // Depending on previous and current entity signature, will add its slot to the pending-in queue, pending-out queue or won't add it to neither
        bool proposeEntity(entityslot_t entitySlot, tl::optional<CEntitySignature> prevSign, tl::optional<CEntitySignature> currSign);
        bool hasPendingEntities() const;

        void processPendingEntities(std::vector<entityslot_t>& destCurrent, 
                                    std::vector<entityslot_t>& destIncoming, 
                                    std::vector<entityslot_t>& destOutgoing);
        
        bool testSignature(const CEntitySignature& signature) const;
    };

} // namespace chestnut::ecs::internal


#include "entity_query_supplier.inl"