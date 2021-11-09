#include "chestnut/ecs/component_storage_segment.hpp"

#include "chestnut/ecs/constants.hpp"

#include <cstring> // memset
#include <numeric> // iota

namespace chestnut::ecs::internal
{    
    CComponentStorageSegment_Base::CComponentStorageSegment_Base( segsize_t size ) 
    {
        m_size = size;

        m_vecAvailableIndices.resize( size );
        // fill vector with all possible slot numbers
        std::iota( m_vecAvailableIndices.rbegin(), m_vecAvailableIndices.rend(), 0 );

        m_arrEntityIDs = new entityid_t[ size ];
        std::memset( m_arrEntityIDs, ENTITY_ID_INVALID, size * sizeof( entityid_t ) );
    }

    CComponentStorageSegment_Base::~CComponentStorageSegment_Base()
    {
        delete[] m_arrEntityIDs;
    }

    segsize_t CComponentStorageSegment_Base::getSize() const
    {
        return m_size;
    }

    bool CComponentStorageSegment_Base::isEmpty() const
    {
        // if all indices are available, segment is empty
        return m_vecAvailableIndices.size() == m_size;
    }

    bool CComponentStorageSegment_Base::isFull() const
    {
        // if there are no available indices, segment is full
        return m_vecAvailableIndices.empty();
    }

    segsize_t CComponentStorageSegment_Base::getTakenSlotCount() const
    {
        return m_size - m_vecAvailableIndices.size();
    }

    bool CComponentStorageSegment_Base::hasSlottedComponent( entityid_t entityID ) const
    {
        if( entityID == ENTITY_ID_INVALID )
        {
            return false;
        }

        for (entityid_t i = 0; i < m_size; i++)
        {
            if( m_arrEntityIDs[i] == entityID )
            {
                return true;
            }   
        }

        return false;
    }

} // namespace chestnut::ecs::internal
