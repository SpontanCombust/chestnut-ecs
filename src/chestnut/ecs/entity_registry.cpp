#include "chestnut/ecs/entity_registry.hpp"

#include <exception> // invalid_argument

namespace chestnut::ecs::internal
{
    CEntityRegistry::CEntityRegistry() 
    {
        m_entityIdCounter = ENTITY_ID_MINIMAL;
    }

    inline entityid entityIdFromIndex( entityid index )
    {
        return index + ENTITY_ID_MINIMAL;
    }

    inline entityid indexFromEntityId( entityid id )
    {
        return id - ENTITY_ID_MINIMAL;
    }

    entityid CEntityRegistry::registerNewEntity( bool isTemplateEntity ) 
    {
        entityid id;

        if( !m_vecRecycledEntityIDs.empty() )
        {
            id = m_vecRecycledEntityIDs.back();
            m_vecRecycledEntityIDs.pop_back();

            entityid idx = indexFromEntityId(id);
            m_vecEntityRecords[idx].isIdUsed = true;
            m_vecEntityRecords[idx].isTemplate = isTemplateEntity;
            // signature remains default
        }
        else
        {
            SEntityRegistryRecord record;
            record.isIdUsed = true;
            record.isTemplate = isTemplateEntity;
            // signature remains default

            m_vecEntityRecords.push_back( record );

            id = entityIdFromIndex( m_vecEntityRecords.size() - 1 );
        }

        return id;
    }

    entityid CEntityRegistry::registerNewEntity( bool isTemplateEntity, const CEntitySignature& signature ) 
    {
        entityid id;
        
        if( !m_vecRecycledEntityIDs.empty() )
        {
            id = m_vecRecycledEntityIDs.back();
            m_vecRecycledEntityIDs.pop_back();

            entityid idx = indexFromEntityId(id);
            m_vecEntityRecords[idx].isIdUsed = true;
            m_vecEntityRecords[idx].isTemplate = isTemplateEntity;
            m_vecEntityRecords[idx].signature = signature;
        }
        else
        {
            SEntityRegistryRecord record;
            record.isIdUsed = true;
            record.isTemplate = isTemplateEntity;
            record.signature = signature;

            m_vecEntityRecords.push_back( record );

            id = entityIdFromIndex( m_vecEntityRecords.size() - 1 );
        }

        return id;
    }

    void CEntityRegistry::updateEntity( entityid id, const CEntitySignature& newSignature ) 
    {
        if( hasEntity( id, true ) )
        {
            m_vecEntityRecords[ indexFromEntityId(id) ].signature = newSignature;
        }
    }

    bool CEntityRegistry::hasEntity( entityid id, bool canBeTemplateEntity ) const
    {
        if( id == ENTITY_ID_INVALID )
        {
            return false;
        }
        
        id = indexFromEntityId(id);

        if( id < m_vecEntityRecords.size() && m_vecEntityRecords[id].isIdUsed )
        {
            if( !canBeTemplateEntity )
            {
                return !m_vecEntityRecords[id].isTemplate;
            }

            return true;
        }

        return false;
    }

    bool CEntityRegistry::hasTemplateEntity( entityid id ) const
    {
        if( id == ENTITY_ID_INVALID )
        {
            return false;
        }

        id = indexFromEntityId(id);

        if( id < m_vecEntityRecords.size() && m_vecEntityRecords[id].isIdUsed )
        {
            return m_vecEntityRecords[id].isTemplate;
        }

        return false;
    }

    void CEntityRegistry::unregisterEntity( entityid id ) 
    {
        if( id == ENTITY_ID_INVALID )
        {
            return;
        }

        entityid idx = indexFromEntityId(id);

        if( idx < m_vecEntityRecords.size() )
        {
            m_vecEntityRecords[idx].isIdUsed = false;
            m_vecEntityRecords[idx].isTemplate = false; 
            m_vecEntityRecords[idx].signature.clear();

            m_vecRecycledEntityIDs.push_back(id);
        }
    }

    entitysize CEntityRegistry::getEntityCount() const
    {
        return m_vecEntityRecords.size() - m_vecRecycledEntityIDs.size();
    }

    entitysize CEntityRegistry::getEntityCountOfExactSignature( const CEntitySignature& requiredSignature ) const
    {
        const entitysize size = m_vecEntityRecords.size();
        
        entitysize count = 0;
        for (entityid i = 0; i < size; i++)
        {
            if( m_vecEntityRecords[i].isIdUsed && m_vecEntityRecords[i].signature == requiredSignature )
            {
                count++;
            }
        }
        
        return count;
    }

    entitysize CEntityRegistry::getEntityCountOfPartialSignature( const CEntitySignature& requiredSignaturePart ) const
    {
        const entitysize size = m_vecEntityRecords.size();
        
        entitysize count = 0;
        for (entityid i = 0; i < size; i++)
        {
            if( m_vecEntityRecords[i].isIdUsed && m_vecEntityRecords[i].signature.has( requiredSignaturePart ) )
            {
                count++;
            }
        }
        
        return count;
    }

    const CEntitySignature& CEntityRegistry::getEntitySignature( entityid id ) const
    {
        if( id == ENTITY_ID_INVALID )
        {
            throw std::invalid_argument( "Entity has invalid ID!" );
        }

        id = indexFromEntityId(id);

        if( id < m_vecEntityRecords.size() && m_vecEntityRecords[id].isIdUsed )
        {
            return m_vecEntityRecords[id].signature;
        }
        else
        {
            throw std::invalid_argument( "Entity " + std::to_string( entityIdFromIndex(id) ) + " is not registered!" );
        }
    }

} // namespace chestnut::ecs::internal
