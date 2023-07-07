#pragma once

#include "entity_signature.hpp"
#include "types.hpp"

#include <unordered_set>
#include <vector>

namespace chestnut::ecs
{
    /**
     * @brief Struct used to tell how many entities got added and removed from query on its update
     * 
     */
    struct SEntityQueryUpdateInfo
    {
        unsigned int added = 0;
        unsigned int removed = 0;
        unsigned int total = 0;
    };
}

namespace chestnut::ecs::internal
{
    //TODO unit test CEntityQueryGuard
    /**
     * @brief A helper class for managing queries
     * 
     * @details
     * Acts as a buffer object between the storage and proper queries so that query object can't be mutated
     * during its usage.
     */
    class CEntityQueryGuard
    {
    private:
        CEntitySignature m_requireSignature;
        CEntitySignature m_rejectSignature;

        std::unordered_set<entityslot_t> m_pendingIn_setEntitySlots;
        std::unordered_set<entityslot_t> m_pendingOut_setEntitySlots;

    public:
        CEntityQueryGuard(const CEntitySignature& requireSignature, const CEntitySignature& rejectSignature);

        const CEntitySignature& requireSignature() const;
        const CEntitySignature& rejectSignature() const;

        const std::unordered_set<entityslot_t>& pendingIn() const;
        const std::unordered_set<entityslot_t>& pendingOut() const;

        void enqueueEntity( entityslot_t entitySlot );
        void dequeueEntity( entityslot_t entitySlot );
        bool hasQueuedEntities() const;

        SEntityQueryUpdateInfo updateReceiver(std::vector<entityslot_t>& receiver);
        
        bool testSignature(const CEntitySignature& signature) const;
    };

} // namespace chestnut::ecs::internal


#include "entity_query_guard.inl"