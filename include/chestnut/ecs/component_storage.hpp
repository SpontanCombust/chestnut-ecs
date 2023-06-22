#pragma once

#include "sparse_set.hpp"
#include "types.hpp"
#include "entity_signature.hpp"

#include <memory>
#include <typeindex>
#include <unordered_map>

namespace chestnut::ecs::internal
{
    class CComponentStorage
    {
    private:
        mutable std::unordered_map<std::type_index, std::unique_ptr<CSparseSetBase>> m_mapTypeToSparseSet;
        entityslot_t m_lastSlot;


    public:
        CComponentStorage();
        ~CComponentStorage();

        //TODO2.0 use optional/result instead of exceptions
        template<typename T>
        T& at(entityslot_t id);

        template<typename T>
        const T& at(entityslot_t id) const;

        
        template<typename T>
        bool empty() const noexcept;

        template<typename T>
        size_t size() const noexcept;

        template<typename T>
        bool contains(entityslot_t id) const noexcept;


        template<typename T>
        void clear() noexcept;

        template<typename T>
        void insert(entityslot_t id, T&& arg) noexcept;

        template<typename T>
        void insert(entityslot_t id) noexcept;

        template<typename T>
        void erase(entityslot_t id) noexcept;


        void eraseAll(entityslot_t id) noexcept;

        CEntitySignature signature(entityslot_t id) const noexcept;

    private:
        template<typename T>
        CSparseSet<T>& getSparseSet() const noexcept;
    };

} // namespace chestnut::ecs::internal


#include "component_storage.inl"