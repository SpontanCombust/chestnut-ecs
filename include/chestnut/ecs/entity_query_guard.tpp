#include <algorithm> //stable_sort
#include <cstring> //memcpy
#include <numeric> //iota

namespace chestnut::ecs::internal
{
    template<class C>
    void CEntityQueryGuard::sortQuery( std::function< bool( const C&, const C& ) > compare ) 
    {
        auto it = m_targetQuery.m_mapCompTypeToVecComp.find( typeid( C ) );
        if( it == m_targetQuery.m_mapCompTypeToVecComp.end() )
        {
            return;
        }

        const std::vector< internal::IComponentWrapper * >& vecC = it->second;

        entitysize entCount = m_targetQuery.getEntityCount();
        if( entCount > 0 )
        {
            entityid *orderedIndices = new entityid[ entCount ];

            // fill array with indices
            std::iota( orderedIndices, orderedIndices + entCount, 0 );

            // now sort these indices 
            std::stable_sort( orderedIndices, orderedIndices + entCount, 
            [&vecC, &compare]( entityid i1, entityid i2 ) -> bool
            {
                internal::SComponentWrapper<C>* c1 = static_cast< internal::SComponentWrapper<C>* >( vecC[i1] );
                internal::SComponentWrapper<C>* c2 = static_cast< internal::SComponentWrapper<C>* >( vecC[i2] );

                return compare( c1->data, c2->data );
            });


            // based on these reordered indices we'll change the order of entity IDs and component pointers       

            std::vector< entityid > reorderedEntityIDs( entCount );
            for (entityid i = 0; i < entCount; i++)
            {
                reorderedEntityIDs[i] = m_targetQuery.m_vecEntityIDs[ orderedIndices[i] ];
            }
            m_targetQuery.m_vecEntityIDs = reorderedEntityIDs;
            
            std::vector< internal::IComponentWrapper* > reorderedComponents( entCount );
            for( auto& [ type, vecComps ] : m_targetQuery.m_mapCompTypeToVecComp )
            {
                for (entityid i = 0; i < entCount; i++)
                {
                    reorderedComponents[i] = vecComps[ orderedIndices[i] ];
                }
                vecComps = reorderedComponents;
            }
            
            delete orderedIndices;
        }
    }

} // namespace chestnut::ecs::internal
