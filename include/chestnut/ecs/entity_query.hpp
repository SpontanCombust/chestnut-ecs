#pragma once

#include "types.hpp"
#include "component_storage.hpp"
#include "entity_query_supplier.hpp"

#include <functional>
#include <vector>

namespace chestnut::ecs
{
    class CEntityQuery
    {
        friend class internal::CEntityQuerySupplier;

    public:
        class IteratorBase;

        template<typename ...Types>
        class CurrentEntitiesIterator;

        template<typename ...Types>
        class IncomingEntitiesIterator;

        class OutgoingEntitiesIterator;


        template<typename ...Types>
        using Iterator = CurrentEntitiesIterator<Types...>;


    private:
        internal::CComponentStorage *m_storagePtr;
        internal::CEntityQuerySupplier *m_supplier;

        // New entity slots that arrived since last update()
        std::vector<entityslot_t> m_vecIncomingEntitySlots;
        // Entity slots that were removed since last update()
        std::vector<entityslot_t> m_vecOutgoingEntitySlots;
        // Current valid entity slots
        std::vector<entityslot_t> m_vecEntitySlots;


    public:
        CEntityQuery(internal::CComponentStorage *storagePtr, internal::CEntityQuerySupplier *supplier) noexcept;
        CEntityQuery(const CEntityQuery& other) noexcept;


        const CEntitySignature& getRequireSignature() const noexcept;
        const CEntitySignature& getRejectSignature() const noexcept;
        const std::vector<CEntity> getEntities() const;
        size_t getEntityCount() const noexcept;

        void update();



        struct Iterators {
            Iterators(CEntityQuery *parent) 
            : current(parent), incoming(parent), outgoing(parent) {}

            struct Current {
                CEntityQuery *m_parent;
                Current(CEntityQuery *parent) : m_parent(parent) {}

                template<typename ...Types>
                CurrentEntitiesIterator<Types...> begin();
                template<typename ...Types>
                CurrentEntitiesIterator<Types...> end();
            } current;

            struct Incoming {
                CEntityQuery *m_parent;
                Incoming(CEntityQuery *parent) : m_parent(parent) {}

                template<typename ...Types>
                IncomingEntitiesIterator<Types...> begin();
                template<typename ...Types>
                IncomingEntitiesIterator<Types...> end();
            } incoming;

            struct Outgoing {
                CEntityQuery *m_parent;
                Outgoing(CEntityQuery *parent) : m_parent(parent) {}

                OutgoingEntitiesIterator begin();
                OutgoingEntitiesIterator end();
            } outgoing;

        } iterators;

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
