#ifndef __CHESTNUT_COMPONENT_H__
#define __CHESTNUT_COMPONENT_H__

#include "types.hpp"

#include <type_traits>

namespace chestnut::ecs
{
    const entityid ENTITY_ID_INVALID = 0;

    class CComponent
    {
    public:
        entityid owner;

        CComponent();
    };


    #define ASSERT_DERIVED_FROM_COMPONENT(T) static_assert( std::is_base_of< CComponent, T >::value, "Component class " #T " must inherit from CComponent" );


} // namespace chestnut::ecs

#endif // __CHESTNUT_COMPONENT_H__