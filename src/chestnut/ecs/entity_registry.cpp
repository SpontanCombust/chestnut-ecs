#include "chestnut/ecs/entity_registry.hpp"

#include <exception>

namespace chestnut::internal
{    
    SEntityRegistryRecord::SEntityRegistryRecord( entityid _id ) 
    {
        id = _id;
    }

    SEntityRegistryRecord::SEntityRegistryRecord( entityid _id, const CEntitySignature& _signature ) 
    {
        id = _id;
        signature = _signature;
    }



    void CEntityRegistry::registerEntity( entityid id ) 
    {
        if( !hasEntity( id ) )
        {
            m_mapEntityIDToEntityRecord[ id ] = SEntityRegistryRecord( id );
        }
    }

    void CEntityRegistry::registerEntity( entityid id, const CEntitySignature& signature ) 
    {
        if( !hasEntity( id ) )
        {
            m_mapEntityIDToEntityRecord[ id ] = SEntityRegistryRecord( id, signature );
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

    bool CEntityRegistry::hasEntity( entityid id ) const
    {
        auto it = m_mapEntityIDToEntityRecord.find( id );
        if( it != m_mapEntityIDToEntityRecord.end() )
        {
            return true;
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
