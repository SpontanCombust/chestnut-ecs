#pragma once

#include "types.hpp"
#include "component_storage.hpp"
#include "entity_signature.hpp"

#include <functional>
#include <vector>

namespace chestnut::ecs::internal
{
    /**
     * @brief Class that manages distribution of entity IDs and provides helper functions to find certain entities 
     */
    class CEntityRegistry
    {
    private:
        /**
         * @brief Pointer to the component storage
         */
        const CComponentStorage *m_componentStoragePtr;

        /**
         * @brief Counter for distributing new entity ID
         */
        entityid_t m_entityIdCounter;
        //TODO make it be sorted
        /**
         * @brief Vector of freed entity IDs 
         */
        std::vector< entityid_t > m_vecRecycledEntityIDs;


    public:
        /**
         * @brief Constructor
         */
        CEntityRegistry(const CComponentStorage *componentStorage) noexcept;


        /**
         * @brief Create a record of a new entity and return its ID
         * 
         * @param canRecycleId specifies whether the registry can reuse the ID of a previously unregistered entity
         * 
         * @return registered entity ID
         */
        entityid_t registerNewEntity(bool canRecycleId = true) noexcept;

        /**
         * @brief Returns whether an entity with this id is registered
         * 
         * @param id ID of the entity
         * 
         * @return true if entity record has been found
         * @return false otherwise
         */
        bool isEntityRegistered(entityid_t id) const noexcept;

        /**
         * @brief Remove record of entity with ID
         * 
         * @param id ID of the entity
         */
        void unregisterEntity(entityid_t id) noexcept;


        
        /**
         * @brief Get the value of the internal ID counter
         * 
         * @return highest id or ENTITY_ID_INVALID if no entities have been registered yet
         */
        entityid_t getHighestIdRegistered() const noexcept;

        /**
         * @brief Get the amount of all registered entities
         * 
         * @return entity count
         */
        entitysize_t getEntityCount() const noexcept;

        /**
         * @brief Get the amount of all registered entities with the exact signature as given
         * 
         * @param requiredSignature signature
         * 
         * @return entity count
         */
        entitysize_t getEntityCountOfExactSignature( const CEntitySignature& requiredSignature) const noexcept;

        /**
         * @brief Get the amount of all registered entities, which signature is includes types in requiredSignaturePart
         * 
         * @param requiredSignaturePart signature
         * 
         * @return entity count
         */
        entitysize_t getEntityCountOfPartialSignature( const CEntitySignature& requiredSignaturePart) const noexcept;


        /**
         * @brief Get the signature of the entity
         * 
         * @param id entity ID
         * @return entity's signature or an empty signature if entity is not registered
         */
        CEntitySignature getEntitySignature(entityid_t id) const noexcept;

        /**
         * @brief Get a vector of entities which signature complies with the predicate
         * 
         * @param pred signature predicate
         * 
         * @return vector of entity IDs
         */
        std::vector<entityid_t> findEntities(std::function<bool(const CEntitySignature&)> predicate) const noexcept;
    };
    
} // namespace chestnut::ecs::internal


#include "entity_registry.inl"
