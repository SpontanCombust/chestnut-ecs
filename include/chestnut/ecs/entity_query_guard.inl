#include <iterator> // next

namespace chestnut::ecs::internal
{    
    inline CEntityQueryGuard::CEntityQueryGuard( queryid_t id, const CEntitySignature& requireSignature, const CEntitySignature& rejectSignature, CComponentStorageTypeMap *storageMapPtr )
    : m_targetQuery( id )
    {
        m_storageMapPtr = storageMapPtr;

        m_requireSignature = requireSignature;
        m_rejectSignature = rejectSignature;

        // initialize batch's component map
        for( std::type_index type : requireSignature.m_setComponentTypes )
        {
            m_targetQuery.m_mapCompTypeToVecComp[ type ] = std::vector< IComponentWrapper * >();
        }
    }

    inline void CEntityQueryGuard::fetchAndAddEntityWithComponents( entityid_t entityID ) 
    {
        // First of all check if entity has been scheduled for removal
        // In that case only delete removal data
        // We can assume entity data already was in the batch before scheduled removal
        // Don't allow stuff to be removed in the first place if it is to be added back in

        auto it = m_pendingOut_setEntityIDs.find( entityID );

        if( it != m_pendingOut_setEntityIDs.end() )
        {
            m_pendingOut_setEntityIDs.erase( it );
        }
        // Otherwise fetch and add data
        else
        {
            m_pendingIn_vecEntityIDs.push_back( entityID );

            for( std::type_index type : m_requireSignature.m_setComponentTypes )
            {
                // This will throw std::out_of_range if systems using this class don't assure the type exists in storage map
                IComponentStorage *typedStorage = m_storageMapPtr->at( type );

                IComponentWrapper *comp = typedStorage->getComponent( entityID );

                m_pendingIn_mapCompTypeToVecComp[ type ].push_back( comp );       
            } 
        }
    }

    inline void CEntityQueryGuard::removeEntityWithComponents( entityid_t entityID ) 
    {
        // First check if entity has been scheduled for addition
        // In that case delete addition data
        // Don't allow stuff to be added in the first place if it is to be removed later on anyways

        // do a for loop search, because we'll need the index
        entityid_t i;
        for (i = 0; i < m_pendingIn_vecEntityIDs.size(); i++)
        {
            if( m_pendingIn_vecEntityIDs[i] == entityID )
            {
                break;
            }
        }

        if( i < m_pendingIn_vecEntityIDs.size() )
        {
            // use std::next to erase using index
            m_pendingIn_vecEntityIDs.erase( std::next( m_pendingIn_vecEntityIDs.begin(), i ) );

            for( auto& [ type, vecComp ] : m_pendingIn_mapCompTypeToVecComp )
            {
                // components in these mapped vectors are at the same index as they are
                // in the vector of entity IDs, because we add them in the same order
                vecComp.erase( std::next( vecComp.begin(), i ) );
            }
        }
        // if the entity wasn't to be added, then maybe it already is in the batch - schedule it for removal
        else
        {
            m_pendingOut_setEntityIDs.insert( entityID );
        }
    }

    inline bool CEntityQueryGuard::updateQuery() 
    {
        // first do the removal

        if( !m_pendingOut_setEntityIDs.empty() )
        {
            // iterate over all IDs in the target batch vector of entity IDs
            // remove those that are in the set of IDs sheduled for removal 

            std::vector< entityid_t > vecIndicesToEraseInOrder;

            for (entityid_t i = 0; i < m_targetQuery.m_vecEntityIDs.size(); /*NOP*/)
            {
                entityid_t entityID = m_targetQuery.m_vecEntityIDs[i];

                if( m_pendingOut_setEntityIDs.find( entityID ) != m_pendingOut_setEntityIDs.end() )
                {
                    m_targetQuery.m_vecEntityIDs.erase( std::next( m_targetQuery.m_vecEntityIDs.begin(), i ) );
                    vecIndicesToEraseInOrder.push_back(i);
                }
                else
                {
                    i++;
                }
            }

            for( auto& [ type, vecComp ] : m_targetQuery.m_mapCompTypeToVecComp )
            {
                for (entityid_t i = 0; i < vecIndicesToEraseInOrder.size(); i++)
                {
                    vecComp.erase( std::next( vecComp.begin(), vecIndicesToEraseInOrder[i] ) );
                }
            }
        }
        


        // then addition

        if( !m_pendingIn_vecEntityIDs.empty() )
        {
            // add to target batch all the IDs of entities of pending components
            m_targetQuery.m_vecEntityIDs.insert( m_targetQuery.m_vecEntityIDs.end(),
                                                 m_pendingIn_vecEntityIDs.begin(),
                                                 m_pendingIn_vecEntityIDs.end() );

            // now add the actual components to target batch
            for( auto& [ type, vecComp ] : m_targetQuery.m_mapCompTypeToVecComp )
            {
                vecComp.insert( vecComp.end(),
                                m_pendingIn_mapCompTypeToVecComp[ type ].begin(), 
                                m_pendingIn_mapCompTypeToVecComp[ type ].end() );
            }
        }



        // clear pending data

        m_pendingOut_setEntityIDs.clear();

        m_pendingIn_vecEntityIDs.clear();

        for( auto& [ type, vecComp ] : m_pendingIn_mapCompTypeToVecComp )
        {
            vecComp.clear();
        }


        // return whether the target batch has any components now
        return m_targetQuery.m_vecEntityIDs.size() > 0;
    }

    inline bool CEntityQueryGuard::testQuery( const CEntitySignature& signature ) const
    {
        return signature.hasAllFrom( m_requireSignature ) && !signature.hasAnyFrom( m_rejectSignature );
    }

    inline const CEntityQuery* CEntityQueryGuard::getQuery() const
    {
        return &m_targetQuery;
    }

    inline CEntityQuery* CEntityQueryGuard::getQuery() 
    {
        return &m_targetQuery;
    }

} // namespace chestnut::ecs::internal
