#include <algorithm>
#include <exception>
#include <numeric>
#include <sstream>

namespace chestnut::ecs
{    
    template<class C>
    CComponentStorageSegment<C>::CComponentStorageSegment( segid id, segsize size ) 
    {
        m_id = id;

        m_size = size;

        m_arrComponentSlots = new C[ size ];

        m_vecAvailableIndices.resize( size );
        // fill vector with all possible slot numbers
        std::iota( m_vecAvailableIndices.rbegin(), m_vecAvailableIndices.rend(), 0 );
    }

    template<class C>
    CComponentStorageSegment<C>::~CComponentStorageSegment() 
    {
        delete[] m_arrComponentSlots;
    }

    template<class C>
    segid CComponentStorageSegment<C>::getID() const
    {
        return m_id;
    }

    template<class C>
    segsize CComponentStorageSegment<C>::getSize() const
    {
        return m_size;
    }

    template<class C>
    bool CComponentStorageSegment<C>::isEmpty() const
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

    template<class C>
    bool CComponentStorageSegment<C>::isFull() const
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

    template<class C>
    segsize CComponentStorageSegment<C>::getTakenSlotCount() const
    {
        return m_size - m_vecAvailableIndices.size();
    }

    template<class C>
    segsize CComponentStorageSegment<C>::tryTakeUpSlot( entityid entityID ) 
    {
        if( hasSlottedComponent( entityID ) )
        {
            return m_mapEntityIDToIndex[ entityID ];
        }
        else if( isFull() )
        {
            throw std::out_of_range( "Segment is full. Can't take up a slot!" );
        }

        size_t slot = m_vecAvailableIndices.back();
        m_vecAvailableIndices.pop_back();

        m_arrComponentSlots[ slot ].owner = entityID;

        m_mapEntityIDToIndex[ entityID ] = slot;

        return slot;
    }

    template<class C>
    bool CComponentStorageSegment<C>::hasSlottedComponent( entityid entityID ) const
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

    template<class C>
    C* CComponentStorageSegment<C>::getComponentByEntity( entityid entityID ) const
    {
        auto it = m_mapEntityIDToIndex.find( entityID );

        if( it != m_mapEntityIDToIndex.end() )
        {
            return &m_arrComponentSlots[ it->second ];
        }
        else
        {
            return nullptr;
        }
    }

    template<class C>
    C* CComponentStorageSegment<C>::getComponentByIndex( segsize index ) const
    {
        if( index >= m_size )
        {
            throw std::out_of_range( "Segment index out of size range!" );
        }

        return &m_arrComponentSlots[ index ];
    }

    template<class C>
    C* CComponentStorageSegment<C>::operator[]( segsize index ) const
    {
        return getComponentByIndex( index );
    }

    template<class C>
    segsize CComponentStorageSegment<C>::getIndexByEntity( entityid entityID ) const
    {
        auto it = m_mapEntityIDToIndex.find( entityID );

        if( it != m_mapEntityIDToIndex.end() )
        {
            return it->second;
        }
        else
        {
            throw std::out_of_range( "Entity not found. No index to return!" );
        }
    }

    template<class C>
    void CComponentStorageSegment<C>::freeSlotByEntity( entityid entityID ) 
    {
        auto it = m_mapEntityIDToIndex.find( entityID );
        if( it != m_mapEntityIDToIndex.end() )
        {
            // reset the component
            m_arrComponentSlots[ it->second ] = C();
            m_mapEntityIDToIndex.erase( it );
            m_vecAvailableIndices.push_back( it->second );
        }
    }

    template<class C>
    void CComponentStorageSegment<C>::freeSlotByIndex( segsize index ) 
    {
        if( index >= m_size )
        {
            throw std::out_of_range( "Segment index out of size range!" );
        }

        entityid ent = m_arrComponentSlots[ index ].owner;

        // ENTITY_ID_INVALID means that component at that slot has default values and does not belong to any entity
        if( ent != ENTITY_ID_INVALID )
        {
            m_arrComponentSlots[ index ] = C();
            m_mapEntityIDToIndex.erase( ent );
            m_vecAvailableIndices.push_back( index );
        }
    }

    template<class C>
    void CComponentStorageSegment<C>::clearSlots() 
    {
        m_vecAvailableIndices.clear();
        m_vecAvailableIndices.resize( m_size );
        std::iota( m_vecAvailableIndices.rbegin(), m_vecAvailableIndices.rend(), 0 );

        m_mapEntityIDToIndex.clear();
    }

    template<class C>
    std::string CComponentStorageSegment<C>::toString() const
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

} // namespace chestnut::ecs
