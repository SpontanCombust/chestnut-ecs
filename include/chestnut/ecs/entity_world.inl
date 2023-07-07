#include "native_components.hpp"

#include <exception>
#include "entity_world.hpp"

namespace chestnut::ecs
{
    inline CEntityWorld::CEntityWorld() 
    : m_entityRegistry(),
      entityIterator(this)
    {
        
    }

    inline CEntityWorld::~CEntityWorld() 
    {

    }



    inline CEntity CEntityWorld::createEntity() 
    {
        CEntity entity = m_entityRegistry.registerNewEntity();
        m_componentStorage.insert(entity.slot, CIdentityComponent{entity.uuid});
        //TODO think if queries should be updated
        return entity;
    }

    template <typename... Components>
    inline CEntity CEntityWorld::createEntity(Components &&...data)
    {
        CEntity entity = m_entityRegistry.registerNewEntity();

        m_componentStorage.insert(entity.slot, CIdentityComponent{entity.uuid});
        (m_componentStorage.insert(entity.slot, std::forward<Components>(data)), ...); // insert for each component

        //FIXME also needs identity component
        CEntitySignature newSign = CEntitySignature::from<Components...>();
        this->updateQuerySuppliersOnEntityChange(entity.slot, nullptr, &newSign);

        return entity;
    }

    inline bool CEntityWorld::isEntityAlive(CEntity entity) const
    {
        return m_entityRegistry.isEntityRegistered(entity.uuid);
    }

    inline void CEntityWorld::destroyEntity(CEntity entity) 
    {
        if(isEntityAlive(entity))
        {
            const CEntitySignature signature = m_componentStorage.signature(entity.slot);

            if(!signature.empty())
            {
                updateQuerySuppliersOnEntityChange(entity.slot, &signature, nullptr);
                m_componentStorage.eraseAll(entity.slot);
            }

            m_entityRegistry.unregisterEntity(entity.uuid);
        }
    }



    template <typename C>
    inline tl::expected<CComponentHandle<C>, std::string> CEntityWorld::insertComponent(CEntity entity, C &&data)
    {
        // check if entity exists at all
        if( !isEntityAlive(entity) )
        {
            return tl::make_unexpected("No such entity found");
        }

        // check if entity already owns the component
        if(!m_componentStorage.contains<C>(entity.slot))
        {
            // make an updated signature for the entity
            CEntitySignature oldSignature = m_componentStorage.signature(entity.slot);
            CEntitySignature newSignature = oldSignature; 
            newSignature.add<C>();
            
            updateQuerySuppliersOnEntityChange(entity.slot, &oldSignature, &newSignature );
        }

        m_componentStorage.insert<C>(entity.slot, std::forward<C>(data));

        return CComponentHandle<C>(entity, &m_componentStorage);
    }
    
    template < typename C >
    inline bool CEntityWorld::hasComponent(CEntity entity) const
    {
        if(!isEntityAlive(entity))
        {
            return false;
        }

        return m_componentStorage.contains<C>(entity.slot);
    }

    template< typename C >
    inline tl::expected<CComponentHandle<C>, std::string> CEntityWorld::getComponent(CEntity entity) const
    {
        if(!hasComponent<C>(entity))
        {
            return tl::make_unexpected("Component does not exist");
        }

        return CComponentHandle<C>(entity, &m_componentStorage);
    }

    template< typename C >
    void CEntityWorld::destroyComponent(CEntity entity) 
    {
        if(hasComponent<C>(entity))
        {
            const CEntitySignature oldSignature = m_componentStorage.signature(entity.slot);

            CEntitySignature newSignature = oldSignature;
            newSignature.remove<C>();

            m_componentStorage.erase<C>(entity.slot);

            updateQuerySuppliersOnEntityChange(entity.slot, &oldSignature, &newSignature );
        }
    }





