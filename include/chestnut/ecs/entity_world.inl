#include <type_traits>
#include <exception> // invalid_argument

namespace chestnut::ecs
{
    namespace internal
    {
        template< typename T >
        inline constexpr bool is_valid_component_type = !std::is_fundamental_v<T> && 
                                                         std::is_default_constructible_v<T> && 
                                                         std::is_copy_assignable_v<T>;

    } // namespace internal



    // ========================= PUBLIC ========================= //

    inline CEntityWorld::CEntityWorld() 
    {
        
    }

    inline CEntityWorld::~CEntityWorld() 
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



    inline entityid_t CEntityWorld::createEntity() 
    {
        return m_entityRegistry.registerNewEntity( false );
    }

    inline std::vector< entityid_t > CEntityWorld::createEntities( entitysize_t amount ) 
    {
        std::vector< entityid_t > ids;

        ids.resize( amount );

        for (entitysize_t i = 0; i < amount; i++)
        {
            ids[i] = m_entityRegistry.registerNewEntity( false );
        }
        
        return ids;
    }

    inline bool CEntityWorld::hasEntity( entityid_t entityID ) const
    {
        return m_entityRegistry.hasEntity( entityID, internal::CEntityRegistry::CAN_BE_REGULAR_ENTITY );
    }

    inline void CEntityWorld::destroyEntity( entityid_t entityID ) 
    {
        if( hasEntity( entityID ) )
        {
            const CEntitySignature* signature = m_entityRegistry.getEntitySignature( entityID );

            if( !signature->isEmpty() )
            {
                updateQueriesOnEntityChange( entityID, signature, nullptr );

                for( const auto& type : signature->m_setComponentTypes )
                {
                    internal::IComponentStorage *storage = m_mapCompTypeToStorage.at( type );
                    storage->eraseComponent( entityID );
                }
            }

            m_entityRegistry.unregisterEntity( entityID );
        }
    }

    inline void CEntityWorld::destroyEntities( const std::vector< entityid_t >& entityIDs ) 
    {
        for( entityid_t id : entityIDs )
        {
            destroyEntity( id );
        }
    }




    inline entityid_t CEntityWorld::createTemplateEntity() 
    {
        return m_entityRegistry.registerNewEntity( true );
    }

    inline bool CEntityWorld::hasTemplateEntity( entityid_t templateEntityID ) const
    {
        return m_entityRegistry.hasEntity( templateEntityID, internal::CEntityRegistry::CAN_BE_TEMPLATE_ENTITY );
    }

    inline void CEntityWorld::destroyTemplateEntity( entityid_t templateEntityID ) 
    {
        if( hasTemplateEntity( templateEntityID ) )
        {
            const CEntitySignature* signature = m_entityRegistry.getEntitySignature( templateEntityID );

            if( !signature->isEmpty() )
            {
                for( const auto& type : signature->m_setComponentTypes )
                {
                    internal::IComponentStorage *storage = m_mapCompTypeToStorage.at( type );
                    storage->eraseComponent( templateEntityID );
                }
            }

            m_entityRegistry.unregisterEntity( templateEntityID );
        }
    }




    inline entityid_t CEntityWorld::createEntityFromTemplate( entityid_t templateEntityID ) 
    {
        if( hasTemplateEntity( templateEntityID ) )
        {
            const CEntitySignature* templateSignature = m_entityRegistry.getEntitySignature( templateEntityID );

            entityid_t entityID = m_entityRegistry.registerNewEntity( false, *templateSignature );

            if( !templateSignature->isEmpty() )
            {
                for( std::type_index type : templateSignature->m_setComponentTypes )
                {
                    internal::IComponentStorage *storage = m_mapCompTypeToStorage[ type ];
                    storage->storeComponentCopy( entityID, templateEntityID );
                }

                updateQueriesOnEntityChange( entityID, nullptr, templateSignature );
            }

            return entityID; 
        }
        else
        {
            return ENTITY_ID_INVALID;
        }
    }

    inline std::vector< entityid_t > CEntityWorld::createEntitiesFromTemplate( entityid_t templateEntityID, entitysize_t amount ) 
    {
        std::vector< entityid_t > ids;

        if( hasTemplateEntity( templateEntityID ) )
        {
            ids.reserve( amount );


            const CEntitySignature* templateSignature = m_entityRegistry.getEntitySignature( templateEntityID );

            if( !templateSignature->isEmpty() )
            {
                for (entitysize_t i = 0; i < amount; i++)
                {
                    entityid_t entityID = m_entityRegistry.registerNewEntity( false, *templateSignature );

                    for( std::type_index type : templateSignature->m_setComponentTypes )
                    {
                        internal::IComponentStorage *storage = m_mapCompTypeToStorage[ type ];
                        storage->storeComponentCopy( entityID, templateEntityID );
                    }

                    updateQueriesOnEntityChange( entityID, nullptr, templateSignature );

                    ids.push_back( entityID );            
                }
            }
            else
            {
                for (entitysize_t i = 0; i < amount; i++)
                {
                    ids.push_back( m_entityRegistry.registerNewEntity( false ) );
                }
            }
        }
        
        return ids;
    }




