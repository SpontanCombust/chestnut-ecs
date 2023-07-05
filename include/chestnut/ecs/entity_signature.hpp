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


#pragma once

#include <string>
#include <typeindex>
#include <type_traits> // enable_if
#include <vector>

namespace chestnut::ecs
{
    /**
     * @brief Class used to record component types that entity is comprised of
     */
    class CEntitySignature
    {
    public:
        using Iterator = std::vector<std::type_index>::const_iterator;

        CEntitySignature();

        CEntitySignature& add(std::type_index type);
        CEntitySignature& remove(std::type_index type);
        bool has(std::type_index type) const;

        CEntitySignature& add(const CEntitySignature& other);
        CEntitySignature& remove(const CEntitySignature& other);
        bool has(const CEntitySignature& other) const;

        CEntitySignature& clear();
        bool empty() const;
        size_t size() const;



        template<typename Type, typename ...Rest>
        static CEntitySignature from();

        
        template<typename Type, typename ...Rest>
        CEntitySignature& add();

        template<typename ...Rest>
        std::enable_if_t<sizeof...(Rest) == 0, CEntitySignature&> add();


        template<typename Type, typename ...Rest>
        CEntitySignature& remove();

        template<typename ...Rest>
        std::enable_if_t<sizeof...(Rest) == 0, CEntitySignature&> remove();


        template<typename Type, typename ...Rest>
        bool has() const;

        template<typename ...Types>
        std::enable_if_t<sizeof...(Types) == 0, bool> has() const;


        bool operator==(const CEntitySignature& other) const;
        bool operator!=(const CEntitySignature& other) const;

        CEntitySignature& operator+=(const CEntitySignature& other); // equivalent to add(CEntitySignature)
        CEntitySignature& operator-=(const CEntitySignature& other); // equivalent to remove(CEntitySignature)


        CEntitySignature operator+(const CEntitySignature& other) const; // union
        CEntitySignature operator-(const CEntitySignature& other) const; // difference
        CEntitySignature operator&(const CEntitySignature& other) const; // intersection


        Iterator begin() const;
        Iterator end() const;

    private:
        std::vector<std::type_index> m_vecTypes;

        void addImpl(std::type_index type);
        void removeImpl(std::type_index type);
        void sort();
    };

} // namespace chestnut::ecs


#include "entity_signature.inl"
