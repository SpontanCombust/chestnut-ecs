/**
 * @file macros.hpp
 * @author Przemysław Cedro (SpontanCombust)
 * @brief Header file with macros defined in the library
 * @version 1.0
 * @date 2021-11-30
 * 
 * @copyright Copyright (c) 2021
 * 
 */


#ifndef __CHESTNUT_ECS_MACROS_H__
#define __CHESTNUT_ECS_MACROS_H__

#ifndef CHESTNUT_DEFAULT_STORAGE_SEGMENT_SIZE
    /**
     * @brief Macro constant meaning the default size of component storage segment
     * 
     * @see CComponentStorageSegment, ComponentTraits
     */
    #define CHESTNUT_DEFAULT_STORAGE_SEGMENT_SIZE 64 //arbitrary number
#endif

#endif // __CHESTNUT_ECS_MACROS_H__