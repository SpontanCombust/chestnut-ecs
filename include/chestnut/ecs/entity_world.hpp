#ifndef __CHESTNUT_ECS_ENTITY_WORLD_H__
#define __CHESTNUT_ECS_ENTITY_WORLD_H__

#include "types.hpp"
#include "entity_signature.hpp"
#include "component.hpp"
#include "component_storage.hpp"
#include "entity_registry.hpp"
#include "component_batch_guard.hpp"
#include "entity_query.hpp"

#include <vector>

namespace chestnut::ecs
{
    class CEntityWorld
    {
    private:
        // A counter used to distribute IDs to entities
        entityid m_idCounter;
        std::vector< entityid > m_vecRecycledIDs;

        // A bookkeeping object used to keep track of created entities and their signatures
        internal::CEntityRegistry m_entityRegistry;

        // A map pointing to direct component storages by the type of the component they store
        internal::CComponentStorageTypeMap m_mapCompTypeToStorage;

        // A vector of batch objects organizing components by entity signature
        // They're mutable, because we cache pending components inside them and want to update them when performing a query
        // This doesn't affect World's state
        mutable std::vector< internal::CComponentBatchGuard > m_vecBatchGuards;


    public:
        CEntityWorld();

        CEntityWorld( const CEntityWorld& ) = delete; // we don't allow copying components

        ~CEntityWorld();



        entityid createEntity();

        std::vector< entityid > createEntities( unsigned int amount );

        bool hasEntity( entityid entityID ) const;

        void destroyEntity( entityid entityID );

        void destroyEntities( const std::vector< entityid >& entityIDs );



        // Returns null if entity doesn't exist
        // Returns existing component if it was already created before
        // Otherwise returns newly created component
        template< class C >
        C *createComponent( entityid entityID );

        template< class C >
        bool hasComponent( entityid entityID ) const;

        // Returns null if entity doesn't exist or if it doesn't own that component
        // Otherwise returns component owned by the entity
        template< class C >
        C *getComponent( entityid entityID ) const;

        template< class C >
        void destroyComponent( entityid entityID );



        entityid createTemplateEntity();

        bool hasTemplateEntity( entityid templateEntityID ) const;

        void destroyTemplateEntity( entityid templateEntityID );



        // Creates entity based on component data owned by entity template with given ID
        // If no such template entity exists doesn't create any new entity or components and returns ENTITY_ID_INVALID
        entityid createEntityFromTemplate( entityid templateEntityID );

        // Creates entity based on component data owned by entity template with given ID
        // If no such template entity exists doesn't create any new entity or components and returns empty vector
        std::vector< entityid > createEntitiesFromTemplate( entityid templateEntityID, unsigned int amount );



        //TODO eighter make getter for storages or allow to reserve component space somehow; also gettable segment size

        // Returns the number of entity variations (and thus batches) queried
        int queryEntities( SEntityQuery& query ) const;


    private:
        template< class C >
        void setupComponentTypeIfDidntAlready();


        entityid getNewEntityID();


        CComponent *createComponentInternal( std::type_index compType, entityid entityID );

        bool hasComponentInternal( std::type_index compType, entityid entityID ) const;

        CComponent *getComponentInternal( std::type_index compType, entityid entityID ) const;

        void destroyComponentInternal( std::type_index compType, entityid entityID );


        // Throws std::invalid_argument if signature is empty
        // We don't make batches for empty signatures, so always check it before you call this function
        // In any other case, assures batch guard exists (creates one if there isn't one already) 
        internal::CComponentBatchGuard& getBatchGuardWithSignature( const CEntitySignature& signature );
    };
    
} // namespace chestnut::ecs


#include "entity_world.tpp"


#endif // __CHESTNUT_ECS_ENTITY_WORLD_H__