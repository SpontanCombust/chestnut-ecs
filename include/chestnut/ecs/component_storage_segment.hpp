/**
 * @file component_storage_segment.hpp
 * @author Przemysław Cedro (SpontanCombust)
 * @brief Header for the internal implementation of storage segment
 * @version 1.0
 * @date 2021-11-30
 * 
 * @copyright Copyright (c) 2021
 * 
 */


#pragma once

#include "types.hpp"
#include "component_wrapper.hpp"

#include <vector>

namespace chestnut::ecs::internal
{
    /**
     * @brief Storage segment base class
     * 
     */
    class CComponentStorageSegment_Base
    {
    protected:
        /**
         * @brief Size of the underlying component array
         * 
         */
        segsize_t m_size;
        /**
         * @brief Array of tracked entity IDs
         * 
         */
        entityid_t *m_arrEntityIDs;

        /**
         * @brief Vector of freed indices in the array
         * 
         */
        std::vector< segsize_t > m_vecAvailableIndices;

    public:
        /**
         * @brief Constructor
         * 
         * @param size size of the underlying array
         */
        CComponentStorageSegment_Base( segsize_t size ) noexcept;
        /**
         * @brief Destructor; frees memory entity ID array
         * 
         */
        ~CComponentStorageSegment_Base();

        /**
         * @brief Get segment size
         * 
         * @return segsize_t segment size
         */
        segsize_t getSize() const noexcept;
        /**
         * @brief Return if segment is empty
         * 
         * @return true if all slots are free
         * @return false if at least one slot is not free
         */
        bool isEmpty() const noexcept;
        /**
         * @brief Return if segment is full
         * 
         * @return true if all slots are taken
         * @return false if at least one slot is not taken
         */
        bool isFull() const noexcept;

        /**
         * @brief Get the number of taken slots
         * 
         * @return segsize_t taken slots quanity
         */
        segsize_t getTakenSlotCount() const noexcept;

        /**
         * @brief Returns whether a component belonging to the entity is slotted in the segment
         * 
         * @param entityID entity ID
         * @return true ifcomponent is slotted
         * @return false if component is not slotted
         */
        bool hasSlottedComponent( entityid_t entityID ) const noexcept;

    };




    /**
     * @brief Storage segment template class
     * 
     * @tparam C type of the component
     */
    template< class C >
    class CComponentStorageSegment : public CComponentStorageSegment_Base
    {
    private:
        /**
         * @brief Underlying component (wrapper) array
         * 
         */
        SComponentWrapper<C> *m_arrComponentSlots;

    public:
        /**
         * @brief Constructor
         * 
         * @param size size of the underlying array
         */
        CComponentStorageSegment( segsize_t size ) noexcept;
        /**
         * @brief Destructor; frees underlying component array
         * 
         */
        ~CComponentStorageSegment();

        
        /**
         * @brief Construct an instance of component (wrapper)
         * 
         * @details
         * Null is returned if entity ID is invalid or if there are no free slots.
         * 
         * @param entityID ID of the entity
         * @return Newly slotted component wrapper or null or error
         */
        SComponentWrapper<C>* tryTakeUpSlot( entityid_t entityID ) noexcept;

        /**
         * @brief Construct an instance of component (wrapper) and copy to it from copySrc; if copySrc is null - always returns null
         * 
         * @details
         * Null is returned if entity ID is invalid, if there are no free slots or if copySrc is null.
         * 
         * @param entityID ID of the entity
         * @param copySrc pointer to the component to copy from
         * @return Newly slotted component wrapper or null or error
         */
        SComponentWrapper<C>* tryTakeUpSlot( entityid_t entityID, const SComponentWrapper<C>* copySrc ) noexcept;

        /**
         * @brief Find a return a component belonging to the entity
         * 
         * @param entityID ID of the entity
         * @return component wrapper if found, null otherwise 
         */
        SComponentWrapper<C>* getSlottedComponent( entityid_t entityID ) const noexcept;

        /**
         * @brief Erase component belonging to entity from segment
         * 
         * @details
         * If entity doesn't have the component slotted in this segment, nothing is done
         * 
         * @param entityID ID of the entity
         */
        void freeSlot( entityid_t entityID ) noexcept;
        
        /**
         * @brief Clear all components from the segment
         * 
         */
        void clearSlots() noexcept;
    };

} // namespace chestnut::ecs::internal


#include "component_storage_segment.inl"
