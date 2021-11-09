#ifndef __CHESTNUT_ECS_COMPONENT_STORAGE_SEGMENT_H__
#define __CHESTNUT_ECS_COMPONENT_STORAGE_SEGMENT_H__

#include "types.hpp"
#include "component_wrapper.hpp"

#include <vector>

namespace chestnut::ecs::internal
{
    class CComponentStorageSegment_Base
    {
    protected:
        segsize_t m_size;
        entityid_t *m_arrEntityIDs;

        std::vector< segsize_t > m_vecAvailableIndices;

    public:
        CComponentStorageSegment_Base( segsize_t size );
        CComponentStorageSegment_Base( const CComponentStorageSegment_Base& ) = delete; // we don't copy segments
        ~CComponentStorageSegment_Base();

        segsize_t getSize() const;
        bool isEmpty() const;
        bool isFull() const;

        // Returns the number of segment slots taken up by entity components
        segsize_t getTakenSlotCount() const;

        // Returns whether entity with id equal entityID has its component slotted in this segment 
        bool hasSlottedComponent( entityid_t entityID ) const;

    };




    template< class C >
    class CComponentStorageSegment : public CComponentStorageSegment_Base
    {
    private:
        SComponentWrapper<C> *m_arrComponentSlots;

    public:
        CComponentStorageSegment( segsize_t size );
        CComponentStorageSegment( const CComponentStorageSegment& ) = delete; // we don't copy segments
        ~CComponentStorageSegment();

        
        // Returns the slotted component or null if there was no place left for it
        // If entity already has taken up a slot, no action is taken aside from returning component at that slot
        // Asserts entityID is not ENTITY_ID_INVALID
        SComponentWrapper<C>* tryTakeUpSlot( entityid_t entityID );

        // Returns the slotted component or null if there was no place left for it
        // If entity already has taken up a slot, no action is taken aside from returning component at that slot
        // Copies contents from copySrc into the slotted component
        // Asserts entityID is not ENTITY_ID_INVALID
        SComponentWrapper<C>* tryTakeUpSlot( entityid_t entityID, const SComponentWrapper<C>* copySrc );

        // Returns null if entity component is not slotted
        SComponentWrapper<C>* getSlottedComponent( entityid_t entityID ) const;

        // Nothing is done if entity doesn't take up any slot
        void freeSlot( entityid_t entityID );
        
        void clearSlots();
    };

} // namespace chestnut::ecs::internal


#include "component_storage_segment.tpp"


#endif // __CHESTNUT_ECS_COMPONENT_STORAGE_SEGMENT_H__