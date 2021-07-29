#ifndef __CHESTNUT_COMPONENT_STORAGE_SEGMENT_H__
#define __CHESTNUT_COMPONENT_STORAGE_SEGMENT_H__

#include "types.hpp"
#include "component.hpp"

#include <string>
#include <unordered_map>
#include <vector>

namespace chestnut::internal
{
    class CComponentStorageSegment_Base
    {
    protected:
        segsize m_size;

        std::vector< segsize > m_vecAvailableIndices;
        std::unordered_map< entityid, segsize > m_mapEntityIDToIndex;

    public:
        CComponentStorageSegment_Base( segsize size );
        CComponentStorageSegment_Base( const CComponentStorageSegment_Base& ) = delete; // we don't copy segments

        segsize getSize() const;
        bool isEmpty() const;
        bool isFull() const;

        // Returns the number of segment slots taken up by entity components
        segsize getTakenSlotCount() const;

        // Returns whether entity with id equal entityID has its component slotted in this segment 
        bool hasSlottedComponent( entityid entityID ) const;

        // Returns index at which entity's component is stored
        // Throws std::out_of_range exception if doesn't find the entity
        segsize getIndexByEntity( entityid entityID ) const;

        // Returns a string with IDs of all entities that take up slots in the segment
        std::string toString() const;
    };




    template< class C >
    class CComponentStorageSegment : public CComponentStorageSegment_Base
    {
        ASSERT_DERIVED_FROM_COMPONENT(C);

    private:
        C *m_arrComponentSlots;

    public:
        CComponentStorageSegment( segsize size );
        CComponentStorageSegment( const CComponentStorageSegment& ) = delete; // we don't copy segments
        ~CComponentStorageSegment();

        
        // Returns the slotted component or null if there was no place left for it
        // If entity already has taken up a slot, no action is taken aside from returning component at that slot
        // If entityID is equal to ENTITY_ID_INVALID returns null
        C* tryTakeUpSlot( entityid entityID );

        // Returns null if entity component is not slotted
        C* getComponentByEntity( entityid entityID ) const;

        // Throws std::out_of_range exception if index exceeds size of the segment
        C* getComponentByIndex( segsize index ) const;

        // Returns component at index
        // Throws std::out_of_range exception if index exceeds size of the segment
        C* operator[]( segsize index ) const;

        // Nothing is done if entity doesn't take up any slot
        void freeSlotByEntity( entityid entityID );

        // Throws std::out_of_range exception if index exceeds size of the segment
        void freeSlotByIndex( segsize index );
        
        void clearSlots();
    };

} // namespace chestnut::internal


#include "component_storage_segment.tpp"


#endif // __CHESTNUT_COMPONENT_STORAGE_SEGMENT_H__