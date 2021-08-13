#include "chestnut/ecs/entity_registry.hpp"

#include <exception>

namespace chestnut::internal
{    
    void CEntityRegistry::registerEntity( entityid id, bool isTemplateEntity ) 
    {
        if( !hasEntity( id ) )
        {
            SEntityRegistryRecord record;
            record.id = id;
            record.isTemplate = isTemplateEntity;
            // we leave signature empty

            m_mapEntityIDToEntityRecord[ id ] = record;
        }
    }

    void CEntityRegistry::registerEntity( entityid id, bool isTemplateEntity, const CEntitySignature& signature ) 
    {
        if( !hasEntity( id ) )
        {
            SEntityRegistryRecord record;
            record.id = id;
            record.isTemplate = isTemplateEntity;
            record.signature = signature;

            m_mapEntityIDToEntityRecord[ id ] = record;
        }
    }

    void CEntityRegistry::updateEntity( entityid id, const CEntitySignature& newSignature ) 
    {
        auto it = m_mapEntityIDToEntityRecord.find( id );
        if( it != m_mapEntityIDToEntityRecord.end() )
        {
            it->second.signature = newSignature;
        }
    }

    void CEntityRegistry::removeEntity( entityid id ) 
    {
        m_mapEntityIDToEntityRecord.erase( id );
    }

    void CEntityRegistry::removeAllEntities() 
    {
        m_mapEntityIDToEntityRecord.clear();
    }

    bool CEntityRegistry::hasEntity( entityid id, bool canBeTemplateEntity ) const
    {
        auto it = m_mapEntityIDToEntityRecord.find( id );

        // first find if the ID is even in the registry
        if( it != m_mapEntityIDToEntityRecord.end() )
        {
            // entity is registered

            // should accept it if it's a template entity?
            if( !canBeTemplateEntity )
            {
                // if it exists and can't be a template entity then just check if it isn't registered as template
                return !it->second.isTemplate;
            }
            // if we can ignore the type then it's valid in any case
            else
            {
                return true;
            }   
        }
        else
        {
            // no ID found, entity not registered at all
            return false;
        }
    }

    bool CEntityRegistry::hasTemplateEntity( entityid id ) const
    {
        auto it = m_mapEntityIDToEntityRecord.find( id );

        if( it != m_mapEntityIDToEntityRecord.end() )
        {
            return it->second.isTemplate;
        }
        else
        {
            return false;
        }
    }

    entityid CEntityRegistry::getEntityCount() const
    {
        return m_mapEntityIDToEntityRecord.size();
    }

    const CEntitySignature& CEntityRegistry::getEntitySignature( entityid id ) const
    {
        auto it = m_mapEntityIDToEntityRecord.find( id );
        if( it != m_mapEntityIDToEntityRecord.end() )
        {
            return it->second.signature;
        }
        else
        {
            throw std::invalid_argument( "Entity " + std::to_string(id) + " is not registered!" );
        }
    }

} // namespace chestnut