    inline queryid_t CEntityWorld::createQuery( const CEntitySignature& requireSignature, const CEntitySignature& rejectSignature )
    {
        static queryid_t queryIDCounter = 0;

        queryIDCounter++;

        internal::CEntityQueryGuard *guard = new internal::CEntityQueryGuard( queryIDCounter, requireSignature, rejectSignature, &m_mapCompTypeToStorage );


        std::vector< entityid_t > vecEntitiesToFetchFrom = m_entityRegistry.findEntities( 
        [&guard]( const CEntitySignature& sign )
        {
            return guard->testQuery( sign );

        }, internal::CEntityRegistry::CAN_BE_REGULAR_ENTITY );

        for (entityid_t i = 0; i < vecEntitiesToFetchFrom.size(); i++)
        {
            guard->fetchAndAddEntityWithComponents( vecEntitiesToFetchFrom[i] );
        }
    

        m_mapQueryIDToQueryGuard[queryIDCounter] = guard;

        return queryIDCounter;
    }

    inline CEntityQuery* CEntityWorld::queryEntities( queryid_t id ) const
    {
        auto it = m_mapQueryIDToQueryGuard.find( id );
        if( it != m_mapQueryIDToQueryGuard.end() )
        {
            it->second->updateQuery();
            return it->second->getQuery();
        }

        return nullptr;
    }

    inline void CEntityWorld::destroyQuery( queryid_t id )
    {
        auto it = m_mapQueryIDToQueryGuard.find( id );
        if( it != m_mapQueryIDToQueryGuard.end() )
        {
            delete it->second;
            m_mapQueryIDToQueryGuard.erase( it );
        }
    }

    inline std::vector< entityid_t > CEntityWorld::findEntities(std::function< bool( const CEntitySignature& ) > pred ) const
    {
        return m_entityRegistry.findEntities( pred, internal::CEntityRegistry::CAN_BE_REGULAR_ENTITY );
    }




    inline CEntityWorld::WriteLock CEntityWorld::lockForWriting() const
    {
        return WriteLock( m_mutex );
    }

    inline CEntityWorld::ReadLock CEntityWorld::lockForReading() const
    {
        return ReadLock( m_mutex );
    }


    // ========================= PRIVATE ========================= //

    inline internal::IComponentWrapper* CEntityWorld::createComponentInternal( std::type_index compType, entityid_t entityID ) 
    {
        // check if entity exists at all
        if( !m_entityRegistry.hasEntity( entityID, internal::CEntityRegistry::CAN_BE_REGULAR_ENTITY | internal::CEntityRegistry::CAN_BE_TEMPLATE_ENTITY ) )
        {
            return nullptr;
        }



        // check if entity already owns the component

        const CEntitySignature* oldSignature = m_entityRegistry.getEntitySignature( entityID ); // hasEntity() assures entity exists

        internal::IComponentStorage *storage = m_mapCompTypeToStorage[ compType ]; // createComponent assures this won't return null

        if( oldSignature->has( compType ) )
        {
            return storage->getComponent( entityID );
        }



        // make an updated signature for the entity
        CEntitySignature newSignature = *oldSignature; 
        newSignature.add( compType );
        
        // instantiate the actual new component
        internal::IComponentWrapper *comp = storage->storeComponent( entityID );

        // if it's not a template entity we update queries for it
        if( m_entityRegistry.hasEntity( entityID, internal::CEntityRegistry::CAN_BE_REGULAR_ENTITY ) )
        {
            updateQueriesOnEntityChange( entityID, oldSignature, &newSignature );
        }


        // update entity registry - apply new signature to entity //
        m_entityRegistry.updateEntity( entityID, newSignature );


        return comp;
    }

    inline bool CEntityWorld::hasComponentInternal( std::type_index compType, entityid_t entityID ) const
    {
        if( !m_entityRegistry.hasEntity( entityID, internal::CEntityRegistry::CAN_BE_REGULAR_ENTITY | internal::CEntityRegistry::CAN_BE_TEMPLATE_ENTITY ) )
        {
            return false;
        }

        const CEntitySignature* signature = m_entityRegistry.getEntitySignature( entityID ); // hasEntity() assures entity exists
        return signature->has( compType );
    }

