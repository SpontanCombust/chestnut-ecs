#include "native_components.hpp"

#include <algorithm> // stable_sort
#include <exception>
#include <numeric> // iota
#include "entity_query.hpp"

namespace chestnut::ecs
{

inline CEntityQuery::CEntityQuery(internal::CComponentStorage *storagePtr, internal::CEntityQuerySupplier *supplier) noexcept
: m_storagePtr(storagePtr),
  m_supplier(supplier),
  iterators(this)
{

}

inline CEntityQuery::CEntityQuery(const CEntityQuery &other) noexcept
: m_storagePtr(other.m_storagePtr),
  m_supplier(other.m_supplier),
  iterators(this)
{

}

inline const CEntitySignature& CEntityQuery::getRejectSignature() const noexcept
{
    return m_supplier->requireSignature();
}

inline const CEntitySignature& CEntityQuery::getRequireSignature() const noexcept
{
    return m_supplier->rejectSignature();
}

inline const std::vector<CEntity> CEntityQuery::getEntities() const
{
    std::vector<CEntity> v;

    for(unsigned int slot : m_vecCurrentEntitySlots)
    {
        CUniqueIdentifier uuid = m_storagePtr->at<CIdentityComponent>(slot).value()->uuid;
        v.push_back(CEntity(uuid, slot));
    }    

    return v;
}

inline size_t CEntityQuery::getEntityCount() const noexcept
{
    return m_vecCurrentEntitySlots.size();
}

inline void CEntityQuery::update()
{
    m_supplier->processPendingEntities(
        this->m_vecCurrentEntitySlots,
        this->m_vecAcquiredEntitySlots,
        this->m_vecDiscardedEntitySlots
    );
}




template<typename ...Types>
inline CEntityQuery::CurrentEntitiesIterator<Types...> CEntityQuery::Iterators::Current::begin()
{
    if(!m_parent->m_supplier->testSignature(CEntitySignature::from<Types...>()))
    {
        throw std::runtime_error("Iterator not compatible with the query");
    }

    return CurrentEntitiesIterator<Types...>(m_parent, 0);
}

template<typename ...Types>
inline CEntityQuery::CurrentEntitiesIterator<Types...> CEntityQuery::Iterators::Current::end()
{
    if(!m_parent->m_supplier->testSignature(CEntitySignature::from<Types...>()))
    {
        throw std::runtime_error("Iterator not compatible with the query");
    }

    return CurrentEntitiesIterator<Types...>(m_parent, m_parent->m_vecCurrentEntitySlots.size());
}


template<typename ...Types>
inline CEntityQuery::AcquiredEntitiesIterator<Types...> CEntityQuery::Iterators::Acquired::begin()
{
    if(!m_parent->m_supplier->testSignature(CEntitySignature::from<Types...>()))
    {
        throw std::runtime_error("Iterator not compatible with the query");
    }

    return AcquiredEntitiesIterator<Types...>(m_parent, 0);    
}

template<typename ...Types>
inline CEntityQuery::AcquiredEntitiesIterator<Types...> CEntityQuery::Iterators::Acquired::end()
{
    if(!m_parent->m_supplier->testSignature(CEntitySignature::from<Types...>()))
    {
        throw std::runtime_error("Iterator not compatible with the query");
    }

    return AcquiredEntitiesIterator<Types...>(m_parent, m_parent->m_vecAcquiredEntitySlots.size());    
}


inline CEntityQuery::DiscardedEntitiesIterator CEntityQuery::Iterators::Discarded::begin()
{
    return DiscardedEntitiesIterator(m_parent, 0);    
}

inline CEntityQuery::DiscardedEntitiesIterator CEntityQuery::Iterators::Discarded::end()
{
    return DiscardedEntitiesIterator(m_parent, m_parent->m_vecDiscardedEntitySlots.size());    
}

template<typename ...Types>
inline CEntityQuery::Iterator<Types...> CEntityQuery::begin()
{
    return this->iterators.current.begin<Types...>();
}

template<typename ...Types>
inline CEntityQuery::Iterator<Types...> CEntityQuery::end()
{
    return this->iterators.current.end<Types...>();
}




template<typename ...Types>
inline void CEntityQuery::forEach(const std::function<void(Types&...)>& handler )
{   
    for(auto it = this->begin<Types...>(); it != this->end<Types...>(); it++)
    {
        std::apply(handler, *it);
    }
}



template<typename ...Types>
inline void CEntityQuery::sort(std::function<bool(CEntityQuery::Iterator<Types...>, CEntityQuery::Iterator<Types...>)> comparator)
{
    if(!m_supplier->testSignature(CEntitySignature::from<Types...>()))
    {
        throw std::runtime_error("Iterator not compatible with the query");
    }

    std::vector<size_t> indices(m_vecCurrentEntitySlots.size());
    std::iota(indices.begin(), indices.end(), 0);

    std::stable_sort(indices.begin(), indices.end(),
        [this, &comparator](size_t idx1, size_t idx2) -> bool {
            Iterator<Types...> it1(this, idx1);
            Iterator<Types...> it2(this, idx2);

            return comparator(it1, it2);
        }
    );

    std::vector<entityslot_t> sortedEnts(m_vecCurrentEntitySlots.size());

    for (size_t i = 0; i < m_vecCurrentEntitySlots.size(); i++)
    {
        sortedEnts[i] = this->m_vecCurrentEntitySlots[indices[i]];
    }

    this->m_vecCurrentEntitySlots = std::move(sortedEnts);
}

} // namespace chestnut::ecs
