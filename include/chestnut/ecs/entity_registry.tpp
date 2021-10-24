namespace chestnut::ecs::internal
{
    inline entityid CEntityRegistry::entityIdFromIndex( entityid index ) const
    {
        return index + ENTITY_ID_MINIMAL;
    }

    inline entityid CEntityRegistry::indexFromEntityId( entityid id ) const
    {
        return id - ENTITY_ID_MINIMAL;
    }

    template< typename SignaturePredicate >
    std::vector< entityid > CEntityRegistry::findEntities( SignaturePredicate pred ) const
    {
        std::vector< entityid > ids;

        for (entityid i = 0; i < m_dequeEntityRecords.size(); i++)
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
