#include "chestnut/ecs/entity_world.hpp"

#include <algorithm> // find_if
#include <exception> // invalid_argument

namespace chestnut::ecs
{
    using namespace internal;

    // ========================= PUBLIC ========================= //

    CEntityWorld::CEntityWorld() 
    {
        m_idCounter = ENTITY_ID_INVALID;
    }

    CEntityWorld::~CEntityWorld() 
    {
        for( auto& [ type, storage ] : m_mapCompTypeToStorage )
        {
            delete storage;
        }
    }



    entityid CEntityWorld::createEntity() 
    {
        entityid id = getNewEntityID();
        m_entityRegistry.registerEntity( id, false );
        return id;
    }

    std::vector< entityid > CEntityWorld::createEntities( unsigned int amount ) 
    {
        std::vector< entityid > ids;

        if( amount > 0 )
        {
            ids.reserve( amount );

            for (unsigned int i = 0; i < amount; i++)
            {
                ids.push_back( createEntity() );
            }
        }

        return ids;
    }

    bool CEntityWorld::hasEntity( entityid entityID ) const
    {
        return m_entityRegistry.hasEntity( entityID, false );
    }

    void CEntityWorld::destroyEntity( entityid entityID ) 
    {
        if( hasEntity( entityID ) )
        {
            const CEntitySignature& signature = m_entityRegistry.getEntitySignature( entityID );

            if( !signature.isEmpty() )
            {
                CComponentBatchGuard& batchGuard = getBatchGuardWithSignature( signature );
                batchGuard.removeEntityWithComponents( entityID );

                for( const auto& type : signature.m_setComponentTypes )
                {
                    IComponentStorage *storage = m_mapCompTypeToStorage.at( type );
                    storage->eraseComponent( entityID );
                }
            }

            m_entityRegistry.removeEntity( entityID );

            m_vecRecycledIDs.push_back( entityID );
        }
    }

    void CEntityWorld::destroyEntities( const std::vector< entityid >& entityIDs ) 
    {
        for( entityid id : entityIDs )
        {
            destroyEntity( id );
        }
    }




    entityid CEntityWorld::createTemplateEntity() 
    {
        entityid id = getNewEntityID();
        m_entityRegistry.registerEntity( id, true );
        return id;
    }

    bool CEntityWorld::hasTemplateEntity( entityid templateEntityID ) const
    {
        return m_entityRegistry.hasTemplateEntity( templateEntityID );
    }

    void CEntityWorld::destroyTemplateEntity( entityid templateEntityID ) 
    {
        if( hasTemplateEntity( templateEntityID ) )
        {
            const CEntitySignature& signature = m_entityRegistry.getEntitySignature( templateEntityID );

            if( !signature.isEmpty() )
            {
                for( const auto& type : signature.m_setComponentTypes )
                {
                    IComponentStorage *storage = m_mapCompTypeToStorage.at( type );
                    storage->eraseComponent( templateEntityID );
                }
            }

            m_entityRegistry.removeEntity( templateEntityID );

            m_vecRecycledIDs.push_back( templateEntityID );
        }
    }




    entityid CEntityWorld::createEntityFromTemplate( entityid templateEntityID ) 
    {
        if( hasTemplateEntity( templateEntityID ) )
        {
            const CEntitySignature& templateSignature = m_entityRegistry.getEntitySignature( templateEntityID );


            entityid entityID = getNewEntityID();
            m_entityRegistry.registerEntity( entityID, false, templateSignature );


            for( std::type_index type : templateSignature.m_setComponentTypes )
            {
                IComponentStorage *storage = m_mapCompTypeToStorage[ type ];
                storage->storeComponentCopy( entityID, templateEntityID );
            }


            CComponentBatchGuard& batchGuard = getBatchGuardWithSignature( templateSignature );
            batchGuard.fetchAndAddEntityWithComponents( entityID, m_mapCompTypeToStorage );


            return entityID; 
        }
        else
        {
            return ENTITY_ID_INVALID;
        }
    }

    std::vector< entityid > CEntityWorld::createEntitiesFromTemplate( entityid templateEntityID, unsigned int amount ) 
    {
        std::vector< entityid > ids;

        if( hasTemplateEntity( templateEntityID ) )
        {
            ids.reserve( amount );


            const CEntitySignature& templateSignature = m_entityRegistry.getEntitySignature( templateEntityID );

            CComponentBatchGuard& batchGuard = getBatchGuardWithSignature( templateSignature );


            for (unsigned int i = 0; i < amount; i++)
            {
                entityid entityID = getNewEntityID();
                m_entityRegistry.registerEntity( entityID, false, templateSignature );
                

                for( std::type_index type : templateSignature.m_setComponentTypes )
                {
                    IComponentStorage *storage = m_mapCompTypeToStorage[ type ];
                    storage->storeComponentCopy( entityID, templateEntityID );
                }


                batchGuard.fetchAndAddEntityWithComponents( entityID, m_mapCompTypeToStorage );


                ids.push_back( entityID );            
            }
        }
        
        return ids;
    }