    inline CEntityQuery CEntityWorld::createQuery(const CEntitySignature& requireSignature, const CEntitySignature& rejectSignature)
    {
        auto supplier = std::make_unique<internal::CEntityQueryGuard>(requireSignature, rejectSignature);

        std::vector<CEntity> vecEntitiesToFetchFrom = findEntities( 
        [&supplier]( const CEntitySignature& sign )
        {
            return supplier->testSignature( sign );
        });

        for (const CEntity& entity: vecEntitiesToFetchFrom)
        {
            supplier->enqueueEntity(entity.slot);
        }
    
        CEntityQuery query(&m_componentStorage, supplier.get());
        m_listQuerySuppliers.push_back(std::move(supplier));

        return query;
    }

    inline CEntityQuery CEntityWorld::createQuery(const CEntitySignature& requireSignature)
    {
        return this->createQuery(requireSignature, CEntitySignature());
    }

    inline void CEntityWorld::destroyQuery(CEntityQuery& query)
    {
        for (auto it = m_listQuerySuppliers.cbegin(); it != m_listQuerySuppliers.cend(); ++it)
        {
            const auto& supplier = *it;
            if (query.getRequireSignature() == supplier->requireSignature() 
             && query.getRejectSignature() == supplier->rejectSignature())
            {
                m_listQuerySuppliers.erase(it);
                break;
            }
        }
    }




    inline CEntityIterator CEntityWorld::EntityIteratorMethods::begin() noexcept
    {
        auto it = CEntityIterator(
            &m_parent->m_componentStorage, 
            ENTITY_SLOT_MINIMAL
        );

        if(!it.isSlotTaken() && it.isInBounds())
        {
            ++it;
        }

        return it;
    }

    inline CEntityIterator CEntityWorld::EntityIteratorMethods::end() noexcept
    {
        auto it = CEntityIterator(
            &m_parent->m_componentStorage, 
            (entityslot_t)m_parent->m_componentStorage.maxSlot() + 1
        );

        return it;
    }

    inline CEntityConstIterator CEntityWorld::EntityIteratorMethods::cbegin() const noexcept
    {
        auto it = CEntityConstIterator(
            &m_parent->m_componentStorage, 
            ENTITY_SLOT_MINIMAL
        );

        if(!it.isSlotTaken() && it.isInBounds())
        {
            ++it;
        }

        return it;
    }

    inline CEntityConstIterator CEntityWorld::EntityIteratorMethods::cend() const noexcept
    {
        auto it = CEntityConstIterator(
            &m_parent->m_componentStorage, 
            (entityslot_t)m_parent->m_componentStorage.maxSlot() + 1
        );

        return it;
    }



    
    inline tl::optional<CEntitySignature> CEntityWorld::getEntitySignature(CEntity entity) const
    {
        if (!isEntityAlive(entity))
        {
            return tl::nullopt;
        }
        
        return m_componentStorage.signature(entity.slot);
    }

    inline std::vector<CEntity> CEntityWorld::findEntities(std::function<bool(const CEntitySignature&)> pred) const
    {
        std::vector<CEntity> v;

        auto it = this->entityIterator.cbegin();
        auto end = this->entityIterator.cend();
        for(; it != end; ++it)
        {
            if (pred(it.signature()))
            {
                v.push_back(it.entity());
            }
        }

        return v;
    }
    





    inline void CEntityWorld::updateQuerySuppliersOnEntityChange( entityslot_t entitySlot, const CEntitySignature* prevSignature, const CEntitySignature* currSignature )
    {
        bool prevValid, currValid;

        for( auto& supplier : m_listQuerySuppliers )
        {
            if( prevSignature )
            {
                prevValid = supplier->testSignature( *prevSignature );
            }
            else
            {
                prevValid = false;
            }

            if( currSignature )
            {
                currValid = supplier->testSignature( *currSignature );
            }
            else
            {
                currValid = false;
            }

            if( !prevValid && currValid )
            {
                supplier->enqueueEntity( entitySlot );
            }
            else if( prevValid && !currValid )
            {
                supplier->dequeueEntity( entitySlot );
            }
        }
    }

} // namespace chestnut::ecs
