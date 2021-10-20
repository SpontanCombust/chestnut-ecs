#ifndef __CHESTNUT_ECS_ENTITY_REGISTRY_H__
#define __CHESTNUT_ECS_ENTITY_REGISTRY_H__

#include "types.hpp"
#include "constants.hpp"
#include "entity_signature.hpp"

#include <deque>
#include <vector>

namespace chestnut::ecs::internal
{
    struct SEntityRegistryRecord
    {
        bool isIdUsed;
        bool isTemplate;
        CEntitySignature signature;
    };

    class CEntityRegistry
    {
    private:
        entityid m_entityIdCounter;
        std::vector< entityid > m_vecRecycledEntityIDs;

    public:
        std::deque< SEntityRegistryRecord > m_vecEntityRecords; // can't do it with vector if signatures get passed outside by reference

    public:
        CEntityRegistry();


        entityid registerNewEntity( bool isTemplateEntity );

        entityid registerNewEntity( bool isTemplateEntity, const CEntitySignature& signature );


        // Ignores request if entity is not yet registered
        void updateEntity( entityid id, const CEntitySignature& newSignature );


        bool hasEntity( entityid id, bool canBeTemplateEntity = true ) const;

        bool hasTemplateEntity( entityid id ) const;


        void unregisterEntity( entityid id );


        entitysize getEntityCount() const;
        // Count only entities that have the exact same signature as given
        entitysize getEntityCountOfExactSignature( const CEntitySignature& requiredSignature ) const;
        // Count entities, which at least part of their signature is the given signature
        entitysize getEntityCountOfPartialSignature( const CEntitySignature& requiredSignaturePart ) const;


        // Throws std::invalid_argument exception if entity is not registered
        const CEntitySignature& getEntitySignature( entityid id ) const;
    };
    
} // namespace chestnut::ecs::internal


#endif // __CHESTNUT_ECS_ENTITY_REGISTRY_H__