    int CEntityWorld::queryEntities( SEntityQuery& query ) const
    {
        query.vecBatches.clear();
        
        int batchesQueried = 0;
        for( CComponentBatchGuard& batchGuard : m_vecBatchGuards )
        {
            if( query.entitySignCond( batchGuard.getBatchSignature() ) )
            {
                if( batchGuard.updateBatch() )
                {
                    query.vecBatches.push_back( batchGuard.getBatchPtr() );
                    batchesQueried++;
                }
            }
        }

        return batchesQueried;
    }





    // ========================= PRIVATE ========================= //

    entityid CEntityWorld::getNewEntityID() 
    {
        entityid id;
        if( !m_vecRecycledIDs.empty() )
        {
            id = m_vecRecycledIDs.back();
            m_vecRecycledIDs.pop_back();
        }
        else
        {
            id = ++m_idCounter;
        }

        return id;
    }
    
    CComponent* CEntityWorld::createComponentInternal( std::type_index compType, entityid entityID ) 
    {
        // check if entity exists at all
        if( !m_entityRegistry.hasEntity( entityID, true ) )
        {
            return nullptr;
        }



        // check if entity already owns the component

        const CEntitySignature& oldSignature = m_entityRegistry.getEntitySignature( entityID ); // hasEntity() assures entity exists

        IComponentStorage *storage = m_mapCompTypeToStorage[ compType ]; // createComponent assures this won't return null

        if( oldSignature.has( compType ) )
        {
            return storage->getComponent( entityID );
        }



        // make an updated signature for the entity
        CEntitySignature newSignature = oldSignature; 
        newSignature.add( compType );
        
        // instantiate the actual new component
        CComponent *comp = storage->storeComponent( entityID );

        // if it's not a template entity we send components from/to batch
        if( !m_entityRegistry.hasTemplateEntity( entityID ) )
        {
            // remove component pointers from the previous batch if entity had any component before
            if( !oldSignature.isEmpty() )
            {
                CComponentBatchGuard& oldSignBatchGuard = getBatchGuardWithSignature( oldSignature );
                oldSignBatchGuard.removeEntityWithComponents( entityID );
            }

            // fetch component pointers to the new batch
            CComponentBatchGuard& newSignBatchGuard = getBatchGuardWithSignature( newSignature );
            newSignBatchGuard.fetchAndAddEntityWithComponents( entityID, m_mapCompTypeToStorage );
        }


        // update entity registry - apply new signature to entity //
        m_entityRegistry.updateEntity( entityID, newSignature );


        return comp;
    }

    bool CEntityWorld::hasComponentInternal( std::type_index compType, entityid entityID ) const
    {
        if( !m_entityRegistry.hasEntity( entityID, true ) )
        {
            return false;
        }

        CEntitySignature signature = m_entityRegistry.getEntitySignature( entityID ); // hasEntity() assures entity exists
        return signature.has( compType );
    }

    CComponent* CEntityWorld::getComponentInternal( std::type_index compType, entityid entityID ) const
    {
        if( hasComponentInternal( compType, entityID ) )
        {
            // we know it won't throw exception from hasComponentInternal
            IComponentStorage *storage = m_mapCompTypeToStorage.at( compType );

            return storage->getComponent( entityID );
        }
        else
        {
            return nullptr;
        }
    }

    void CEntityWorld::destroyComponentInternal( std::type_index compType, entityid entityID ) 
    {
        if( hasComponentInternal( compType, entityID ) )
        {
            // compute signatures //
            const CEntitySignature& oldSignature = m_entityRegistry.getEntitySignature( entityID ); // hasComponent() assures entity exists

            CEntitySignature newSignature = oldSignature;
            newSignature.remove( compType );


            // remove instance of the component for that entity
            IComponentStorage *storage = m_mapCompTypeToStorage[ compType ];
            storage->eraseComponent( entityID );


            // if entity is not a template entity we send components from/to batch
            if( !m_entityRegistry.hasTemplateEntity( entityID ) )
            {
                // remove component pointers from the previous batch entity was asigned to
                CComponentBatchGuard& oldSignBatchGuard = getBatchGuardWithSignature( oldSignature );
                oldSignBatchGuard.removeEntityWithComponents( entityID );

                // if after removing this component entity still has any components left, fetch them into a new batch 
                if( !newSignature.isEmpty() )
                {
                    CComponentBatchGuard& newSignBatchGuard = getBatchGuardWithSignature( newSignature );
                    newSignBatchGuard.fetchAndAddEntityWithComponents( entityID, m_mapCompTypeToStorage );
                }
            }


            // update registry //
            m_entityRegistry.updateEntity( entityID, newSignature );
        }
    }

    CComponentBatchGuard& CEntityWorld::getBatchGuardWithSignature( const CEntitySignature& signature )
    {
        // we don't make batches for empty signatures
        if( signature.isEmpty() )
        {
            throw std::invalid_argument( "Batch doesn't get created for empty signature" );
        }
        else
        {
            auto it = std::find_if( m_vecBatchGuards.begin(), m_vecBatchGuards.end(),
                                    [&signature]( const CComponentBatchGuard& guard )
                                    {
                                        return guard.getBatchSignature() == signature;
                                    });

            if( it != m_vecBatchGuards.end() )
            {
                return *it;
            }
            else
            {
                m_vecBatchGuards.emplace_back( signature );
                return m_vecBatchGuards.back();
            }
        }
    }

} // namespace chestnut::ecs
