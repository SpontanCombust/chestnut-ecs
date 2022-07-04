#include "constants.hpp"

namespace chestnut::ecs
{       
    template<typename C>
    CComponentHandle<C>::CComponentHandle() noexcept
    {
        owner = ENTITY_ID_INVALID;
        m_componentWrapper = nullptr;
    }

    template<typename C>
    CComponentHandle<C>::CComponentHandle( entityid_t owner, internal::SComponentWrapper<C> *componentWrapper ) noexcept
    {
        this->owner = owner;
        this->m_componentWrapper = componentWrapper;
    }

    template<typename C>
    inline C& CComponentHandle<C>::get() 
    {
        if( !m_componentWrapper )
        {
            throw BadComponentAccessException();
        }

        return m_componentWrapper->data;
    }

    template<typename C>
    inline const C& CComponentHandle<C>::get() const
    {
        if( !m_componentWrapper )
        {
            throw BadComponentAccessException();
        }

        return m_componentWrapper->data;
    }

    template<typename C>
    inline C* CComponentHandle<C>::operator->() 
    {
        if( !m_componentWrapper )
        {
            throw BadComponentAccessException();
        }

        return &m_componentWrapper->data;
    }

    template<typename C>
    inline const C* CComponentHandle<C>::operator->() const
    {
        if( !m_componentWrapper )
        {
            throw BadComponentAccessException();
        }
        
        return &m_componentWrapper->data;
    }

    template<typename C>
    inline CComponentHandle<C>::operator bool() const noexcept
    {
        return m_componentWrapper != nullptr;
    }

} // namespace chestnut::ecs
