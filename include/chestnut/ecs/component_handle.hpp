/**
 * @file component_handle.hpp
 * @author Przemysław Cedro (SpontanCombust)
 * @brief Header file for the component handle template class
 * @version 1.0
 * @date 2021-11-30
 * 
 * @copyright Copyright (c) 2021
 * 
 */


#pragma once

#include "types.hpp"
#include "component_storage.hpp"


namespace chestnut::ecs
{
    /**
     * @brief Type providing safe access to component returned from entity world, also stores owner entity ID
     * 
     * @tparam C component type
     */
    template< typename C >
    class CComponentHandle
    {
        friend class CEntityWorld;
        
    private:
        /**
         * @brief Pointer to the storage
         */
        internal::CComponentStorage *m_componentStorage;

    public:
        /**
         * @brief Owner entity of the component
         */
        entityid_t owner; 


    public:
        /**
         * @brief Constructor; initializes owner to ENTITY_ID_INVALID
         * 
         */
        CComponentHandle() noexcept;

        /**
         * @brief Inner value assignment operator
         * 
         * @param val component value
         * @return reference to this
         * 
         * @throws if handle or component is invalid
         */
        CComponentHandle& operator=(const C& val);

        /**
         * @brief Returns a reference to the held component
         * 
         * @return component reference
         * 
         * @throws if handle or component is invalid
         */

        C& get();
        /**
         * @brief Returns a const reference to the held component
         * 
         * @return const component reference
         * 
         * @throws if handle or component is invalid
         */
        const C& get() const;

        /**
         * @brief Overloaded dereference operator
         * 
         * @return component reference
         * 
         * @throws if handle or component is invalid
         */
        C& operator*();

        /**
         * @brief Overloaded dereference operator
         * 
         * @return const component reference
         * 
         * @throws if handle or component is invalid
         */
        const C& operator*() const;

        /**
         * @brief Overloaded pointer-to-member operator
         * 
         * @return component pointer
         * 
         * @throws if handle or component is invalid
         */
        C* operator->();

        /**
         * @brief Overloaded pointer-to-member operator
         * 
         * @return const component pointer
         * 
         * @throws if handle or component is invalid
         */
        const C* operator->() const;

        /**
         * @brief Overloaded implicit bool cast operator, allows to check if referenced component is valid
         * 
         * @return true if component is valid
         * @return false if component is not valid
         */
        operator bool() const noexcept;

    private:
        CComponentHandle( entityid_t owner, internal::CComponentStorage *storage) noexcept;
    };

} // namespace chestnut::ecs


#include "component_handle.inl"
