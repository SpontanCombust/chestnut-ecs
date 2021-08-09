#ifndef __CHESTNUT_ENTITY_MANAGER_H__
#define __CHESTNUT_ENTITY_MANAGER_H__

#include "types.hpp"
#include "entity_signature.hpp"
#include "component.hpp"
#include "component_storage.hpp"
#include "entity_registry.hpp"
#include "component_batch.hpp"

#include <vector>

namespace chestnut
{
#ifndef CHESTNUT_DEFAULT_STORAGE_SEGMENT_SIZE
    #define CHESTNUT_DEFAULT_STORAGE_SEGMENT_SIZE 100
#endif

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
        std::vector< internal::CComponentBatchGuard > m_vecBatchGuards;


    public:
        CEntityWorld();

        CEntityWorld( const CEntityWorld& ) = delete; // we don't allow copying components

        ~CEntityWorld();


        template< class C >
        void setupComponentType( segsize segmentSize, segsize initCapacity );

        template< class C >
        void setupComponentType( segsize segmentSize );


        entityid createEntity();

        bool hasEntity( entityid entityID ) const;

        void destroyEntity( entityid entityID );

        void destroyEntities( const std::vector< entityid >& entityIDs );

        void destroyAllEntities();


        // TODO entity templates


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


        //TODO eighter make getter for storages or allow to reserve component space somehow

        //TODO queries


    private:
        CComponent *createComponentInternal( std::type_index compType, entityid entityID );

        bool hasComponentInternal( std::type_index compType, entityid entityID ) const;

        CComponent *getComponentInternal( std::type_index compType, entityid entityID ) const;

        void destroyComponentInternal( std::type_index compType, entityid entityID );

        // Throws std::invalid_argument if signature is empty
        // We don't make batches for empty signatures, so always check it before you call this function
        // In any other case, assures batch guard exists (creates one if there isn't one already) 
        internal::CComponentBatchGuard& getBatchGuardWithSignature( const CEntitySignature& signature );
    };
    
} // namespace chestnut


#include "entity_world.tpp"


#endif // __CHESTNUT_ENTITY_MANAGER_H__