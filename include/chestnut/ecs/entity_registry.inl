#include <exception> // invalid_argument

namespace chestnut::ecs::internal
{
    inline entityid_t CEntityRegistry::entityIdFromIndex( entityid_t index ) const
    {
        return index + ENTITY_ID_MINIMAL;
    }

    inline entityid_t CEntityRegistry::indexFromEntityId( entityid_t id ) const
    {
        return id - ENTITY_ID_MINIMAL;
    }

    inline CEntityRegistry::CEntityRegistry() 
    {
        m_entityIdCounter = ENTITY_ID_MINIMAL;
    }

    inline entityid_t CEntityRegistry::registerNewEntity( bool isTemplateEntity ) 
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

    inline entityid_t CEntityRegistry::registerNewEntity( bool isTemplateEntity, const CEntitySignature& signature ) 
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

    inline void CEntityRegistry::updateEntity( entityid_t id, const CEntitySignature& newSignature ) 
    {
        if( hasEntity( id, CAN_BE_REGULAR_ENTITY | CAN_BE_TEMPLATE_ENTITY ) )
        {
            m_dequeEntityRecords[ indexFromEntityId(id) ].signature = newSignature;
        }
    }

    inline bool flagEq( int val, int flags )
    {
        return ( val & flags ) == flags;
    }

    inline bool recordMatchesSearchFlags( const SEntityRegistryRecord& record, int flags )
    {
        bool result = false;

        result |= flagEq( flags, CEntityRegistry::CAN_BE_REGULAR_ENTITY ) && !record.isTemplate;
        result |= flagEq( flags, CEntityRegistry::CAN_BE_TEMPLATE_ENTITY ) && record.isTemplate;
        
        return result;
    }

    inline bool CEntityRegistry::hasEntity( entityid_t id, int searchFlags ) const
    {
        if( id == ENTITY_ID_INVALID )
        {
            return false;
        }
        
        id = indexFromEntityId(id);

        if( id < m_dequeEntityRecords.size() && m_dequeEntityRecords[id].isIdUsed )
        {
            return recordMatchesSearchFlags( m_dequeEntityRecords[id], searchFlags );
        }

        return false;
    }

    inline void CEntityRegistry::unregisterEntity( entityid_t id ) 
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

    inline entitysize_t CEntityRegistry::getEntityCount( int searchFlags ) const
    {
        if( flagEq( searchFlags, CAN_BE_REGULAR_ENTITY | CAN_BE_TEMPLATE_ENTITY ) )
        {
            return m_dequeEntityRecords.size() - m_vecRecycledEntityIDs.size();
        }
        else
        {
            const entitysize_t size = m_dequeEntityRecords.size();
            entitysize_t count = 0;

            for (entityid_t i = 0; i < size; i++)
            {
                const auto& record = m_dequeEntityRecords[i];
                if( record.isIdUsed && recordMatchesSearchFlags( record, searchFlags ) )
                {
                    count++;
                }
            }

            return count;
        }
    }

    inline entitysize_t CEntityRegistry::getEntityCountOfExactSignature( const CEntitySignature& requiredSignature, int searchFlags ) const
    {
        const entitysize_t size = m_dequeEntityRecords.size();
        
        entitysize_t count = 0;
        for (entityid_t i = 0; i < size; i++)
        {
            const auto& record = m_dequeEntityRecords[i];
            if( record.isIdUsed && recordMatchesSearchFlags( record, searchFlags ) && record.signature == requiredSignature )
            {
                count++;
            }
        }
        
        return count;
    }

    inline entitysize_t CEntityRegistry::getEntityCountOfPartialSignature( const CEntitySignature& requiredSignaturePart, int searchFlags ) const
    {
        const entitysize_t size = m_dequeEntityRecords.size();
        
        entitysize_t count = 0;
        for (entityid_t i = 0; i < size; i++)
        {
            const auto& record = m_dequeEntityRecords[i];
            if( record.isIdUsed && recordMatchesSearchFlags( record, searchFlags ) && record.signature.hasAllFrom( requiredSignaturePart ) )
            {
                count++;
            }
        }
        
        return count;
    }

    inline const CEntitySignature* CEntityRegistry::getEntitySignature( entityid_t id ) const
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
    
    inline std::vector<entityid_t> CEntityRegistry::findEntities( std::function< bool( const CEntitySignature& ) > pred, int searchFlags ) const
    {
        std::vector< entityid_t > ids;

        for (entityid_t i = 0; i < m_dequeEntityRecords.size(); i++)
        {
            const auto& record = m_dequeEntityRecords[i];
            if( record.isIdUsed && recordMatchesSearchFlags( record, searchFlags ) )   
            {
                if( pred( record.signature ) )
                {
                    ids.push_back( entityIdFromIndex(i) );
                }
            }
        }

        return ids;
    }

} // namespace chestnut::ecs::internal
