/**
 * @file constants.hpp
 * @author Przemysław Cedro (SpontanCombust)
 * @brief Header file for constants defined in the library
 * @version 1.0
 * @date 2021-11-30
 * 
 * @copyright Copyright (c) 2021
 * 
 */


#pragma once

#include "types.hpp"

#include <limits>

namespace chestnut::ecs
{
    /**
     * @brief Constant reserved for invalid entity ID 
     */
    const entityid_t ENTITY_ID_INVALID = std::numeric_limits<entityid_t>::max();

    /**
     * @brief Constant for the minimal value entity ID can take
     */
    const entityid_t ENTITY_ID_MINIMAL = 0;
    
} // namespace chestnut::ecs
