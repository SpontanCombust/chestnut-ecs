/**
 * @file entity_world.hpp
 * @author Przemysław Cedro (SpontanCombust)
 * @brief Header file for the main class of the library - Entity World
 * @version 1.0
 * @date 2021-12-02
 * 
 * @copyright Copyright (c) 2021
 * 
 */


#pragma once

#include "types.hpp"
#include "component_storage.hpp"
#include "entity_registry.hpp"
#include "entity_query_guard.hpp"
#include "entity_query.hpp"
#include "entity_iterator.hpp"
#include "component_handle.hpp"

#include <string>
#include <list>

namespace chestnut::ecs
{
    class CEntityWorld
    {
    private:
        /**
         * @brief Storage object for the components
         */
        mutable internal::CComponentStorage m_componentStorage;

        /**
         * @brief Object that distributes entity IDs and provides helper lookup functions into storage
         */
        internal::CEntityRegistry m_entityRegistry;


        /**
         * @brief A map of query guards, that is, objects responsible for buffering component data for the actual queries (that they store)
         * 
         * @details
         * Query guards are mutable, because we cache pending components inside them and want to update them when calling update on query.
         * This action doesn't affect World itself.
         */
        mutable std::list<std::unique_ptr<internal::CEntityQueryGuard>> m_listQuerySuppliers;



    public:
        /**
         * @brief Constructor
         */
        CEntityWorld();

        // World is too heavy to have it be able to be copied
        CEntityWorld(const CEntityWorld&) = delete;

        /**
         * @brief Destructor
         * 
         * @details
         * Takes care of cleaning out any left components
         */
        ~CEntityWorld();


        CEntity createEntity();

        template<typename ...Components>
        CEntity createEntity(Components&&... data);

        bool isEntityAlive(CEntity entity) const;

        void destroyEntity(CEntity entity);



        //TODO stronger typing (not primitive or pointer type and more) for components, use concept
        // Returns null if entity doesn't exist
        template<typename C>
        tl::expected<CComponentHandle<C>, std::string> insertComponent(CEntity entity, C&& data);

        template<typename C>
        bool hasComponent(CEntity entity) const;

        // Returns null if entity doesn't exist or if it doesn't own that component
        // Otherwise returns component owned by the entity
        template<typename C>
        tl::expected<CComponentHandle<C>, std::string> getComponent(CEntity entity) const;

        template<typename C>
        void destroyComponent(CEntity entity);

        


        CEntityQuery createQuery(const CEntitySignature& requireSignature, const CEntitySignature& rejectSignature);
        CEntityQuery createQuery(const CEntitySignature& requireSignature);

        void destroyQuery(CEntityQuery& query);


        class EntityIteratorMethods
        {
        private:
            CEntityWorld *m_parent;

        public:
            EntityIteratorMethods(CEntityWorld *parent) : m_parent(parent) {}

            CEntityIterator begin() noexcept;
            CEntityIterator end() noexcept;
            CEntityConstIterator cbegin() const noexcept;
            CEntityConstIterator cend() const noexcept;
            
        } entityIterator;


        tl::optional<CEntitySignature> getEntitySignature(CEntity entity) const;

        // Simpler form of querying for entities, where you only get their IDs when
        // Use this only when you'll be looking for entities non-frequently. Otherwise use regular queries
        std::vector<CEntity> findEntities(std::function<bool(const CEntitySignature&)> predicate) const;


    private:
        // If null passed for signature, it is interpreted as that the signature is definitely empty
        void updateQuerySuppliersOnEntityChange(entityslot_t entitySlot, const CEntitySignature* prevSignature, const CEntitySignature* currSignature);
    };

} // namespace chestnut::ecs


#include "entity_world.inl"
