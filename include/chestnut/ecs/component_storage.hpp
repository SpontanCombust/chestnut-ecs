/**
 * @file component_storage.hpp
 * @author Przemysław Cedro (SpontanCombust)
 * @brief Header file for the internal component storage implementation
 * @version 1.0
 * @date 2021-11-30
 * 
 * @copyright Copyright (c) 2021
 * 
 */


#pragma once

#include "types.hpp"
#include "component_wrapper.hpp"
#include "component_storage_segment.hpp"

#include <deque>
#include <typeindex>
#include <unordered_map>

namespace chestnut::ecs::internal
{
    /**
     * @brief Interface class for CComponentStorage.
     */
    class IComponentStorage
    {
    protected:
        IComponentStorage() = default;

    public:
        virtual ~IComponentStorage() = default;

        virtual segsize_t getSegmentSize() const = 0;

        // Returns created component or component that has already existed with that entityID
        virtual IComponentWrapper *storeComponent( entityid_t entityID ) = 0;
        // Returns created component or component that has already existed with that entityID
        // Attempts to copy contents from entity srcEntityID to entityID if entity with srcEntityID exists
        virtual IComponentWrapper *storeComponentCopy( entityid_t entityID, entityid_t srcEntityID ) = 0;
        virtual bool hasComponent( entityid_t entityID ) const = 0;
        // Returns null if component doesn't exist
        virtual IComponentWrapper *getComponent( entityid_t entityID ) const = 0;
        virtual void eraseComponent( entityid_t entityID ) = 0;
        // Erases all stored components; doesn't modify capacity
        virtual void clearComponents() = 0;

        // Allocates (if necessary) memory for the minimal number of segments 
        // that can fit in total specified number of components.
        // If specified totalSize is smaller than current capacity nothing is done.
        virtual void reserve( segsize_t totalSize ) = 0;
        // Allocates additional memory for specified amount of components
        // The amount of memory allocated also depends on the size of segment setting
        virtual void reserveAdditional( segsize_t additionalSize ) = 0;
        // Similair to reserve() with an exception that it attempts to deallocate memory
        // if specified targetSize is smaller than capacity 
        virtual void resize( segsize_t targetSize ) = 0;
        // Reduces the capacity by deallocating all segments that don't store any active components at the moment
        virtual void removeEmptySegments() = 0;

        // Returns the number of stored components
        virtual segsize_t getSize() const = 0;
        // Returns the total number of components that can be stored 
        virtual segsize_t getCapacity() const = 0;
        // Returns collective size of empty segments
        virtual segsize_t getEmptySegmentTotalSize() const = 0;   
    };

    /**
     * @brief Component storage class keeping its data in constant size segments
     * 
     * @tparam C type of component
     * 
     * @details
     * Can store components in a series of contiguous segments. Segments between each other are not necesairly contiguous.
     * Memory for these segments never has to be moved. It only needs to be allocated and deallocated (during storage's 
     * lifetime this is optional).
     * 
     * Segmenting the storage allows for relative balance between speed and being able to access data using pointers.
     * The problem with using simple vector for this comes with that last part. Let's take simple situation:
     * at the beginning of the tick user decides to cache some component pointer. During the tick user creates a lot
     * of components. If the amount of these new ones exceeds the capacity of the vector the memory has to be reallocated
     * for bigger size. This can invalidate the pointer we talked about. This does not need to happen accross the whole tick.
     * You can simply have a situation where one thread is caching component and the other is creating them.
     * I decided to do it this way and focus on making sure that important objects like component batches will have their contents modified 
     * whenever component creation/eraseure happens. Using indices would be safer, but would require more preperation.
     * 
     * Using direct pointers is also more user friendly and transparent when it comes to code outside of systems, like scripts in my opinion. 
     * The only thing to remember is that user is not the *direct* owner of components themselves, so it's safer to repeat fetching components
     * (and checking if they're valid) from these places on every tick rather than clinging onto them for an unspecified amount of time. 
     * 
     * User does not need to care how the data is stored - all he/she cares about is that no weird stuff will happen to pointers they cached during a tick
     * if they decide to create some components and forget to explicitly preallocate more memory before that.
     * In practice this allows for safely caching pointers not just for the duration of a tick, but for the entire lifetime 
     * of the component.
     * 
     * The segment size set for the storage depends solely on how frequently and in which quantity components of given type have to be created.
     * You can even specify a very large segment size in the constructor and allocate memory for components only once, and if this amount
     * happens to run out anyways, nothing too bad will happen anyways.
     * When specifying segment size strike a balance between how big it will be and in which quantities will be created. It general it's best
     * to keep the number of segments that will have to be created to minimum, but at the same time think not to make these too big to be able
     * to load them from memory more easily.
     * 
     * This class works a bit similair to how std::deque is implemented, but here we can specify the size of allocated arrays.
     */
    template< class C >
    class CComponentStorage : public IComponentStorage
    {
    private:
        /**
         * @brief Segment type typedef
         */
        using SegType = internal::CComponentStorageSegment<C>;

