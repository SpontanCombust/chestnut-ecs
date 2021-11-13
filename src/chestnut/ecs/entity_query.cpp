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





    void CEntityQuery::reorderData() 
    {
        entitysize_t entCount = getEntityCount();

        m_sort_reorderedEntityIDs.clear();
        m_sort_reorderedEntityIDs.resize( entCount );
        for (entityid_t i = 0; i < entCount; i++)
        {
            m_sort_reorderedEntityIDs[i] = m_vecEntityIDs[ m_sort_orderedIndices[i] ];
        }
        m_vecEntityIDs = m_sort_reorderedEntityIDs;
        
        m_sort_reorderedComponents.clear();
        m_sort_reorderedComponents.resize( entCount );
        for( auto& [ type, vecComps ] : m_mapCompTypeToVecComp )
        {
            for (entityid_t i = 0; i < entCount; i++)
            {
                m_sort_reorderedComponents[i] = vecComps[ m_sort_orderedIndices[i] ];
            }
            vecComps = m_sort_reorderedComponents;
        }    
    }

} // namespace chestnut::ecs
