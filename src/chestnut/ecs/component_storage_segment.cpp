#include "chestnut/ecs/component_storage_segment.hpp"

#include <exception>
#include <numeric>
#include <sstream>

namespace chestnut::internal
{    
    CComponentStorageSegment_Base::CComponentStorageSegment_Base( segsize size ) 
    {
        m_size = size;

        m_vecAvailableIndices.resize( size );
        // fill vector with all possible slot numbers
        std::iota( m_vecAvailableIndices.rbegin(), m_vecAvailableIndices.rend(), 0 );
    }

    segsize CComponentStorageSegment_Base::getSize() const
    {
        return m_size;
    }

    bool CComponentStorageSegment_Base::isEmpty() const
    {
        if( m_mapEntityIDToIndex.empty() )
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    bool CComponentStorageSegment_Base::isFull() const
    {
        if( m_vecAvailableIndices.empty() )
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    segsize CComponentStorageSegment_Base::getTakenSlotCount() const
    {
        return m_size - m_vecAvailableIndices.size();
    }

    bool CComponentStorageSegment_Base::hasSlottedComponent( entityid entityID ) const
    {
        if( m_mapEntityIDToIndex.find( entityID ) != m_mapEntityIDToIndex.end() )
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    std::string CComponentStorageSegment_Base::toString() const
    {
        std::stringstream ss;

        ss << "[";
        if( !m_mapEntityIDToIndex.empty() )
        {
            auto it = m_mapEntityIDToIndex.begin();
            ss << std::to_string( it->first );
            it++;

            for(; it != m_mapEntityIDToIndex.end(); it++ )
            {
                ss << ", " << std::to_string( it->first );
            }
        }
        ss << "]";

        return ss.str();
    }


} // namespace chestnut::internal
