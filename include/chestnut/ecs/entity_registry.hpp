#ifndef __CHESTNUT_ENTITY_REGISTRY_H__
#define __CHESTNUT_ENTITY_REGISTRY_H__

#include "types.hpp"
#include "entity_signature.hpp"

#include <unordered_map>

namespace chestnut::internal
{
    struct SEntityRegistryRecord
    {
        entityid id;
        bool isTemplate;
        CEntitySignature signature;
    };

    class CEntityRegistry
    {
    private:
        std::unordered_map< entityid, SEntityRegistryRecord > m_mapEntityIDToEntityRecord;

    public:
        // Ignores request if entity is already registered
        void registerEntity( entityid id, bool isTemplateEntity );

        // Ignores request if entity is already registered
        void registerEntity( entityid id, bool isTemplateEntity, const CEntitySignature& signature );

        // Ignores request if entity is not yet registered
        void updateEntity( entityid id, const CEntitySignature& newSignature );

        bool hasEntity( entityid id, bool canBeTemplateEntity = true ) const;

        bool hasTemplateEntity( entityid id ) const;

        void removeEntity( entityid id );

        void removeAllEntities();

        entityid getEntityCount() const;

        // Throws std::invalid_argument exception on not finding the entity
        const CEntitySignature& getEntitySignature( entityid id ) const;
    };
    
} // namespace chestnut


#endif // __CHESTNUT_ENTITY_REGISTRY_H__