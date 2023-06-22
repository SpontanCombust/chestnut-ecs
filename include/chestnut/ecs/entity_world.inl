#include <typelist.hpp>

#include <exception>

namespace chestnut::ecs
{
    inline CEntityWorld::CEntityWorld() 
    : m_entityRegistry(&m_componentStorage),
      entityIterator(this)
    {
        
    }

    inline CEntityWorld::~CEntityWorld() 
    {

    }



    inline entityslot_t CEntityWorld::createEntity(bool canRecycleId) 
    {
        return m_entityRegistry.registerNewEntity(canRecycleId);
    }

    template<typename C>
    entityslot_t CEntityWorld::createEntityWithComponents(C&& data, bool canRecycleId)
    {
        entityslot_t ent = m_entityRegistry.registerNewEntity(canRecycleId);

        m_componentStorage.insert<C>(ent, std::forward<C>(data));

        CEntitySignature newSign = CEntitySignature::from<C>();
        this->updateQueriesOnEntityChange(ent, nullptr, &newSign);

        return ent;
    }

    template<typename C, typename... CRest>
    entityslot_t CEntityWorld::createEntityWithComponents(std::tuple<C, CRest...>&& data, bool canRecycleId)
    {
        entityslot_t ent = m_entityRegistry.registerNewEntity(canRecycleId);

        tl::type_list<C, CRest...>::for_each([&](auto t) {
            using _Type = typename decltype(t)::type;
            m_componentStorage.insert<_Type>(ent, std::forward<_Type>(std::get<_Type>(data)));
        });

        CEntitySignature newSign = CEntitySignature::from<C, CRest...>();
        this->updateQueriesOnEntityChange(ent, nullptr, &newSign);

        return ent;
    }

    inline bool CEntityWorld::hasEntity( entityslot_t entityID ) const
    {
        return m_entityRegistry.isEntityRegistered(entityID);
    }

    inline void CEntityWorld::destroyEntity( entityslot_t entityID ) 
    {
        if(hasEntity(entityID))
        {
            const CEntitySignature signature = m_entityRegistry.getEntitySignature(entityID);

            if(!signature.isEmpty())
            {
                updateQueriesOnEntityChange(entityID, &signature, nullptr);
                m_componentStorage.eraseAll(entityID);
            }

            m_entityRegistry.unregisterEntity( entityID );
        }
    }




    template <typename C>
    inline CComponentHandle<C> CEntityWorld::createComponent(entityslot_t entityID, C &&data)
    {
        // check if entity exists at all
        if( !hasEntity(entityID) )
        {
            return CComponentHandle<C>();
        }

        // check if entity already owns the component
        if(!m_componentStorage.contains<C>(entityID))
        {
            // make an updated signature for the entity
            CEntitySignature oldSignature = m_entityRegistry.getEntitySignature(entityID);
            CEntitySignature newSignature = oldSignature; 
            newSignature.add<C>();
            
            // instantiate the actual new component
            m_componentStorage.insert<C>(entityID, std::forward<C>(data));

            updateQueriesOnEntityChange( entityID, &oldSignature, &newSignature );
        }

        return CComponentHandle<C>(entityID, &m_componentStorage);
    }

    template <typename C>
    inline CComponentHandle<C> CEntityWorld::createOrUpdateComponent(entityslot_t entityID, C &&data)
    {
        // check if entity exists at all
        if( !hasEntity(entityID) )
        {
            return CComponentHandle<C>();
        }

        // check if entity already owns the component
        if(!m_componentStorage.contains<C>(entityID))
        {
            // make an updated signature for the entity
            CEntitySignature oldSignature = m_entityRegistry.getEntitySignature(entityID);
            CEntitySignature newSignature = oldSignature; 
            newSignature.add<C>();
            
            updateQueriesOnEntityChange( entityID, &oldSignature, &newSignature );
        }

        m_componentStorage.insert<C>(entityID, std::forward<C>(data));

        return CComponentHandle<C>(entityID, &m_componentStorage);
    }
    
    template < typename C >
    bool CEntityWorld::hasComponent( entityslot_t entityID ) const
    {
        if(!hasEntity(entityID))
        {
            return false;
        }

        return m_componentStorage.contains<C>(entityID);
    }

    template< typename C >
    CComponentHandle<C> CEntityWorld::getComponent( entityslot_t entityID ) const
    {
        if(hasComponent<C>(entityID))
        {
            return CComponentHandle<C>(entityID, &m_componentStorage);
        }

        return CComponentHandle<C>();
    }

