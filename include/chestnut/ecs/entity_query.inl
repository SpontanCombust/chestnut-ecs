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

    for(unsigned int slot : m_vecEntitySlots)
    {
        CUniqueIdentifier uuid = m_storagePtr->at<CIdentityComponent>(slot).value()->uuid;
        v.push_back(CEntity(uuid, slot));
    }    

    return v;
}

inline size_t CEntityQuery::getEntityCount() const noexcept
{
    return m_vecEntitySlots.size();
}

inline void CEntityQuery::update()
{
    m_supplier->processPendingEntities(
        this->m_vecEntitySlots,
        this->m_vecIncomingEntitySlots,
        this->m_vecOutgoingEntitySlots
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

    return CurrentEntitiesIterator<Types...>(m_parent, m_parent->m_vecEntitySlots.size());
}


template<typename ...Types>
inline CEntityQuery::IncomingEntitiesIterator<Types...> CEntityQuery::Iterators::Incoming::begin()
{
    if(!m_parent->m_supplier->testSignature(CEntitySignature::from<Types...>()))
    {
        throw std::runtime_error("Iterator not compatible with the query");
    }

    return IncomingEntitiesIterator<Types...>(m_parent, 0);    
}

template<typename ...Types>
inline CEntityQuery::IncomingEntitiesIterator<Types...> CEntityQuery::Iterators::Incoming::end()
{
    if(!m_parent->m_supplier->testSignature(CEntitySignature::from<Types...>()))
    {
        throw std::runtime_error("Iterator not compatible with the query");
    }

    return IncomingEntitiesIterator<Types...>(m_parent, m_parent->m_vecIncomingEntitySlots.size());    
}


inline CEntityQuery::OutgoingEntitiesIterator CEntityQuery::Iterators::Outgoing::begin()
{
    return OutgoingEntitiesIterator(m_parent, 0);    
}

inline CEntityQuery::OutgoingEntitiesIterator CEntityQuery::Iterators::Outgoing::end()
{
    return OutgoingEntitiesIterator(m_parent, m_parent->m_vecOutgoingEntitySlots.size());    
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

    std::vector<size_t> indices(m_vecEntitySlots.size());
    std::iota(indices.begin(), indices.end(), 0);

    std::stable_sort(indices.begin(), indices.end(),
        [this, &comparator](size_t idx1, size_t idx2) -> bool {
            Iterator<Types...> it1(this, idx1);
            Iterator<Types...> it2(this, idx2);

            return comparator(it1, it2);
        }
    );

    std::vector<entityslot_t> sortedEnts(m_vecEntitySlots.size());

    for (size_t i = 0; i < m_vecEntitySlots.size(); i++)
    {
        sortedEnts[i] = this->m_vecEntitySlots[indices[i]];
    }

    this->m_vecEntitySlots = std::move(sortedEnts);
}

} // namespace chestnut::ecs
