#include "chestnut/ecs/entity_query.hpp"

namespace chestnut::ecs
{
    CEntityQuery::CEntityQuery( queryid id ) 
    {
        m_id = id;
    }

    queryid CEntityQuery::getID() const
    {
        return m_id;
    }

    entitysize CEntityQuery::getEntityCount() const
    {
        return m_vecEntityIDs.size();
    }

    void CEntityQuery::forEachEntityWith( std::function< void( entityid ) > func ) const
    {
        if( m_vecEntityIDs.empty() )
        {
            return;
        }

        const entityid entCount = m_vecEntityIDs.size();

        for (entityid i = 0; i < entCount; i++)
        {
            func( m_vecEntityIDs[i] );
        }
    }
    
    void CEntityQuery::forEachEntityPairWith( std::function< void( entityid, entityid ) > func ) const
    {
        if( m_vecEntityIDs.empty() )
        {
            return;
        }

        const entityid entCount = m_vecEntityIDs.size();

        for (entityid i = 0; i < entCount; i++)
        {
            for (entityid j = i; j < entCount; j++)
            {
                func( m_vecEntityIDs[i], m_vecEntityIDs[j] );   
            }
        }
    }

} // namespace chestnut::ecs
