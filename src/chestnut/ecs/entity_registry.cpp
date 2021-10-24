#include "chestnut/ecs/entity_registry.hpp"

#include <exception> // invalid_argument

namespace chestnut::ecs::internal
{
    CEntityRegistry::CEntityRegistry() 
    {
        m_entityIdCounter = ENTITY_ID_MINIMAL;
    }

    entityid CEntityRegistry::registerNewEntity( bool isTemplateEntity ) 
    {
        entityid id;

        if( !m_vecRecycledEntityIDs.empty() )
        {
            id = m_vecRecycledEntityIDs.back();
            m_vecRecycledEntityIDs.pop_back();

            entityid idx = indexFromEntityId(id);
            m_dequeEntityRecords[idx].isIdUsed = true;
            m_dequeEntityRecords[idx].isTemplate = isTemplateEntity;
            // signature remains default
        }
        else
        {
            SEntityRegistryRecord record;
            record.isIdUsed = true;
            record.isTemplate = isTemplateEntity;
            // signature remains default

            m_dequeEntityRecords.push_back( record );

            id = entityIdFromIndex( m_dequeEntityRecords.size() - 1 );
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
            m_dequeEntityRecords[idx].isIdUsed = true;
            m_dequeEntityRecords[idx].isTemplate = isTemplateEntity;
            m_dequeEntityRecords[idx].signature = signature;
        }
        else
        {
            SEntityRegistryRecord record;
            record.isIdUsed = true;
            record.isTemplate = isTemplateEntity;
            record.signature = signature;

            m_dequeEntityRecords.push_back( record );

            id = entityIdFromIndex( m_dequeEntityRecords.size() - 1 );
        }

        return id;
    }

    void CEntityRegistry::updateEntity( entityid id, const CEntitySignature& newSignature ) 
    {
        if( hasEntity( id, true ) )
        {
            m_dequeEntityRecords[ indexFromEntityId(id) ].signature = newSignature;
        }
    }

    bool CEntityRegistry::hasEntity( entityid id, bool canBeTemplateEntity ) const
    {
        if( id == ENTITY_ID_INVALID )
        {
            return false;
        }
        
        id = indexFromEntityId(id);

        if( id < m_dequeEntityRecords.size() && m_dequeEntityRecords[id].isIdUsed )
        {
            if( !canBeTemplateEntity )
            {
                return !m_dequeEntityRecords[id].isTemplate;
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

        if( id < m_dequeEntityRecords.size() && m_dequeEntityRecords[id].isIdUsed )
        {
            return m_dequeEntityRecords[id].isTemplate;
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

        if( idx < m_dequeEntityRecords.size() )
        {
            m_dequeEntityRecords[idx].isIdUsed = false;
            m_dequeEntityRecords[idx].isTemplate = false; 
            m_dequeEntityRecords[idx].signature.clear();

            m_vecRecycledEntityIDs.push_back(id);
        }
    }

    entitysize CEntityRegistry::getEntityCount() const
    {
        return m_dequeEntityRecords.size() - m_vecRecycledEntityIDs.size();
    }

    entitysize CEntityRegistry::getEntityCountOfExactSignature( const CEntitySignature& requiredSignature ) const
    {
        const entitysize size = m_dequeEntityRecords.size();
        
        entitysize count = 0;
        for (entityid i = 0; i < size; i++)
        {
            if( m_dequeEntityRecords[i].isIdUsed && m_dequeEntityRecords[i].signature == requiredSignature )
            {
                count++;
            }
        }
        
        return count;
    }

    entitysize CEntityRegistry::getEntityCountOfPartialSignature( const CEntitySignature& requiredSignaturePart ) const
    {
        const entitysize size = m_dequeEntityRecords.size();
        
        entitysize count = 0;
        for (entityid i = 0; i < size; i++)
        {
            if( m_dequeEntityRecords[i].isIdUsed && m_dequeEntityRecords[i].signature.hasAllFrom( requiredSignaturePart ) )
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

        if( id < m_dequeEntityRecords.size() && m_dequeEntityRecords[id].isIdUsed )
        {
            return m_dequeEntityRecords[id].signature;
        }
        else
        {
            throw std::invalid_argument( "Entity " + std::to_string( entityIdFromIndex(id) ) + " is not registered!" );
        }
    }

} // namespace chestnut::ecs::internal
