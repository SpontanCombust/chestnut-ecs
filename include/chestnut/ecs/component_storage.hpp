#pragma once

#include "sparse_set.hpp"
#include "types.hpp"
#include "entity_signature.hpp"

#include <tl/expected.hpp>
#include <tl/optional.hpp>

#include <memory>
#include <string>
#include <typeindex>
#include <unordered_map>

namespace chestnut::ecs::internal
{
    class CComponentStorage
    {
    private:
        mutable std::unordered_map<std::type_index, std::unique_ptr<CSparseSetBase>> m_mapTypeToSparseSet;


    public:
        CComponentStorage();
        ~CComponentStorage();


        template<typename T>
        tl::expected<T*, std::string> at(entityslot_t slot);

        template<typename T>
        tl::expected<const T*, std::string> at(entityslot_t slot) const;

        
        template<typename T>
        bool empty() const noexcept;

        template<typename T>
        size_t size() const noexcept;

        size_t maxSlot() const noexcept;

        template<typename T>
        bool contains(entityslot_t slot) const noexcept;


        template<typename T>
        void clear() noexcept;

        template<typename T>
        void insert(entityslot_t slot, T&& arg) noexcept;

        template<typename T>
        void insert(entityslot_t slot) noexcept;

        template<typename T>
        void erase(entityslot_t slot) noexcept;


        void eraseAll(entityslot_t slot) noexcept;

        CEntitySignature signature(entityslot_t slot) const noexcept;

    private:
        template<typename T>
        CSparseSet<T>& getSparseSet() const noexcept;
    };

} // namespace chestnut::ecs::internal


#include "component_storage.inl"