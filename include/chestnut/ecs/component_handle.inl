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
        auto uuid = m_componentStorage->at<CIdentityComponent>(this->owner.slot)
                                       .map([](const auto& identComp) { return identComp->uuid; });

        if (!uuid.has_value() || uuid.value() != this->owner.uuid)
        {
            return false;
        }

        return m_componentStorage->contains<C>(this->owner.slot);
    }

    template<typename C>
    inline CComponentHandle<C>& CComponentHandle<C>::operator=(C&& val) 
    {
        m_componentStorage->insert<C>(this->owner.slot, std::forward<C>(val));
        return *this;
    }

    template<typename C>
    inline tl::expected<C*, std::string> CComponentHandle<C>::get() 
    {
        auto uuid = m_componentStorage->at<CIdentityComponent>(this->owner.slot)
                                       .map([](const auto& identComp) { return identComp->uuid; });
        
        if (!uuid.has_value() || uuid.value() != this->owner.uuid)
        {
            return tl::make_unexpected("Entity " + std::to_string(this->owner.uuid) + " is not alive");
        }

        auto component = m_componentStorage->at<C>(this->owner.slot);
        if (!component.has_value())
        {
            return tl::make_unexpected("Entity does not have this component");
        }

        return component.value();
    }

    template<typename C>
    inline tl::expected<const C*, std::string> CComponentHandle<C>::get() const
    {
        auto uuid = m_componentStorage->at<CIdentityComponent>(this->owner.slot)
                                       .map([](const auto& identComp) { return identComp->uuid; });
        
        if (!uuid.has_value() || uuid.value() != this->owner.uuid)
        {
            return tl::make_unexpected("Entity " + std::to_string(this->owner.uuid) + " is not alive");
        }

        auto component = m_componentStorage->at<C>(this->owner.slot);
        if (!component.has_value())
        {
            return tl::make_unexpected("Entity does not have this component");
        }

        return component.value();
    }

    template<typename C>
    inline C& CComponentHandle<C>::operator*() 
    {
        return *this->get().value();
    }

    template<typename C>
    inline const C& CComponentHandle<C>::operator*() const
    {
        return *this->get().value();
    }

    template<typename C>
    inline C* CComponentHandle<C>::operator->() 
    {
        return this->get().value();
    }

    template<typename C>
    inline const C* CComponentHandle<C>::operator->() const
    {
        return this->get().value();
    }

    template<typename C>
    inline CComponentHandle<C>::operator bool() const noexcept
    {
        return this->valid();
    }

} // namespace chestnut::ecs
