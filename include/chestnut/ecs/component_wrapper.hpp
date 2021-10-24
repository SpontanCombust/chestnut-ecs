#ifndef __CHESTNUT_ECS_COMPONENT_WRAPPER_H__
#define __CHESTNUT_ECS_COMPONENT_WRAPPER_H__

namespace chestnut::ecs::internal
{
    // These types are here to avoid the need to make user component types be derived from any base type 

    struct IComponentWrapper
    {
        virtual ~IComponentWrapper() = default;
    };

    template< typename C >
    struct SComponentWrapper : IComponentWrapper
    {
        C data;
    };

} // namespace chestnut::ecs::internal

#endif // __CHESTNUT_ECS_COMPONENT_WRAPPER_H__