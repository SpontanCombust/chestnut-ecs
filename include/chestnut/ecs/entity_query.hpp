#pragma once

#include "types.hpp"
#include "component_storage.hpp"
#include "entity_query_guard.hpp"

#include <functional>
#include <vector>

namespace chestnut::ecs
{
    class CEntityQuery
    {
        friend class internal::CEntityQueryGuard;

    public:
        template<typename ...Types>
        class Iterator;
        template<typename ...Types>
        friend class Iterator;

    private:
        internal::CComponentStorage *m_storagePtr;
        internal::CEntityQueryGuard *m_supplier;

        std::vector< entityslot_t > m_vecEntitySlots;


    public:
        CEntityQuery(internal::CComponentStorage *storagePtr, internal::CEntityQueryGuard *supplier) noexcept;


        const CEntitySignature& getRequireSignature() const noexcept;
        const CEntitySignature& getRejectSignature() const noexcept;
        const std::vector<CEntity> getEntities() const;
        size_t getEntityCount() const noexcept;


        SEntityQueryUpdateInfo update();
                

        template<typename ...Types>
        Iterator<Types...> begin();

        template<typename ...Types>
        Iterator<Types...> end();


        template<typename ...Types>
        void forEach(const std::function<void(Types&...)>& handler);


        template<typename ...Types>
        void sort(std::function<bool(Iterator<Types...>, Iterator<Types...>)> comparator);
    };

} // namespace chestnut::ecs


#include "entity_query_iterator.inl"
#include "entity_query.inl"
