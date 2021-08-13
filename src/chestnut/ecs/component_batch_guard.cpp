#include "chestnut/ecs/component_batch_guard.hpp"

#include <algorithm>

namespace chestnut::internal
{    
    CComponentBatchGuard::CComponentBatchGuard( const CEntitySignature& signature ) 
    {
        m_targetBatch.signature = signature;

        // initialize batch's component map
        for( std::type_index type : signature.m_setComponentTypes )
        {
            m_targetBatch.mapCompTypeToCompVec[ type ] = std::vector< CComponent * >();
        }
    }

    void CComponentBatchGuard::fetchAndAddEntityWithComponents( entityid entityID, const CComponentStorageTypeMap& storageMap ) 
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

            for( std::type_index type : m_targetBatch.signature.m_setComponentTypes )
            {
                // This ~~shouldn't~~ throw an exception if systems using this class are setup correctly
                IComponentStorage *typedStorage = storageMap.at( type );

                CComponent *comp = typedStorage->getComponent( entityID );

                m_pendingIn_mapCompTypeToVecComp[ type ].push_back( comp );       
            } 
        }
    }

    void CComponentBatchGuard::removeEntityWithComponents( entityid entityID ) 
    {
        // First check if entity has been scheduled for addition
        // In that case delete addition data
        // Don't allow stuff to be added in the first place if it is to be removed later on anyways

        // do a for loop search, because we'll need the index
        entityid i;
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

    bool CComponentBatchGuard::updateBatch() 
    {
        // first do the removal

        if( !m_pendingOut_setEntityIDs.empty() )
        {
            // iterate over all IDs in the target batch vector of entity IDs
            // remove those that are in the set of IDs sheduled for removal 

            std::vector< entityid > vecIndicesToEraseInOrder;

            for (entityid i = 0; i < m_targetBatch.vecEntityIDs.size(); /*NOP*/)
            {
                entityid entityID = m_targetBatch.vecEntityIDs[i];

                if( m_pendingOut_setEntityIDs.find( entityID ) != m_pendingOut_setEntityIDs.end() )
                {
                    m_targetBatch.vecEntityIDs.erase( std::next( m_targetBatch.vecEntityIDs.begin(), i ) );
                    vecIndicesToEraseInOrder.push_back(i);
                }
                else
                {
                    i++;
                }
            }

            for( auto& [ type, vecComp ] : m_targetBatch.mapCompTypeToCompVec )
            {
                for( entityid index : vecIndicesToEraseInOrder )
                {
                    vecComp.erase( std::next( vecComp.begin(), index ) );
                }
            }
        }
        


        // then addition

        if( !m_pendingIn_vecEntityIDs.empty() )
        {
            // add to target batch all the IDs of entities of pending components
            m_targetBatch.vecEntityIDs.insert( m_targetBatch.vecEntityIDs.end(),
                                               m_pendingIn_vecEntityIDs.begin(),
                                               m_pendingIn_vecEntityIDs.end() );

            // now add the actual components to target batch
            for( auto& [ type, vecComp ] : m_targetBatch.mapCompTypeToCompVec )
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
        return m_targetBatch.vecEntityIDs.size() > 0;
    }

    bool CComponentBatchGuard::hasAnyComponentsInBatch() const
    {
        return m_targetBatch.vecEntityIDs.size() > 0;
    }

    const CEntitySignature& CComponentBatchGuard::getBatchSignature() const
    {
        return m_targetBatch.signature;
    }

    const SComponentBatch* CComponentBatchGuard::getBatchPtr() const
    {
        return &m_targetBatch;
    }


} // namespace chestnut::internal
