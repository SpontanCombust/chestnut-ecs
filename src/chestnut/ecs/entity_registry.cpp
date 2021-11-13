#include "chestnut/ecs/entity_registry.hpp"

#include <exception> // invalid_argument

namespace chestnut::ecs::internal
{
    CEntityRegistry::CEntityRegistry() 
    {
        m_entityIdCounter = ENTITY_ID_MINIMAL;
    }

    entityid_t CEntityRegistry::registerNewEntity( bool isTemplateEntity ) 
    {
        entityid_t id;

        if( !m_vecRecycledEntityIDs.empty() )
        {
            id = m_vecRecycledEntityIDs.back();
            m_vecRecycledEntityIDs.pop_back();

            entityid_t idx = indexFromEntityId(id);
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

    entityid_t CEntityRegistry::registerNewEntity( bool isTemplateEntity, const CEntitySignature& signature ) 
    {
        entityid_t id;
        
        if( !m_vecRecycledEntityIDs.empty() )
        {
            id = m_vecRecycledEntityIDs.back();
            m_vecRecycledEntityIDs.pop_back();

            entityid_t idx = indexFromEntityId(id);
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

    void CEntityRegistry::updateEntity( entityid_t id, const CEntitySignature& newSignature ) 
    {
        if( hasEntity( id, true ) )
        {
            m_dequeEntityRecords[ indexFromEntityId(id) ].signature = newSignature;
        }
    }

    bool CEntityRegistry::hasEntity( entityid_t id, bool canBeTemplateEntity ) const
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

    bool CEntityRegistry::hasTemplateEntity( entityid_t id ) const
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

    void CEntityRegistry::unregisterEntity( entityid_t id ) 
    {
        if( id == ENTITY_ID_INVALID )
        {
            return;
        }

        entityid_t idx = indexFromEntityId(id);

        if( idx < m_dequeEntityRecords.size() )
        {
            m_dequeEntityRecords[idx].isIdUsed = false;
            m_dequeEntityRecords[idx].isTemplate = false; 
            m_dequeEntityRecords[idx].signature.clear();

            m_vecRecycledEntityIDs.push_back(id);
        }
    }

    entitysize_t CEntityRegistry::getEntityCount() const
    {
        return m_dequeEntityRecords.size() - m_vecRecycledEntityIDs.size();
    }

    entitysize_t CEntityRegistry::getEntityCountOfExactSignature( const CEntitySignature& requiredSignature ) const
    {
        const entitysize_t size = m_dequeEntityRecords.size();
        
        entitysize_t count = 0;
        for (entityid_t i = 0; i < size; i++)
        {
            if( m_dequeEntityRecords[i].isIdUsed && m_dequeEntityRecords[i].signature == requiredSignature )
            {
                count++;
            }
        }
        
        return count;
    }

    entitysize_t CEntityRegistry::getEntityCountOfPartialSignature( const CEntitySignature& requiredSignaturePart ) const
    {
        const entitysize_t size = m_dequeEntityRecords.size();
        
        entitysize_t count = 0;
        for (entityid_t i = 0; i < size; i++)
        {
            if( m_dequeEntityRecords[i].isIdUsed && m_dequeEntityRecords[i].signature.hasAllFrom( requiredSignaturePart ) )
            {
                count++;
            }
        }
        
        return count;
    }

    const CEntitySignature* CEntityRegistry::getEntitySignature( entityid_t id ) const
    {
        if( id == ENTITY_ID_INVALID )
        {
            return nullptr;
        }

        id = indexFromEntityId(id);

        if( id < m_dequeEntityRecords.size() && m_dequeEntityRecords[id].isIdUsed )
        {
            return &m_dequeEntityRecords[id].signature;
        }
        
        return nullptr;
    }
    
    std::vector<entityid_t> CEntityRegistry::findEntities( std::function< bool( const CEntitySignature& ) > pred ) const
    {
        std::vector< entityid_t > ids;

        for (entityid_t i = 0; i < m_dequeEntityRecords.size(); i++)
        {
            if( m_dequeEntityRecords[i].isIdUsed && !m_dequeEntityRecords[i].isTemplate )   
            {
                if( pred( m_dequeEntityRecords[i].signature ) )
                {
                    ids.push_back( entityIdFromIndex(i) );
                }
            }
        }

        return ids;
    }

} // namespace chestnut::ecs::internal
