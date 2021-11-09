#ifndef __CHESTNUT_ECS_ENTITY_REGISTRY_H__
#define __CHESTNUT_ECS_ENTITY_REGISTRY_H__

#include "types.hpp"
#include "constants.hpp"
#include "entity_signature.hpp"

#include <deque>
#include <functional>
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
        entityid_t m_entityIdCounter;
        std::vector< entityid_t > m_vecRecycledEntityIDs;

    public:
        std::deque< SEntityRegistryRecord > m_dequeEntityRecords; // can't do it with vector if signatures get passed outside by reference

    public:
        CEntityRegistry();


        entityid_t registerNewEntity( bool isTemplateEntity );

        entityid_t registerNewEntity( bool isTemplateEntity, const CEntitySignature& signature );


        // Ignores request if entity is not yet registered
        void updateEntity( entityid_t id, const CEntitySignature& newSignature );


        bool hasEntity( entityid_t id, bool canBeTemplateEntity = true ) const;

        bool hasTemplateEntity( entityid_t id ) const;


        void unregisterEntity( entityid_t id );


        entitysize_t getEntityCount() const;
        // Count only entities that have the exact same signature as given
        entitysize_t getEntityCountOfExactSignature( const CEntitySignature& requiredSignature ) const;
        // Count entities, which at least part of their signature is the given signature
        entitysize_t getEntityCountOfPartialSignature( const CEntitySignature& requiredSignaturePart ) const;


        // Throws std::invalid_argument exception if entity is not registered
        const CEntitySignature& getEntitySignature( entityid_t id ) const;

        // Doesn't consider template entities
        std::vector< entityid_t > findEntities( std::function< bool( const CEntitySignature& ) > pred ) const;


    private:
        entityid_t entityIdFromIndex( entityid_t index ) const;

        entityid_t indexFromEntityId( entityid_t id ) const;
    };
    
} // namespace chestnut::ecs::internal


#include "entity_registry.tpp"


#endif // __CHESTNUT_ECS_ENTITY_REGISTRY_H__