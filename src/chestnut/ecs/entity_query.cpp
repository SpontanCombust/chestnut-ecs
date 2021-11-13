#include "chestnut/ecs/entity_query.hpp"

namespace chestnut::ecs
{
    CEntityQuery::CEntityQuery( queryid_t id ) 
    {
        m_id = id;
    }

    queryid_t CEntityQuery::getID() const
    {
        return m_id;
    }

    entitysize_t CEntityQuery::getEntityCount() const
    {
        return m_vecEntityIDs.size();
    }





    void CEntityQuery::forEachEntityWith( std::function< void( entityid_t ) > func ) const
    {
        if( m_vecEntityIDs.empty() )
        {
            return;
        }

        const entityid_t entCount = m_vecEntityIDs.size();

        for (entityid_t i = 0; i < entCount; i++)
        {
            func( m_vecEntityIDs[i] );
        }
    }
    
    void CEntityQuery::forEachEntityPairWith( std::function< void( entityid_t, entityid_t ) > func ) const
    {
        if( m_vecEntityIDs.empty() )
        {
            return;
        }

        const entityid_t entCount = m_vecEntityIDs.size();

        for (entityid_t i = 0; i < entCount; i++)
        {
            for (entityid_t j = i; j < entCount; j++)
            {
                func( m_vecEntityIDs[i], m_vecEntityIDs[j] );   
            }
        }
    }





    void CEntityQuery::reorderData( const std::vector< entityid_t >& orderedIndices ) 
    {
        entitysize_t entCount = getEntityCount();

        std::vector< entityid_t > reorderedEntityIDs( entCount );
        for (entityid_t i = 0; i < entCount; i++)
        {
            reorderedEntityIDs[i] = m_vecEntityIDs[ orderedIndices[i] ];
        }
        m_vecEntityIDs = reorderedEntityIDs;
        
        std::vector< internal::IComponentWrapper* > reorderedComponents( entCount );
        for( auto& [ type, vecComps ] : m_mapCompTypeToVecComp )
        {
            for (entityid_t i = 0; i < entCount; i++)
            {
                reorderedComponents[i] = vecComps[ orderedIndices[i] ];
            }
            vecComps = reorderedComponents;
        }    
    }

} // namespace chestnut::ecs
