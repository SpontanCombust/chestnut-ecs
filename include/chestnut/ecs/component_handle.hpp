#ifndef __CHESTNUT_ECS_COMPONENT_HANDLE_H__
#define __CHESTNUT_ECS_COMPONENT_HANDLE_H__

#include "types.hpp"
#include "component_wrapper.hpp"

namespace chestnut::ecs
{
    /**
     * @brief Type providing safe access to component returned from entity world, also stores owner entity ID
     * 
     * @tparam C 
     */
    template< typename C >
    class CComponentHandle
    {
        friend class CEntityWorld;
        
    private:
        internal::SComponentWrapper<C> *m_componentWrapper;

    public:
        entityid_t owner; // entity to which the component belongs 


    public:
        CComponentHandle();

        C& get();
        const C& get() const;

        C* operator->();
        const C* operator->() const;

        operator bool() const;

    private:
        CComponentHandle( entityid_t owner, internal::SComponentWrapper<C> *componentWrapper );
    };

} // namespace chestnut::ecs


#include "component_handle.tpp"


#endif // __CHESTNUT_ECS_COMPONENT_HANDLE_H__