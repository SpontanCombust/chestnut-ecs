#ifndef __CHESTNUT_COMPONENT_H__
#define __CHESTNUT_COMPONENT_H__

#include "types.hpp"

#include <type_traits>

namespace chestnut
{
    const entityid ENTITY_ID_INVALID = 0;

    class CComponent
    {
    public:
        entityid owner;

        CComponent();
        virtual ~CComponent() = default;
    };


    #define CHESTNUT_STATIC_ASSERT_VALID_COMPONENT_CLASS(T) static_assert( ( std::is_base_of_v< CComponent, T > && std::is_default_constructible_v< T > ), "Component class " #T " must inherit from CComponent and be default constructible" );


} // namespace chestnut

#endif // __CHESTNUT_COMPONENT_H__