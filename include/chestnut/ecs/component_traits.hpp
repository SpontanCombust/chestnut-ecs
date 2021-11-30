/**
 * @file component_traits.hpp
 * @author Cedro Przmysław
 * @brief Header file for the draft of component traits type
 * @version 1.0
 * @date 2021-11-30
 * 
 * @copyright Copyright (c) 2021
 * 
 */


#ifndef __CHESTNUT_ECS_COMPONENT_TRAITS_H__
#define __CHESTNUT_ECS_COMPONENT_TRAITS_H__

#include "types.hpp"
#include "macros.hpp"

namespace chestnut::ecs
{
    /**
     * @brief Template class describing traits of component type
     * 
     * @tparam C type of component
     */
    template< class C >
    struct ComponentTraits
    {
        /**
         * @brief Size of a single segment in storage
         * 
         * @see CComponentStorage
         */
        static const segsize_t storageSegmentSize     = CHESTNUT_DEFAULT_STORAGE_SEGMENT_SIZE;
        /**
         * @brief Initial size of component storage 
         * 
         * @see CComponentStorage
         */
        static const segsize_t storageInitCapacity    = CHESTNUT_DEFAULT_STORAGE_SEGMENT_SIZE;
    };

} // namespace chestnut::ecs

#endif // __CHESTNUT_ECS_COMPONENT_TRAITS_H__