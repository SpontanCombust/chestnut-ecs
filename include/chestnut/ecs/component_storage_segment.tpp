#include "constants.hpp"

#include <cstring> // memset
#include <numeric> // iota

namespace chestnut::ecs::internal
{    
    template<class C>
    CComponentStorageSegment<C>::CComponentStorageSegment( segsize_t size ) 
    : CComponentStorageSegment_Base( size )
    {
        m_arrComponentSlots = new SComponentWrapper<C>[ size ];
    }

    template<class C>
    CComponentStorageSegment<C>::~CComponentStorageSegment() 
    {
        delete[] m_arrComponentSlots;
    }

    template<class C>
    SComponentWrapper<C>* CComponentStorageSegment<C>::tryTakeUpSlot( entityid_t entityID ) 
    {
        if( entityID == ENTITY_ID_INVALID )
        {
            return nullptr;
        }
        else if( SComponentWrapper<C> *comp = getSlottedComponent( entityID ) )
        {
            return comp;
        }
        else if( isFull() )
        {
            return nullptr;
        }

        size_t slot = m_vecAvailableIndices.back();
        m_vecAvailableIndices.pop_back();

        m_arrEntityIDs[ slot ] = entityID;

        return &m_arrComponentSlots[ slot ];
    }

    template<class C>
    SComponentWrapper<C>* CComponentStorageSegment<C>::tryTakeUpSlot( entityid_t entityID, const SComponentWrapper<C>* copySrc ) 
    {
        if( entityID == ENTITY_ID_INVALID )
        {
            return nullptr;
        }
        if( SComponentWrapper<C> *comp = getSlottedComponent( entityID ) )
        {
            return comp;
        }
        else if( isFull() )
        {
            return nullptr;
        }

        size_t slot = m_vecAvailableIndices.back();
        m_vecAvailableIndices.pop_back();

        m_arrEntityIDs[ slot ] = entityID;
        m_arrComponentSlots[ slot ] = *copySrc;

        return &m_arrComponentSlots[ slot ];
    }

    template<class C>
    SComponentWrapper<C>* CComponentStorageSegment<C>::getSlottedComponent( entityid_t entityID ) const
    {
        if( entityID == ENTITY_ID_INVALID )
        {
            return nullptr;
        }

        for (entityid_t i = 0; i < m_size; i++)
        {
            if( m_arrEntityIDs[i] == entityID )
            {
                return &m_arrComponentSlots[i];
            }   
        }

        return nullptr;
    }
    
    template<class C>
    void CComponentStorageSegment<C>::freeSlot( entityid_t entityID ) 
    {
        for (entityid_t i = 0; i < m_size; i++)
        {
            if( m_arrEntityIDs[i] == entityID )
            {
                // reset the component to default state
                m_arrComponentSlots[i].data = C();
                m_arrEntityIDs[i] = ENTITY_ID_INVALID;
                m_vecAvailableIndices.push_back(i);
                break;
            }   
        }
    }

    template<class C>
    void CComponentStorageSegment<C>::clearSlots() 
    {
        m_vecAvailableIndices.clear();
        m_vecAvailableIndices.resize( m_size );
        std::iota( m_vecAvailableIndices.rbegin(), m_vecAvailableIndices.rend(), 0 );

        // reset all components to default state
        for (entityid_t i = 0; i < m_size; i++)
        {
            m_arrComponentSlots[i].data = C();
        }

        std::memset( m_arrEntityIDs, ENTITY_ID_INVALID, m_size * sizeof( entityid_t ) );
    }

} // namespace chestnut::ecs::internal
