/**
 * @file entity_registry.hpp
 * @author Przemysław Cedro (SpontanCombust)
 * @brief Header file for the internal entity registry class
 * @version 1.0
 * @date 2021-11-30
 * 
 * @copyright Copyright (c) 2021
 * 
 */


#pragma once

#include "types.hpp"
#include "constants.hpp"
#include "entity_signature.hpp"

#include <deque>
#include <functional>
#include <vector>

namespace chestnut::ecs::internal
{
    /**
     * @brief Entity record data used in the registry
     */
    struct SEntityRegistryRecord
    {
        /** @brief If the record is actively used by some entity */
        bool isIdUsed;
        /** @brief If the entity is a template entity */
        bool isTemplate;
        /** @brief Signature of the entity */
        CEntitySignature signature;
    };

    /**
     * @brief Class storing metadata about entities e.g. which types of components they own
     */
    class CEntityRegistry
    {
    private:
        /**
         * @brief Counter for distributing new entity ID
         */
        entityid_t m_entityIdCounter;
        /**
         * @brief Vector of freed entity IDs 
         */
        std::vector< entityid_t > m_vecRecycledEntityIDs;

    public:
        /**
         * @brief Flags to use when calling searching methods in the registry
         */
        enum ESearchFlags
        {
            /** @brief If should look through non-template entites */
            CAN_BE_REGULAR_ENTITY   = 1,
            /** @brief If should look through template entities */
            CAN_BE_TEMPLATE_ENTITY  = 2
        };

        /**
         * @brief Deque of entity records
         */
        std::deque< SEntityRegistryRecord > m_dequeEntityRecords; // can't do it with vector if signatures get passed outside by reference

    public:
        /**
         * @brief Constructor
         */
        CEntityRegistry();


        /**
         * @brief Create a record of a new entity and return its ID
         * 
         * @param isTemplateEntity if entity is a template entity
         * @return registered entity ID
         */
        entityid_t registerNewEntity( bool isTemplateEntity );

        /**
         * @brief Create a record of a new entity, set a signature for it and return its ID
         * 
         * @param isTemplateEntity if entity is a template entity
         * @param signature entity signature to immediately set
         * @return registered entity ID
         */
        entityid_t registerNewEntity( bool isTemplateEntity, const CEntitySignature& signature );


        /**
         * @brief Update entity's signature
         * 
         * @details Ignores request if entity is not yet registered.
         * 
         * @param id entity ID
         * @param newSignature new entity signature 
         */
        void updateEntity( entityid_t id, const CEntitySignature& newSignature );


        /**
         * @brief Returns whether an entity with this data is registered
         * 
         * @param id ID of the entity
         * @param searchFlags flags dictating which entities should be included in the search
         * @return true if entity record has been found
         * @return false otherwise
         * 
         * @see ESearchFlags
         */
        bool hasEntity( entityid_t id, int searchFlags = CAN_BE_REGULAR_ENTITY | CAN_BE_TEMPLATE_ENTITY ) const;

        /**
         * @brief Remove record of entity with ID
         * 
         * @param id ID of the entity
         */
        void unregisterEntity( entityid_t id );


        
        /**
         * @brief Get the amount of all registered entities
         * 
         * @param searchFlags flags dictating which entities should be included in the search
         * @return entity count
         * 
         * @see ESearchFlags
         */
        entitysize_t getEntityCount( int searchFlags = CAN_BE_REGULAR_ENTITY | CAN_BE_TEMPLATE_ENTITY ) const;

        /**
         * @brief Get the amount of all registered entities with the exact signature as given
         * 
         * @param requiredSignature signature
         * @param searchFlags flags dictating which entities should be included in the search
         * @return entity count
         * 
         * @see ESearchFlags
         */
        entitysize_t getEntityCountOfExactSignature( const CEntitySignature& requiredSignature, int searchFlags = CAN_BE_REGULAR_ENTITY | CAN_BE_TEMPLATE_ENTITY ) const;

        /**
         * @brief Get the amount of all registered entities, which signature is includes types in requiredSignaturePart
         * 
         * @param requiredSignaturePart signature
         * @param searchFlags flags dictating which entities should be included in the search
         * @return entity count
         * 
         * @see ESearchFlags
         */
        entitysize_t getEntityCountOfPartialSignature( const CEntitySignature& requiredSignaturePart, int searchFlags = CAN_BE_REGULAR_ENTITY | CAN_BE_TEMPLATE_ENTITY ) const;


        /**
         * @brief Get the signature of the entity
         * 
         * @param id entity ID
         * @return const pointer to signature or null if entity is not registered
         */
        const CEntitySignature* getEntitySignature( entityid_t id ) const;

        /**
         * @brief Get a vector of entities which signature complies with the predicate
         * 
         * @details This doesn't take template entities into account
         * 
         * @param pred signature predicate
         * @param searchFlags flags dictating which entities should be included in the search
         * @return vector of entity IDs
         * 
         * @see ESearchFlags
         */
        std::vector< entityid_t > findEntities( std::function< bool( const CEntitySignature& ) > pred, int searchFlags = CAN_BE_REGULAR_ENTITY | CAN_BE_TEMPLATE_ENTITY ) const;


    private:
        /**
         * @brief Turn entry deque index into entity ID
         * 
         * @param index deque index
         * @return entity ID
         */
        entityid_t entityIdFromIndex( entityid_t index ) const;
        /**
         * @brief Turn entity ID into entry deque index
         * 
         * @param id entity ID
         * @return entry deque index
         */
        entityid_t indexFromEntityId( entityid_t id ) const;
    };
    
} // namespace chestnut::ecs::internal


#include "entity_registry.inl"
