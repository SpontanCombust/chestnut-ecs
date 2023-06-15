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
        entityid_t m_highestId;


    public:
        CComponentStorage();
        ~CComponentStorage();

        //TODO2.0 use optional/result instead of exceptions
        template<typename T>
        T& at(entityid_t id);

        template<typename T>
        const T& at(entityid_t id) const;

        
        template<typename T>
        bool empty() const noexcept;

        template<typename T>
        entitysize_t size() const noexcept;

        template<typename T>
        bool contains(entityid_t id) const noexcept;


        template<typename T>
        void clear() noexcept;

        template<typename T>
        void insert(entityid_t id, T&& arg) noexcept;

        template<typename T>
        void insert(entityid_t id) noexcept;

        template<typename T>
        void erase(entityid_t id) noexcept;


        void eraseAll(entityid_t id) noexcept;

        CEntitySignature signature(entityid_t id) const noexcept;

    private:
        template<typename T>
        CSparseSet<T>& getSparseSet() const noexcept;
    };

} // namespace chestnut::ecs::internal


#include "component_storage.inl"