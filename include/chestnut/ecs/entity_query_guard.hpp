#pragma once

#include "component_wrapper.hpp"
#include "component_storage.hpp"
#include "entity_query.hpp"

#include <unordered_set>

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
        std::unordered_set< entityid_t > m_pendingIn_setEntityIDs;
        std::unordered_set< entityid_t > m_pendingOut_setEntityIDs;

        CEntityQuery m_targetQuery;


    public:
        CEntityQueryGuard(CComponentStorage *componentStorage, const CEntitySignature& requireSignature, const CEntitySignature& rejectSignature);


        // Doesn't check for duplicates
        void enqueueEntity( entityid_t entityID );
        void dequeueEntity( entityid_t entityID );

        // Returns whether the underlying query has any components stored inside it after the update
        bool updateQuery();

        bool testQuery( const CEntitySignature& signature ) const;

        const CEntityQuery& getQuery() const;
        CEntityQuery& getQuery();
    };

} // namespace chestnut::ecs::internal


#include "entity_query_guard.inl"