        /**
         * @brief Size of each segment
         */
        segsize_t m_segmentSize;
        /**
         * @brief ID counter for segments
         */
        segid_t m_segmentIDCounter;

        /**
         * @brief Map with segment ID key and segment pointer value
         */
        std::unordered_map< segid_t, SegType * > m_mapSegmentIndexToSegment;
        /**
         * @brief Deque of segment IDs that are not full
         */
        std::deque< segid_t > m_dequeAvailableSegmentIndices;

    public:
        /**
         * @brief Constructor
         * 
         * @param segmentSize size of each segment 
         * @param initCapacity initial capacity of the storage
         */
        CComponentStorage( segsize_t segmentSize, segsize_t initCapacity );
        /**
         * @brief Destructor; frees stored segments
         */
        ~CComponentStorage();

        /**
         * @brief Get size of a storage segment
         * 
         * @return segsize_t size of segment
         */
        segsize_t getSegmentSize() const override;

        /**
         * @brief Construct a component
         * 
         * @param entityID ID of the entity
         * @return upcasted pointer to newly constructed component or one that already exists
         */
        IComponentWrapper *storeComponent( entityid_t entityID ) override;
        /**
         * @brief Construct a component with data copied from other component
         * 
         * @param entityID ID of the entity
         * @param srcEntityID ID of the enity to copy from 
         * @return upcasted pointer to newly constructed component or one that already exists
         */
        IComponentWrapper *storeComponentCopy( entityid_t entityID, entityid_t srcEntityID ) override;
        /**
         * @brief Returns whether storage holds component for specified entity
         * 
         * @param entityID ID of the entity
         * @return true if component exists
         * @return false otherwise
         */
        bool hasComponent( entityid_t entityID ) const override;
        /**
         * @brief Returns pointer to component owned by entity or null if it doesn't exist
         * 
         * @param entityID ID of the owner entity
         * @return upcasted pointer to component wrapper or null if it doesn't exist
         */
        IComponentWrapper *getComponent( entityid_t entityID ) const override;
        /**
         * @brief Erase component belonging to entity from storage
         * 
         * @param entityID ID of the entity
         */
        void eraseComponent( entityid_t entityID ) override;
        /**
         * @brief Clear all components in the storage; doesn't modify capacity
         */
        void clearComponents() override;

        /**
         * @brief Allocates (if necessary) memory for the minimal number of segments that can fit in total specified number of components
         * 
         * @details If specified totalSize is smaller than current capacity nothing is done.
         * 
         * @param totalSize the total amount of components for which storage should have allocated memory 
         */
        void reserve( segsize_t totalSize ) override;
        /**
         * @brief Allocates additional memory for the minimal number of segments that can fit specified additional number of components
         * 
         * @param additionalSize the amount of components storage should allocate memory for 
         */
        void reserveAdditional( segsize_t additionalSize ) override;
        /**
         * @brief Similair to reserve() with an exception that it attempts to deallocate memory if specified targetSize is smaller than capacity 
         * 
         * @param targetSize the target amount of components for which storage should have allocated memory 
         */
        void resize( segsize_t targetSize ) override;
        /**
         * @brief Reduces the capacity by deallocating all segments that don't store any active components at the moment
         */
        void removeEmptySegments() override;

        /**
         * @brief Returns the number of stored components
         * 
         * @return stored components amount 
         */
        segsize_t getSize() const override;
        /**
         * @brief Returns the total number of components that can be stored 
         * 
         * @return total number of components that can be stored  
         */
        segsize_t getCapacity() const override;
        /**
         * @brief Returns collective size of empty segments
         * 
         * @return collective size of empty segments 
         */
        segsize_t getEmptySegmentTotalSize() const override;

    private:
        /**
         * @brief Create and returns ID of new segment
         * 
         * @return new segment ID 
         */
        segid_t createNewSegment();
    };


    /**
     * @brief Typedef of map with type for key and a upcasted pointer to component storage as value
     */
    typedef std::unordered_map< std::type_index, IComponentStorage * > CComponentStorageTypeMap;


} // namespace chestnut::ecs::internal


#include "component_storage.inl"