    template< typename C >
    void CEntityWorld::destroyComponent( entityslot_t entityID ) 
    {
        if(hasComponent<C>(entityID))
        {
            // compute signatures //
            const CEntitySignature oldSignature = m_entityRegistry.getEntitySignature( entityID ); // hasComponent() assures entity exists

            CEntitySignature newSignature = oldSignature;
            newSignature.remove<C>();

            m_componentStorage.erase<C>(entityID);

            updateQueriesOnEntityChange( entityID, &oldSignature, &newSignature );
        }
    }





    inline CEntityQuery *CEntityWorld::createQuery(const CEntitySignature& requireSignature, const CEntitySignature& rejectSignature)
    {
        std::unique_ptr<internal::CEntityQueryGuard> guard = std::make_unique<internal::CEntityQueryGuard>(&m_componentStorage, requireSignature, rejectSignature);


        std::vector< entityslot_t > vecEntitiesToFetchFrom = m_entityRegistry.findEntities( 
        [&guard]( const CEntitySignature& sign )
        {
            return guard->testQuery( sign );
        });

        for (entityslot_t i = 0; i < vecEntitiesToFetchFrom.size(); i++)
        {
            guard->enqueueEntity(vecEntitiesToFetchFrom[i]);
        }
    
        CEntityQuery *query = &guard->getQuery();
        m_mapQueryIDToQueryGuard[query] = std::move(guard);

        return query;
    }

    inline CEntityQuery *CEntityWorld::createQuery(const CEntitySignature& requireSignature)
    {
        return this->createQuery(requireSignature, makeEntitySignature<>());
    }

    inline SEntityQueryUpdateInfo CEntityWorld::queryEntities( CEntityQuery *query ) const
    {
        if(!query)
        {
            throw std::runtime_error("Query is null");
        }

        auto it = m_mapQueryIDToQueryGuard.find( query );
        if( it != m_mapQueryIDToQueryGuard.end() )
        {
            return it->second->updateQuery();
        }

        throw std::runtime_error("Query does not belong to this CEntityWorld");
    }

    inline void CEntityWorld::destroyQuery( CEntityQuery *query )
    {
        auto it = m_mapQueryIDToQueryGuard.find( query );
        if( it != m_mapQueryIDToQueryGuard.end() )
        {
            m_mapQueryIDToQueryGuard.erase( it );
        }
    }




    inline CEntityIterator CEntityWorld::EntityIteratorMethods::begin() noexcept
    {
        auto it = CEntityIterator(
            &m_parent->m_entityRegistry, 
            &m_parent->m_componentStorage, 
            ENTITY_SLOT_MINIMAL
        );

        if(!it.isValid() && it.canGoForward())
        {
            ++it;
        }

        return it;
    }

    inline CEntityIterator CEntityWorld::EntityIteratorMethods::end() noexcept
    {
        auto it = CEntityIterator(
            &m_parent->m_entityRegistry, 
            &m_parent->m_componentStorage, 
            m_parent->m_entityRegistry.getHighestIdRegistered() + 1
        );

        return it;
    }

    inline CEntityConstIterator CEntityWorld::EntityIteratorMethods::cbegin() const noexcept
    {
        auto it = CEntityConstIterator(
            &m_parent->m_entityRegistry, 
            &m_parent->m_componentStorage, 
            ENTITY_SLOT_MINIMAL
        );

        if(!it.isValid() && it.canGoForward())
        {
            ++it;
        }

        return it;
    }

    inline CEntityConstIterator CEntityWorld::EntityIteratorMethods::cend() const noexcept
    {
        auto it = CEntityConstIterator(
            &m_parent->m_entityRegistry, 
            &m_parent->m_componentStorage, 
            m_parent->m_entityRegistry.getHighestIdRegistered() + 1
        );

        return it;
    }



    
    inline CEntitySignature CEntityWorld::getEntitySignature(entityslot_t entityID) const
    {
        return m_entityRegistry.getEntitySignature(entityID);
    }

    inline std::vector< entityslot_t > CEntityWorld::findEntities(std::function< bool( const CEntitySignature& ) > pred ) const
    {
        return m_entityRegistry.findEntities(pred);
    }
    





    inline void CEntityWorld::updateQueriesOnEntityChange( entityslot_t entity, const CEntitySignature* prevSignature, const CEntitySignature* currSignature )
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
                guard->enqueueEntity( entity );
            }
            else if( prevValid && !currValid )
            {
                guard->dequeueEntity( entity );
            }
        }
    }

} // namespace chestnut::ecs
