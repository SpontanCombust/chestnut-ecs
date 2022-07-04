/**
 * @file component_wrapper.hpp
 * @author Przemysław Cedro (SpontanCombust)
 * @brief Header file for a wrapper type of component data used internally by the library
 * @version 1.0
 * @date 2021-11-30
 * 
 * @copyright Copyright (c) 2021
 * 
 */


#pragma once

namespace chestnut::ecs::internal
{
    // These types are here to avoid the need to make user component types be derived from any base type 

    /**
     * @brief Empty interface class for wrapper
     */
    struct IComponentWrapper
    {
        virtual ~IComponentWrapper() = default;
    };

    /**
     * @brief Template class for component wrapper, storing only plain data of the component
     * 
     * @tparam C component type
     */
    template< typename C >
    struct SComponentWrapper : IComponentWrapper
    {
        C data;
    };

} // namespace chestnut::ecs::internal
