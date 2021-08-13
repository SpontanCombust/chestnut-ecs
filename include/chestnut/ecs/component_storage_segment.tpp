#include <cassert>
#include <exception>
#include <numeric>

namespace chestnut::internal
{    
    template<class C>
    CComponentStorageSegment<C>::CComponentStorageSegment( segsize size ) 
    : CComponentStorageSegment_Base( size )
    {
        m_arrComponentSlots = new C[ size ];
    }

    template<class C>
    CComponentStorageSegment<C>::~CComponentStorageSegment() 
    {
        delete[] m_arrComponentSlots;
    }

    template<class C>
    C* CComponentStorageSegment<C>::tryTakeUpSlot( entityid entityID ) 
    {
        assert( entityID != ENTITY_ID_INVALID );
        
        if( hasSlottedComponent( entityID ) )
        {
            return getSlottedComponent( entityID );
        }
        else if( isFull() )
        {
            return nullptr;
        }

        size_t slot = m_vecAvailableIndices.back();
        m_vecAvailableIndices.pop_back();

        m_arrComponentSlots[ slot ].owner = entityID;

        m_mapEntityIDToIndex[ entityID ] = slot;

        return &m_arrComponentSlots[ slot ];
    }

    template<class C>
    C* CComponentStorageSegment<C>::tryTakeUpSlot( entityid entityID, const C& copySrc ) 
    {
        assert( entityID != ENTITY_ID_INVALID );
        
        if( hasSlottedComponent( entityID ) )
        {
            return getSlottedComponent( entityID );
        }
        else if( isFull() )
        {
            return nullptr;
        }

        size_t slot = m_vecAvailableIndices.back();
        m_vecAvailableIndices.pop_back();

        m_arrComponentSlots[ slot ] = copySrc;
        m_arrComponentSlots[ slot ].owner = entityID;

        m_mapEntityIDToIndex[ entityID ] = slot;

        return &m_arrComponentSlots[ slot ];
    }

    template<class C>
    C* CComponentStorageSegment<C>::getSlottedComponent( entityid entityID ) const
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
    void CComponentStorageSegment<C>::freeSlot( entityid entityID ) 
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
