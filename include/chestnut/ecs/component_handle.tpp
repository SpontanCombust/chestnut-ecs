#include "constants.hpp"

namespace chestnut::ecs
{       
    template<typename C>
    CComponentHandle<C>::CComponentHandle() 
    {
        owner = ENTITY_ID_INVALID;
        m_componentWrapper = nullptr;
    }

    template<typename C>
    CComponentHandle<C>::CComponentHandle( entityid owner, internal::SComponentWrapper<C> *componentWrapper ) 
    {
        this->owner = owner;
        this->m_componentWrapper = componentWrapper;
    }

    template<typename C>
    inline C& CComponentHandle<C>::get() 
    {
        return m_componentWrapper->data;
    }

    template<typename C>
    inline const C& CComponentHandle<C>::get() const
    {
        return m_componentWrapper->data;
    }

    template<typename C>
    inline C* CComponentHandle<C>::operator->() 
    {
        return &m_componentWrapper->data;
    }

    template<typename C>
    inline const C* CComponentHandle<C>::operator->() const
    {
        return &m_componentWrapper->data;
    }

    template<typename C>
    inline CComponentHandle<C>::operator bool() const
    {
        return m_componentWrapper != nullptr;
    }

} // namespace chestnut::ecs
