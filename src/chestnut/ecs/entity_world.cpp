#include "chestnut/ecs/entity_world.hpp"

#include <algorithm>
#include <exception>

namespace chestnut
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
        
        m_entityRegistry.registerEntity( id );
        return id;
    }

    bool CEntityWorld::hasEntity( entityid entityID ) const
    {
        return m_entityRegistry.hasEntity( entityID );
    }

    void CEntityWorld::destroyEntity( entityid entityID ) 
    {
        if( !hasEntity( entityID ) )
        {
            return;
        }

        CEntitySignature signature = m_entityRegistry.getEntitySignature( entityID );

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

    void CEntityWorld::destroyEntities( const std::vector< entityid >& entityIDs ) 
    {
        for( entityid id : entityIDs )
        {
            destroyEntity( id );
        }
    }

    void CEntityWorld::destroyAllEntities() 
    {
        for( auto& [ type, storage ] : m_mapCompTypeToStorage )
        {
            storage->clearComponents();
        }

        m_vecBatchGuards.clear();

        m_entityRegistry.removeAllEntities();
    }

    int CEntityWorld::queryEntities( SEntityQuery& query ) const
    {
        query.vecBatches.clear();
        
        int batchesQueried = 0;
        for( CComponentBatchGuard& batchGuard : m_vecBatchGuards )
        {
            if( query.entitySignCond( batchGuard.getBatchSignature() ) )
            {
                batchGuard.updateBatch();
                query.vecBatches.push_back( batchGuard.getBatchPtr() );
                batchesQueried++;
            }
        }

        return batchesQueried;
    }





    // ========================= PRIVATE ========================= //

    CComponent* CEntityWorld::createComponentInternal( std::type_index compType, entityid entityID ) 
    {
        if( !hasEntity( entityID ) )
        {
            return nullptr;
        }
        if( hasComponentInternal( compType, entityID ) )
        {
            return getComponentInternal( compType, entityID );
        }

        CEntitySignature oldSignature, newSignature;
        IComponentStorage *storage;
        CComponent *comp;

        // compute signatures //
        oldSignature = m_entityRegistry.getEntitySignature( entityID ); // hasEntity() assures entity exists
        newSignature = oldSignature; 
        newSignature.add( compType );
        
        // create an instance of the component //
        // we know it won't return null as publicly only functions assuring storages' later existence are available
        storage = m_mapCompTypeToStorage[ compType ];
        comp = storage->storeComponent( entityID );

        // remove component pointers from the previous batch if entity had any component before
        if( !oldSignature.isEmpty() )
        {
            CComponentBatchGuard& oldSignBatchGuard = getBatchGuardWithSignature( oldSignature );
            oldSignBatchGuard.removeEntityWithComponents( entityID );
        }

        // fetch component pointers to the new batch
        CComponentBatchGuard& newSignBatchGuard = getBatchGuardWithSignature( newSignature );
        newSignBatchGuard.fetchAndAddEntityWithComponents( entityID, m_mapCompTypeToStorage );

        // update entity registry - apply new signature to entity //
        m_entityRegistry.updateEntity( entityID, newSignature );

        return comp;
    }

    bool CEntityWorld::hasComponentInternal( std::type_index compType, entityid entityID ) const
    {
        if( !hasEntity( entityID ) )
        {
            return false;
        }

        CEntitySignature signature;

        signature = m_entityRegistry.getEntitySignature( entityID ); // hasEntity() assures entity exists
        return signature.has( compType );
    }

    CComponent* CEntityWorld::getComponentInternal( std::type_index compType, entityid entityID ) const
    {
        if( !hasComponentInternal( compType, entityID ) )
        {
            return nullptr;
        }

        IComponentStorage *storage;
        CComponent *comp;

        // we know it won't throw exception as publicly only functions assuring storages' later existence are available
        storage = m_mapCompTypeToStorage.at( compType );
        comp = storage->getComponent( entityID );

        return comp;
    }

    void CEntityWorld::destroyComponentInternal( std::type_index compType, entityid entityID ) 
    {
        if( !hasComponentInternal( compType, entityID ) )
        {
            return;
        }

        CEntitySignature oldSignature, newSignature;

        // compute signatures //
        oldSignature = m_entityRegistry.getEntitySignature( entityID ); // hasComponent() assures entity exists
        newSignature = oldSignature;
        newSignature.remove( compType );

        IComponentStorage *storage = m_mapCompTypeToStorage[ compType ];
        storage->eraseComponent( entityID );

        CComponentBatchGuard& oldSignBatchGuard = getBatchGuardWithSignature( oldSignature );
        oldSignBatchGuard.removeEntityWithComponents( entityID );

        if( !newSignature.isEmpty() )
        {
            CComponentBatchGuard& newSignBatchGuard = getBatchGuardWithSignature( newSignature );
            newSignBatchGuard.fetchAndAddEntityWithComponents( entityID, m_mapCompTypeToStorage );
        }

        // update registry //
        m_entityRegistry.updateEntity( entityID, newSignature );
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

} // namespace chestnut
