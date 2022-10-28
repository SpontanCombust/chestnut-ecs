#pragma once

#include "types.hpp"
#include "component_wrapper.hpp"
#include "component_storage.hpp"
#include "entity_signature.hpp"

#include <functional>
#include <vector>

namespace chestnut::ecs
{
    namespace internal
    {
        class CEntityQueryGuard; // forward declaration

    } // namespace internal
    
    
    class CEntityQuery
    {
        friend class internal::CEntityQueryGuard;

    public:
        template<typename ...Types>
        struct Iterator;
        template<typename ...Types>
        friend struct Iterator;

    private:
        internal::CComponentStorage *m_storagePtr;

        CEntitySignature m_requireSignature;
        CEntitySignature m_rejectSignature;

        std::vector< entityid_t > m_vecEntityIDs;


    public:
        CEntityQuery(internal::CComponentStorage *storagePtr, CEntitySignature requireSignature, CEntitySignature rejectSignature ) noexcept;


        const CEntitySignature& getRequireSignature() const noexcept;
        const CEntitySignature& getRejectSignature() const noexcept;
        entitysize_t getEntityCount() const noexcept;

        

        template<typename ...Types>
        Iterator<Types...> begin() noexcept;

        template<typename ...Types>
        Iterator<Types...> end() noexcept;


        template<typename ...Types>
        CEntityQuery getSortedBy(std::function<bool(Iterator<Types...>, Iterator<Types...>)> comparator) noexcept;
    };

} // namespace chestnut::ecs


#include "entity_query_iterator.inl"
#include "entity_query.inl"