    inline internal::IComponentWrapper* CEntityWorld::getComponentInternal( std::type_index compType, entityid_t entityID ) const
    {
        if( hasComponentInternal( compType, entityID ) )
        {
            // we know it won't throw exception from hasComponentInternal
            internal::IComponentStorage *storage = m_mapCompTypeToStorage.at( compType );

            return storage->getComponent( entityID );
        }
        else
        {
            return nullptr;
        }
    }

    inline void CEntityWorld::destroyComponentInternal( std::type_index compType, entityid_t entityID ) 
    {
        if( hasComponentInternal( compType, entityID ) )
        {
            // compute signatures //
            const CEntitySignature* oldSignature = m_entityRegistry.getEntitySignature( entityID ); // hasComponent() assures entity exists

            CEntitySignature newSignature = *oldSignature;
            newSignature.remove( compType );


            // remove instance of the component for that entity
            internal::IComponentStorage *storage = m_mapCompTypeToStorage[ compType ];
            storage->eraseComponent( entityID );


            // if it's not a template entity we update queries for it
            if( m_entityRegistry.hasEntity( entityID, internal::CEntityRegistry::CAN_BE_REGULAR_ENTITY ) )
            {
                updateQueriesOnEntityChange( entityID, oldSignature, &newSignature );
            }


            // update registry //
            m_entityRegistry.updateEntity( entityID, newSignature );
        }
    }

    inline void CEntityWorld::updateQueriesOnEntityChange( entityid_t entity, const CEntitySignature* prevSignature, const CEntitySignature* currSignature )
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



    // ========================= PUBLIC ========================= //

    template< typename C, typename Traits >
    void CEntityWorld::setupComponentType()
    {
        static_assert( internal::is_valid_component_type<C>, "Given type is not a valid component type!" );

        std::type_index type = typeid(C);

        auto it = m_mapCompTypeToStorage.find( type );
        
        if( it == m_mapCompTypeToStorage.end() )
        {
            m_mapCompTypeToStorage[ type ] = new internal::CComponentStorage<C>( 
                Traits::storageSegmentSize, 
                Traits::storageInitCapacity 
            );
        }
    }

    template< typename C >
    inline void CEntityWorld::setupComponentTypeIfDidntAlready() 
    {
        setupComponentType< C, chestnut::ecs::ComponentTraits<C> >();
    }





    template< typename C >
    CComponentHandle<C> CEntityWorld::createComponent( entityid_t entityID ) 
    {
        setupComponentTypeIfDidntAlready<C>();

        internal::IComponentWrapper *uncastedComp;
        uncastedComp = createComponentInternal( std::type_index( typeid( C ) ), entityID );
        auto handle = CComponentHandle<C>( entityID, static_cast< internal::SComponentWrapper<C>* >( uncastedComp ) );

        return handle;
    }

    template< typename C >
    bool CEntityWorld::hasComponent( entityid_t entityID ) const
    {
        return hasComponentInternal( std::type_index( typeid( C ) ), entityID );
    }

    template< typename C >
    CComponentHandle<C> CEntityWorld::getComponent( entityid_t entityID ) const
    {
        internal::IComponentWrapper *uncastedComp;
        uncastedComp = getComponentInternal( std::type_index( typeid( C ) ), entityID );
        auto handle = CComponentHandle<C>( entityID, static_cast< internal::SComponentWrapper<C>* >( uncastedComp ) );

        return handle;
    }

    template< typename C >
    void CEntityWorld::destroyComponent( entityid_t entityID ) 
    {
        destroyComponentInternal( std::type_index( typeid( C ) ), entityID );
    }





    template< typename C > 
    void CEntityWorld::reserveComponentMemoryTotal( entitysize_t amount )
    {
        setupComponentTypeIfDidntAlready<C>();

        // setupComponentTypeIfDidntAlready assures this won't return null
        internal::IComponentStorage *storage = m_mapCompTypeToStorage[ typeid(C) ];

        storage->reserve( amount );
    }

    template< typename C > 
    void CEntityWorld::reserveComponentMemoryAdditional( entitysize_t amount )
    {
        setupComponentTypeIfDidntAlready<C>();

        // setupComponentTypeIfDidntAlready assures this won't return null
        internal::IComponentStorage *storage = m_mapCompTypeToStorage[ typeid(C) ];

        storage->reserveAdditional( amount );
    }

    template< typename C >
    void CEntityWorld::freeComponentMemory( entitysize_t amount )
    {
        auto it = m_mapCompTypeToStorage.find( typeid(C) );
        if( it != m_mapCompTypeToStorage.end() )
        {
            internal::IComponentStorage *storage = it->second;

            storage->resize( storage->getCapacity() - amount );
        }
    }

} // namespace chestnut::ecs
