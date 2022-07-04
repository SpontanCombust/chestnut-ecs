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
#include "entity_registry.hpp"
#include "component_storage.hpp"
#include "entity_query_guard.hpp"
#include "component_handle.hpp"
#include "entity_query.hpp"
#include "component_wrapper.hpp"
#include "component_traits.hpp"

#include <shared_mutex>
#include <unordered_map>

namespace chestnut::ecs
{
    class CEntityWorld
    {
    private:
        /**
         * @brief A bookkeeping object used to keep track of created entities and their signatures
         */
        internal::CEntityRegistry m_entityRegistry;

        /**
         * @brief A map pointing to direct component storages by the type of the component they store
         */
        internal::CComponentStorageTypeMap m_mapCompTypeToStorage;

        /**
         * @brief A map of query guards, that is, objects responsible for buffering component data for the actual queries (that they store)
         * 
         * @details
         * Query guards are mutable, because we cache pending components inside them and want to update them when calling update on query.
         * This action doesn't affect World itself.
         */
        mutable std::unordered_map< queryid_t, internal::CEntityQueryGuard* > m_mapQueryIDToQueryGuard;

        /**
         * @brief Shared mutex that can be used for synchronizing actions on the world between threads
         */
        mutable std::shared_mutex m_mutex;

    public:
        /**
         * @brief Write lock typedef for World's mutex
         */
        typedef std::unique_lock< std::shared_mutex > WriteLock;
        /**
         * @brief Read lock typedef for World's mutex
         */
        typedef std::shared_lock< std::shared_mutex > ReadLock;


    public:
        /**
         * @brief Constructor
         */
        CEntityWorld();

        /**
         * @brief World is too heavy to have it be able to be copied
         */
        CEntityWorld( const CEntityWorld& ) = delete;

        /**
         * @brief Destructor
         * 
         * @details
         * Takes care of cleaning out any left components
         */
        ~CEntityWorld();


        // To know how to write traits and for more information, see 
        /**
         * @brief Initialize a component type that is to be used by the World
         * 
         * @tparam C component type
         * @tparam Traits component traits
         * 
         * @details
         * Traits describe how memory for this component type is managed.
         * You can setup component type only once.
         * Make sure to use this function before ever using this component type with other functions,
         * because they may do it automatically, but with default traits.
         * 
         * @see ComponentTraits
         */
        template< typename C, typename Traits = chestnut::ecs::ComponentTraits<C> >
        void setupComponentType();


        /**
         * @brief Create a new entity and return its ID
         * 
         * @return entity ID
         */
        entityid_t createEntity();

        /**
         * @brief Create multiple new entities and return a vector of their IDs
         * 
         * @param amount number of entities
         * @return vector of entity IDs
         */
        std::vector<entityid_t> createEntities( entitysize_t amount );

        /**
         * @brief Checks if entity with that ID exists
         * 
         * @param entityID ID of the entity
         * @return true if entity exists
         * @return false otherwise
         */
        bool hasEntity( entityid_t entityID ) const;

        /**
         * @brief Erase entity and all components that belong to it 
         * 
         * @param entityID ID of the entity
         */
        void destroyEntity( entityid_t entityID );

        /**
         * @brief Erase multiple entities
         * 
         * @param entityIDs vector of entity IDs
         */
        void destroyEntities( const std::vector<entityid_t>& entityIDs );



        // Returns null if entity doesn't exist
        // Returns existing component if it was already created before
        // Otherwise returns newly created component
        // If you haven't setup this component type before, its traits will be made default
        // You can't setup custom traits after that
        template< typename C >
        CComponentHandle<C> createComponent( entityid_t entityID );

        template< typename C >
        bool hasComponent( entityid_t entityID ) const;

        // Returns null if entity doesn't exist or if it doesn't own that component
        // Otherwise returns component owned by the entity
        template< typename C >
        CComponentHandle<C> getComponent( entityid_t entityID ) const;

        template< typename C >
        void destroyComponent( entityid_t entityID );



        entityid_t createTemplateEntity();

        bool hasTemplateEntity( entityid_t templateEntityID ) const;

        void destroyTemplateEntity( entityid_t templateEntityID );

        // Creates entity based on component data owned by entity template with given ID
        // If no such template entity exists doesn't create any new entity or components and returns ENTITY_ID_INVALID
        entityid_t createEntityFromTemplate( entityid_t templateEntityID );

        // Creates entity based on component data owned by entity template with given ID
        // If no such template entity exists doesn't create any new entity or components and returns empty vector
        std::vector<entityid_t> createEntitiesFromTemplate( entityid_t templateEntityID, entitysize_t amount );

        

        // Do so that the total of at least `amount` components of type C are allocated and available
        // Result of calling this function is dependant on the memory setup for component type
        // See component_traits.hpp
        // If you haven't setup this component type before, its traits will be made default
        // You can't setup custom traits after that
        template< typename C > 
        void reserveComponentMemoryTotal( entitysize_t amount );

        // Allocate `amount` components of type C
        // Result of calling this function is dependant on the memory setup for component type
        // See component_traits.hpp
        // If you haven't setup this component type before, its traits will be made default
        // You can't setup custom traits after that
        template< typename C > 
        void reserveComponentMemoryAdditional( entitysize_t amount );

        // Try to deallocate memory from the maximum of `amount` of components of type C
        // Result of calling this function is dependant on the memory setup for component type
        // See component_traits.hpp
        template< typename C >
        void freeComponentMemory( entitysize_t amount );



        queryid_t createQuery( const CEntitySignature& requireSignature, const CEntitySignature& rejectSignature );

        // Returns null if no query with this ID exists
        CEntityQuery* queryEntities( queryid_t id ) const;

        void destroyQuery( queryid_t id );



        // Simpler form of querying for entities, where you only get their IDs when
        // Use this only when you'll be looking for entities non-frequently. Otherwise use regular queries and do forEach with entity IDs.
        std::vector< entityid_t > findEntities( std::function< bool( const CEntitySignature& ) > pred ) const;


        WriteLock lockForWriting() const;

        ReadLock lockForReading() const;


    private:
        template< typename C >
        void setupComponentTypeIfDidntAlready();


        internal::IComponentWrapper* createComponentInternal( std::type_index compType, entityid_t entityID );

        bool hasComponentInternal( std::type_index compType, entityid_t entityID ) const;

        internal::IComponentWrapper* getComponentInternal( std::type_index compType, entityid_t entityID ) const;

        void destroyComponentInternal( std::type_index compType, entityid_t entityID );

        // If null passed for signature, it is interpreted as that the signature is definitely empty
        void updateQueriesOnEntityChange( entityid_t entity, const CEntitySignature* prevSignature, const CEntitySignature* currSignature );
    };

} // namespace chestnut::ecs


#include "entity_world.inl"
