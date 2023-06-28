#pragma once

#include "component_storage.hpp"
#include "entity_query.hpp"

#include <unordered_set>

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
        std::unordered_set< entityslot_t > m_pendingIn_setEntitySlots;
        std::unordered_set< entityslot_t > m_pendingOut_setEntitySlots;

        CEntityQuery m_targetQuery;


    public:
        CEntityQueryGuard(CComponentStorage *componentStorage, const CEntitySignature& requireSignature, const CEntitySignature& rejectSignature);


        // Doesn't check for duplicates
        void enqueueEntity( entityslot_t entitySlot );
        void dequeueEntity( entityslot_t entitySlot );

        // Returns whether the content of the query changed after the update
        SEntityQueryUpdateInfo updateQuery();

        bool testQuery( const CEntitySignature& signature ) const;

        const CEntityQuery& getQuery() const;
        CEntityQuery& getQuery();
    };

} // namespace chestnut::ecs::internal


#include "entity_query_guard.inl"