#include "constants.hpp"

namespace chestnut::ecs
{       
    template<typename C>
    CComponentHandle<C>::CComponentHandle() noexcept
    {
        this->owner = ENTITY_ID_INVALID;
        this->m_componentStorage = nullptr;
    }

    template<typename C>
    CComponentHandle<C>::CComponentHandle( entityid_t owner, internal::CComponentStorage *storage) noexcept
    {
        this->owner = owner;
        this->m_componentStorage = storage;
    }

    template<typename C>
    CComponentHandle<C>& CComponentHandle<C>::operator=(const C& val) 
    {
        m_componentStorage->at<C>(this->owner) = val;
        return *this;
    }

    template<typename C>
    inline C& CComponentHandle<C>::get() 
    {
        if( !m_componentStorage )
        {
            throw std::runtime_error("Invalid component handle");
        }

        return m_componentStorage->at<C>(this->owner);
    }

    template<typename C>
    inline const C& CComponentHandle<C>::get() const
    {
        if( !m_componentStorage )
        {
            throw std::runtime_error("Invalid component handle");
        }

        return m_componentStorage->at<C>(this->owner);
    }

    template<typename C>
    inline C& CComponentHandle<C>::operator*() 
    {
        return this->get();
    }

    template<typename C>
    inline const C& CComponentHandle<C>::operator*() const
    {
        return this->get();
    }

    template<typename C>
    inline C* CComponentHandle<C>::operator->() 
    {
        return &this->get();
    }

    template<typename C>
    inline const C* CComponentHandle<C>::operator->() const
    {
        return &this->get();
    }

    template<typename C>
    inline CComponentHandle<C>::operator bool() const noexcept
    {
        return m_componentStorage != nullptr;
    }

} // namespace chestnut::ecs
