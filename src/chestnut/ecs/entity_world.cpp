#include "chestnut/ecs/entity_world.hpp"

#include <exception> // invalid_argument

namespace chestnut::ecs
{
    using namespace internal;

    // ========================= PUBLIC ========================= //

    CEntityWorld::CEntityWorld() 
    {
        
    }

    CEntityWorld::~CEntityWorld() 
    {
        for( auto& [ type, storage ] : m_mapCompTypeToStorage )
        {
            delete storage;
        }

        for( auto& [ id, guard ] : m_mapQueryIDToQueryGuard )
        {
            delete guard;
        }
    }



    entityid CEntityWorld::createEntity() 
    {
        return m_entityRegistry.registerNewEntity( false );
    }

    std::vector< entityid > CEntityWorld::createEntities( entitysize amount ) 
    {
        std::vector< entityid > ids;

        ids.resize( amount );

        for (entitysize i = 0; i < amount; i++)
        {
            ids[i] = m_entityRegistry.registerNewEntity( false );
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
                updateQueriesOnEntityChange( entityID, &signature, nullptr );

                for( const auto& type : signature.m_setComponentTypes )
                {
                    IComponentStorage *storage = m_mapCompTypeToStorage.at( type );
                    storage->eraseComponent( entityID );
                }
            }

            m_entityRegistry.unregisterEntity( entityID );
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
        return m_entityRegistry.registerNewEntity( true );
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

            m_entityRegistry.unregisterEntity( templateEntityID );
        }
    }




    entityid CEntityWorld::createEntityFromTemplate( entityid templateEntityID ) 
    {
        if( hasTemplateEntity( templateEntityID ) )
        {
            const CEntitySignature& templateSignature = m_entityRegistry.getEntitySignature( templateEntityID );

            entityid entityID = m_entityRegistry.registerNewEntity( false, templateSignature );

            if( !templateSignature.isEmpty() )
            {
                for( std::type_index type : templateSignature.m_setComponentTypes )
                {
                    IComponentStorage *storage = m_mapCompTypeToStorage[ type ];
                    storage->storeComponentCopy( entityID, templateEntityID );
                }

                updateQueriesOnEntityChange( entityID, nullptr, &templateSignature );
            }

            return entityID; 
        }
        else
        {
            return ENTITY_ID_INVALID;
        }
    }

    std::vector< entityid > CEntityWorld::createEntitiesFromTemplate( entityid templateEntityID, entitysize amount ) 
    {
        std::vector< entityid > ids;

        if( hasTemplateEntity( templateEntityID ) )
        {
            ids.reserve( amount );


            const CEntitySignature& templateSignature = m_entityRegistry.getEntitySignature( templateEntityID );

            if( !templateSignature.isEmpty() )
            {
                for (entitysize i = 0; i < amount; i++)
                {
                    entityid entityID = m_entityRegistry.registerNewEntity( false, templateSignature );

                    for( std::type_index type : templateSignature.m_setComponentTypes )
                    {
                        IComponentStorage *storage = m_mapCompTypeToStorage[ type ];
                        storage->storeComponentCopy( entityID, templateEntityID );
                    }

                    updateQueriesOnEntityChange( entityID, nullptr, &templateSignature );

                    ids.push_back( entityID );            
                }
            }
            else
            {
                for (entitysize i = 0; i < amount; i++)
                {
                    ids.push_back( m_entityRegistry.registerNewEntity( false ) );
                }
            }
        }
        
        return ids;
    }




    queryid CEntityWorld::createQuery( const CEntitySignature& requireSignature, const CEntitySignature& rejectSignature )
    {
        static queryid queryIDCounter = 0;

        queryIDCounter++;

        internal::CEntityQueryGuard *guard = new internal::CEntityQueryGuard( queryIDCounter, requireSignature, rejectSignature, &m_mapCompTypeToStorage );


        std::vector< entityid > vecEntitiesToFetchFrom = m_entityRegistry.findEntities( 
        [&guard]( const CEntitySignature& sign )
        {
            return guard->testQuery( sign );
        });

        for (entityid i = 0; i < vecEntitiesToFetchFrom.size(); i++)
        {
            guard->fetchAndAddEntityWithComponents( vecEntitiesToFetchFrom[i] );
        }
    

        m_mapQueryIDToQueryGuard[queryIDCounter] = guard;

        return queryIDCounter;
    }

    const CEntityQuery* CEntityWorld::queryEntities( queryid id ) const
    {
        auto it = m_mapQueryIDToQueryGuard.find( id );
        if( it != m_mapQueryIDToQueryGuard.end() )
        {
            it->second->updateQuery();
            return &it->second->getQuery();
        }

        return nullptr;
    }

    void CEntityWorld::destroyQuery( queryid id )
    {
        auto it = m_mapQueryIDToQueryGuard.find( id );
        if( it != m_mapQueryIDToQueryGuard.end() )
        {
            delete it->second;
            m_mapQueryIDToQueryGuard.erase( it );
        }
    }

    std::vector< entityid > CEntityWorld::findEntities(std::function< bool( const CEntitySignature& ) > pred ) const
    {
        return m_entityRegistry.findEntities( pred );
    }





    // ========================= PRIVATE ========================= //

    IComponentWrapper* CEntityWorld::createComponentInternal( std::type_index compType, entityid entityID ) 
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
        IComponentWrapper *comp = storage->storeComponent( entityID );

        // if it's not a template entity we update queries for it
        if( !m_entityRegistry.hasTemplateEntity( entityID ) )
        {
            updateQueriesOnEntityChange( entityID, &oldSignature, &newSignature );
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

    IComponentWrapper* CEntityWorld::getComponentInternal( std::type_index compType, entityid entityID ) const
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


            // if it's not a template entity we update queries for it
            if( !m_entityRegistry.hasTemplateEntity( entityID ) )
            {
                updateQueriesOnEntityChange( entityID, &oldSignature, &newSignature );
            }


            // update registry //
            m_entityRegistry.updateEntity( entityID, newSignature );
        }
    }

    void CEntityWorld::updateQueriesOnEntityChange( entityid entity, const CEntitySignature* prevSignature, const CEntitySignature* currSignature )
    {
        bool prevValid, currValid;

        for( auto& [ id, guard ] : m_mapQueryIDToQueryGuard )
        {
            if( prevSignature )
            {
                prevValid = guard->testQuery( *prevSignature );
            }
            else
            {
                prevValid = false;
            }

            if( currSignature )
            {
                currValid = guard->testQuery( *currSignature );
            }
            else
            {
                currValid = false;
            }

            if( !prevValid && currValid )
            {
                guard->fetchAndAddEntityWithComponents( entity );
            }
            else if( prevValid && !currValid )
            {
                guard->removeEntityWithComponents( entity );
            }
        }
    }

} // namespace chestnut::ecs
