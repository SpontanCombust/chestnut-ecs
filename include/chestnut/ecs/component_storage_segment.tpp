#include <exception>
#include <numeric>

namespace chestnut::internal
{    
    template<class C>
    CComponentStorageSegment<C>::CComponentStorageSegment( segid id, segsize size ) 
    : CComponentStorageSegment_Base( id, size )
    {
        m_arrComponentSlots = new C[ size ];
    }

    template<class C>
    CComponentStorageSegment<C>::~CComponentStorageSegment() 
    {
        delete[] m_arrComponentSlots;
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
    void CComponentStorageSegment<C>::freeSlotByEntity( entityid entityID ) 
    {
        auto it = m_mapEntityIDToIndex.find( entityID );
        if( it != m_mapEntityIDToIndex.end() )
        {
            // reset the component to default state
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
            // reset the component to default state
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

        // reset all components to default state
        for (segsize i = 0; i < m_size; i++)
        {
            m_arrComponentSlots[i] = C();   
        }
    }

} // namespace chestnut::internal
