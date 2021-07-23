#ifndef __CHESTNUT_COMPONENT_STORAGE_SEGMENT_H__
#define __CHESTNUT_COMPONENT_STORAGE_SEGMENT_H__

#include "types.hpp"
#include "component.hpp"

#include <string>
#include <unordered_map>
#include <vector>

namespace chestnut::ecs
{

    template< class C >
    class CComponentStorageSegment
    {
        ASSERT_DERIVED_FROM_COMPONENT(C);

    private:
        segid m_id;

        segsize m_size;
        C *m_arrComponentSlots;

        std::vector< segsize > m_vecAvailableIndices;
        std::unordered_map< entityid, segsize > m_mapEntityIDToIndex;

    public:
        CComponentStorageSegment( segid id, segsize size );
        CComponentStorageSegment( const CComponentStorageSegment& ) = delete; // we don't copy segments
        ~CComponentStorageSegment();

        segsize getID() const;

        segsize getSize() const;
        bool isEmpty() const;
        bool isFull() const;
        
        segsize getTakenSlotCount() const;
        // Returns index of the slot taken by the component
        // Throws std::out_of_range exception if no slots are available
        // If entity already has taken up a slot, no action is taken aside from returning its index
        segsize tryTakeUpSlot( entityid entityID );
        // Returns whether entity with id equal entityID has its component slotted in this segment 
        bool hasSlottedComponent( entityid entityID ) const;
        // Returns null if entity component is not slotted
        C* getComponentByEntity( entityid entityID ) const;
        // Throws std::out_of_range exception if index exceeds size of the segment
        C* getComponentByIndex( segsize index ) const;
        // Returns component at index
        // Throws std::out_of_range exception if index exceeds size of the segment
        C* operator[]( segsize index ) const;
        // Returns index at which entity's component is stored
        // Throws std::out_of_range exception if doesn't find the entity
        segsize getIndexByEntity( entityid entityID ) const;
        // Nothing is done if entity doesn't take up any slot
        void freeSlotByEntity( entityid entityID );
        // Throws std::out_of_range exception if index exceeds size of the segment
        void freeSlotByIndex( segsize index );
        void clearSlots();

        // Returns a string with IDs of all entities that take up slots in the segment
        std::string toString() const;
    };

} // namespace chestnut


#include "component_storage_segment.tpp"


#endif // __CHESTNUT_COMPONENT_STORAGE_SEGMENT_H__