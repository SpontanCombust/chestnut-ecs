#include "constants.hpp"

#include "native_components.hpp"

namespace chestnut::ecs
{       
    template<typename C>
    CComponentHandle<C>::CComponentHandle(CEntity owner, internal::CComponentStorage *storage) noexcept
    : m_componentStorage(storage), owner(owner)
    {

    }

    template<typename C>
    inline bool CComponentHandle<C>::valid() const
    {
        if (m_componentStorage->contains<CIdentityComponent>(this->owner.slot))
        {
            auto uuid = m_componentStorage->at<CIdentityComponent>(this->owner.slot).uuid;
            if (uuid != this->owner.uuid)
            {
                return false;
            }

            return m_componentStorage->contains<C>(this->owner.slot);
        }

        return false;
    }

    template<typename C>
    inline CComponentHandle<C>& CComponentHandle<C>::operator=(C&& val) 
    {
        m_componentStorage->insert<C>(this->owner.slot, std::forward<C>(val));
        return *this;
    }

    template<typename C>
    inline C& CComponentHandle<C>::get() 
    {
        return m_componentStorage->at<C>(this->owner.slot);
    }

    template<typename C>
    inline const C& CComponentHandle<C>::get() const
    {
        return m_componentStorage->at<C>(this->owner.slot);
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
        return this->valid();
    }

} // namespace chestnut::ecs
