#include "constants.hpp"

#include "chestnut/ecs/constants.hpp"

#include <cstring> // memset
#include <numeric> // iota

namespace chestnut::ecs::internal
{    
    inline CComponentStorageSegment_Base::CComponentStorageSegment_Base( segsize_t size ) noexcept
    {
        m_size = size;

        m_vecAvailableIndices.resize( size );
        // fill vector with all possible slot numbers
        std::iota( m_vecAvailableIndices.rbegin(), m_vecAvailableIndices.rend(), 0 );

        m_arrEntityIDs = new entityid_t[ size ];
        std::memset( m_arrEntityIDs, ENTITY_ID_INVALID, size * sizeof( entityid_t ) );
    }

    inline CComponentStorageSegment_Base::~CComponentStorageSegment_Base()
    {
        delete[] m_arrEntityIDs;
    }

    inline segsize_t CComponentStorageSegment_Base::getSize() const noexcept
    {
        return m_size;
    }

    inline bool CComponentStorageSegment_Base::isEmpty() const noexcept
    {
        // if all indices are available, segment is empty
        return m_vecAvailableIndices.size() == m_size;
    }

    inline bool CComponentStorageSegment_Base::isFull() const noexcept
    {
        // if there are no available indices, segment is full
        return m_vecAvailableIndices.empty();
    }

    inline segsize_t CComponentStorageSegment_Base::getTakenSlotCount() const noexcept
    {
        return m_size - m_vecAvailableIndices.size();
    }

    inline bool CComponentStorageSegment_Base::hasSlottedComponent( entityid_t entityID ) const noexcept
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






    template<class C>
    CComponentStorageSegment<C>::CComponentStorageSegment( segsize_t size ) noexcept
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
    SComponentWrapper<C>* CComponentStorageSegment<C>::tryTakeUpSlot( entityid_t entityID ) noexcept
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
    SComponentWrapper<C>* CComponentStorageSegment<C>::tryTakeUpSlot( entityid_t entityID, const SComponentWrapper<C>* copySrc ) noexcept
    {
        if( entityID == ENTITY_ID_INVALID || !copySrc )
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
    SComponentWrapper<C>* CComponentStorageSegment<C>::getSlottedComponent( entityid_t entityID ) const noexcept
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
    void CComponentStorageSegment<C>::freeSlot( entityid_t entityID ) noexcept
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
    void CComponentStorageSegment<C>::clearSlots() noexcept
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
