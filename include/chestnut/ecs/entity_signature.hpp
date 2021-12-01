/**
 * @file entity_signature.hpp
 * @author Przemysław Cedro (SpontanCombust)
 * @brief Header file for entity signature class
 * @version 1.0
 * @date 2021-11-30
 * 
 * @copyright Copyright (c) 2021
 * 
 */


#ifndef __CHESTNUT_ECS_ENTITY_SIGNATURE_H__
#define __CHESTNUT_ECS_ENTITY_SIGNATURE_H__

#include <string>
#include <typeindex>
#include <unordered_set>

namespace chestnut::ecs
{
    /**
     * @brief Class used to record component types that entity is comprised of
     */
    class CEntitySignature
    {
    public:
        /**
         * @brief Set of std::type_index component types
         */
        std::unordered_set< std::type_index > m_setComponentTypes;

    public:
        /**
         * @brief Adds types to signature
         * 
         * @details If type already is in signature, nothing changes. 
         * 
         * @tparam Types component types
         */
        template< typename ...Types >
        void add();

        /**
         * @brief Removes types from signature
         * 
         * @tparam Types component types
         */
        template< typename ...Types >
        void remove();

        /**
         * @brief Checks if all given types are in the signature
         * 
         * @tparam Types component types
         * @return true if all given types are present
         * @return false if at least one of given types is not present
         */
        template< typename ...Types >
        bool has() const;


        /**
         * @brief Adds type to signature, non-generic method variant
         * 
         * @details If type already is in signature, nothing changes. 
         * 
         * @param compType type of component
         */
        void add( std::type_index compType );

        /**
         * @brief Removes type form signature, non-generic method version
         * 
         * @param compType type of component
         */
        void remove( std::type_index compType );

        /**
         * @brief Checks if given component type is in signature
         * 
         * @param compType component type
         * @return true if type is in signature
         * @return false otherwise
         */
        bool has( std::type_index compType ) const;

        
        /**
         * @brief Adds types from other signature
         * 
         * @param otherSign other signature 
         */
        void addFrom( const CEntitySignature& otherSign );

        /**
         * @brief Removes types that are mutual to this signature and otherSign signature 
         * 
         * @param otherSign other signature
         */
        void removeFrom( const CEntitySignature& otherSign );
        
		/**
		 * @brief Checks if signature contains all types that are in otherSign
		 * 
		 * @param otherSign other signature
		 * @return true if all types from otherSign are present
		 * @return false if at least one type is not present
		 */
        bool hasAllFrom( const CEntitySignature& otherSign ) const;

        /**
         * @brief Checks if signature contains any of the types that are in otherSign
         * 
         * @param otherSign other signature
         * @return true if at least one type from otherSign is present
         * @return false if none of the types in otherSign are present
         */
        bool hasAnyFrom( const CEntitySignature& otherSign ) const;


		/**
		 * @brief Remove all types from signature
		 */
        void clear();


		/**
		 * @brief Check if signature consists of at least one type
		 * 
		 * @return true if signature is empty
		 * @return false otherwise
		 */
        bool isEmpty() const;

		/**
		 * @brief Get the number of types in the signature
		 * 
		 * @return number of types in signature 
		 */
        int getSize() const;


		/**
		 * @brief Overloaded addition assignment operator; calls addFrom
		 * 
		 * @param other other signature
		 * @return CEntitySignature& reference to this
		 */
        CEntitySignature& operator+=( const CEntitySignature& other );

		/**
		 * @brief Overloaded subtraction assignment operator; calls removeFrom
		 * 
		 * @param other other signature
		 * @return CEntitySignature& reference to this
		 */
        CEntitySignature& operator-=( const CEntitySignature& other );
    };

	/**
	 * @brief Returns the sum of two signatures
	 * 
	 * @param lhs left-hand-side signature
	 * @param rhs right-hand-side signature
	 * @return CEntitySignature signature sum 
	 */
    CEntitySignature operator+( const CEntitySignature& lhs, const CEntitySignature& rhs );

	/**
	 * @brief Returns the difference of two signatures
	 * 
	 * @param lhs left-hand-side signature
	 * @param rhs right-hand-side signature
	 * @return CEntitySignature signature difference
	 */
    CEntitySignature operator-( const CEntitySignature& lhs, const CEntitySignature& rhs );

	/**
	 * @brief Checks if two signatures are the same
	 * 
	 * @param lhs left-hand-side signature
	 * @param rhs right-hand-side signature
	 * @return true if both signatures are the same
	 * @return false otherwise
	 */
    bool operator==( const CEntitySignature& lhs, const CEntitySignature& rhs );

	/**
	 * @brief Checks if two signatures are different
	 * 
	 * @param lhs left-hand-side signature
	 * @param rhs right-hand-side signature
	 * @return true if both signatures are different
	 * @return false otherwise
	 */
	bool operator!=( const CEntitySignature& lhs, const CEntitySignature& rhs );


	/**
	 * @brief Creates signature in place using given types
	 * 
	 * @tparam Types types of components
	 * @return CEntitySignature created signature
	 */
    template< typename ...Types >
    CEntitySignature makeEntitySignature();

} // namespace chestnut::ecs


#include "entity_signature.tpp"


#endif // __CHESTNUT_ECS_ENTITY_SIGNATURE_H__