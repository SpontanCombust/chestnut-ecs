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
    const entityslot_t ENTITY_SLOT_INVALID = std::numeric_limits<entityslot_t>::max();

    /**
     * @brief Constant for the minimal value entity ID can take
     */
    const entityslot_t ENTITY_SLOT_MINIMAL = 0;
    
} // namespace chestnut::ecs
