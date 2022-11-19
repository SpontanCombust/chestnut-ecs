/**
 * @file types.hpp
 * @author Przemysław Cedro (SpontanCombust)
 * @brief Header file with library typedefs
 * @version 1.0
 * @date 2021-11-30
 * 
 * @copyright Copyright (c) 2021
 * 
 */


#pragma once

#include <cstdint> // uint16_t, uint32_t

namespace chestnut::ecs
{
    /**
     * @brief Type for the ID of the entity
     * 
     * @details Entity ID is specified as unsigned 32-bit integer.
     */
    typedef uint32_t entityid_t;
    /**
     * @brief Type for quantity of entities (the type is the same as entityid_t)
     */
    typedef entityid_t entitysize_t;

} // namespace chestnut::ecs
